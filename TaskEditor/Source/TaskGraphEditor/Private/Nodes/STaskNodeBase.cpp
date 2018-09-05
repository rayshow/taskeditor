// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "Nodes/STaskNodeBase.h"
#include "Engine/Engine.h"
#include "Internationalization/Culture.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SBoxPanel.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SToolTip.h"
#include "EngineGlobals.h"
#include "GraphEditorSettings.h"
#include "SCommentBubble.h"
#include "SGraphPin.h"
#include "Engine/Breakpoint.h"
#include "IDocumentation.h"
#include "TutorialMetaData.h"
#include "Widgets/Layout/SBox.h"
#include "TaskSystemGraphNode.h"
#include "TaskSystem.h"
#include "Utils/TaskSystemUtils.h"

#define LOCTEXT_NAMESPACE "STaskNodeBase"

//////////////////////////////////////////////////////////////////////////
// STaskNodeBase

const FLinearColor STaskNodeBase::BreakpointHitColor(0.7f, 0.0f, 0.0f);
const FLinearColor STaskNodeBase::LatentBubbleColor(1.f, 0.5f, 0.25f);
const FLinearColor STaskNodeBase::TimelineBubbleColor(0.7f, 0.5f, 0.5f);
const FLinearColor STaskNodeBase::PinnedWatchColor(0.35f, 0.25f, 0.25f);

void STaskNodeBase::UpdateStandardNode()
{
	SGraphNode::UpdateGraphNode();
	// clear the default tooltip, to make room for our custom "complex" tooltip
	SetToolTip(NULL);
}

void STaskNodeBase::UpdateCompactNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// error handling set-up
	SetupErrorReporting();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	TSharedPtr< SToolTip > NodeToolTip = SNew(SToolTip);
	if (!GraphNode->GetTooltipText().IsEmpty())
	{
		NodeToolTip = IDocumentation::Get()->CreateToolTip(TAttribute< FText >(this,
			&SGraphNode::GetNodeTooltip), NULL, GraphNode->GetDocumentationLink(), GraphNode->GetDocumentationExcerptName());
	}

	// Setup a meta tag for this node
	FGraphNodeMetaData TagMeta(TEXT("Graphnode"));
	PopulateMetaTag(&TagMeta);

	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode)
		.Text(this, &STaskNodeBase::GetNodeCompactTitle);

	TSharedRef<SOverlay> NodeOverlay = SNew(SOverlay);

	// add optional node specific widget to the overlay:
	TSharedPtr<SWidget> OverlayWidget = GraphNode->CreateNodeImage();
	if (OverlayWidget.IsValid())
	{
		NodeOverlay->AddSlot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(70.f)
				.HeightOverride(70.f)
				[
					OverlayWidget.ToSharedRef()
				]
			];
	}

	NodeOverlay->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(45.f, 0.f, 45.f, 0.f)
		[
			// MIDDLE
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "Graph.CompactNode.Title")
				.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
				.WrapTextAt(128.0f)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				NodeTitle.ToSharedRef()
			]
		];

	NodeOverlay->AddSlot()
	.HAlign(HAlign_Left)
	.VAlign(VAlign_Center)
	.Padding(0.f, 0.f, 55.f, 0.f)
	[
		// LEFT
		SAssignNew(LeftNodeBox, SVerticalBox)
	];

	NodeOverlay->AddSlot()
	.HAlign(HAlign_Right)
	.VAlign(VAlign_Center)
	.Padding(55.f, 0.f, 0.f, 0.f)
	[
		// RIGHT
		SAssignNew(RightNodeBox, SVerticalBox)
	];

	//
	//             ______________________
	//            | (>) L |      | R (>) |
	//            | (>) E |      | I (>) |
	//            | (>) F |   +  | G (>) |
	//            | (>) T |      | H (>) |
	//            |       |      | T (>) |
	//            |_______|______|_______|
	//
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				// NODE CONTENT AREA
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("Graph.VarNode.Body"))
				]
				+ SOverlay::Slot()
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("Graph.VarNode.Gloss"))
				]
				+ SOverlay::Slot()
				.Padding(FMargin(0, 3))
				[
					NodeOverlay
				]
			]
			+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(5.0f, 1.0f))
				[
					ErrorReporting->AsWidget()
				]
		];

	CreatePinWidgets();

	// Hide pin labels
	for (auto InputPin : this->InputPins)
	{
		if (InputPin->GetPinObj()->ParentPin == nullptr)
		{
			InputPin->SetShowLabel(false);
		}
	}

	for (auto OutputPin : this->OutputPins)
	{
		if (OutputPin->GetPinObj()->ParentPin == nullptr)
		{
			OutputPin->SetShowLabel(false);
		}
	}

	// Create comment bubble
	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &STaskNodeBase::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];

	CreateInputSideAddButton(LeftNodeBox);
	CreateOutputSideAddButton(RightNodeBox);
}

TSharedPtr<SToolTip> STaskNodeBase::GetComplexTooltip()
{
	TSharedPtr<SToolTip> NodeToolTip;
	TSharedRef<SToolTip> DefaultToolTip = IDocumentation::Get()->CreateToolTip(TAttribute<FText>(this, &SGraphNode::GetNodeTooltip), NULL, GraphNode->GetDocumentationLink(), GraphNode->GetDocumentationExcerptName());

	struct LocalUtils
	{
		static EVisibility IsToolTipVisible(TSharedRef<STaskNodeBase> const NodeWidget)
		{
			return NodeWidget->GetNodeTooltip().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
		}

		static EVisibility IsToolTipHeadingVisible(TSharedRef<STaskNodeBase> const NodeWidget)
		{
			return NodeWidget->GetToolTipHeading().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
		}

		static bool IsInteractive()
		{
			const FModifierKeysState ModifierKeys = FSlateApplication::Get().GetModifierKeys();
			return (ModifierKeys.IsAltDown() && ModifierKeys.IsControlDown());
		}
	};
	TSharedRef<STaskNodeBase> ThisRef = SharedThis(this);

	TSharedPtr< SVerticalBox > VerticalBoxWidget;
	SAssignNew(NodeToolTip, SToolTip)
		.Visibility_Static(&LocalUtils::IsToolTipVisible, ThisRef)
		.IsInteractive_Static(&LocalUtils::IsInteractive)

		// Emulate text-only tool-tip styling that SToolTip uses when no custom content is supplied.  We want node tool-tips to 
		// be styled just like text-only tool-tips
		.BorderImage(FCoreStyle::Get().GetBrush("ToolTip.BrightBackground"))
		.TextMargin(FMargin(11.0f))
		[
			SAssignNew(VerticalBoxWidget, SVerticalBox)
			// heading container
		+ SVerticalBox::Slot()
		[
			SNew(SVerticalBox)
			.Visibility_Static(&LocalUtils::IsToolTipHeadingVisible, ThisRef)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(FEditorStyle::Get(), "Documentation.SDocumentationTooltipSubdued")
				.Text(this, &STaskNodeBase::GetToolTipHeading)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 2.f, 0.f, 5.f)
			[
				SNew(SBorder)
				// use the border's padding to actually create the horizontal line
				.Padding(1.f)
				.BorderImage(FEditorStyle::GetBrush(TEXT("Menu.Separator")))
			]
		]
	// tooltip body
		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				DefaultToolTip->GetContentWidget()
			]
		];

	// English speakers have no real need to know this exists.
	if (FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName() != TEXT("en"))
	{
		struct Local
		{
			static EVisibility GetNativeNodeNameVisibility()
			{
				return FSlateApplication::Get().GetModifierKeys().IsAltDown() ? EVisibility::Collapsed : EVisibility::Visible;
			}
		};

		VerticalBoxWidget->AddSlot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				.Text(LOCTEXT("NativeNodeName", "hold (Alt) for native node name"))
				.TextStyle(&FEditorStyle::GetWidgetStyle<FTextBlockStyle>(TEXT("Documentation.SDocumentationTooltip")))
				.Visibility_Static(&Local::GetNativeNodeNameVisibility)
			];
	}
	return NodeToolTip;
}

FText STaskNodeBase::GetToolTipHeading() const
{
	if (UTaskSystemGraphNode_Base const* Node = CastChecked<UTaskSystemGraphNode_Base>(GraphNode))
	{
		return Node->GetToolTipHeading();
	}
	return FText::GetEmpty();
}

void STaskNodeBase::UpdateGraphNode()
{
	UTaskSystemGraphNode_Base* Node = CastChecked<UTaskSystemGraphNode_Base>(GraphNode);
	const bool bCompactMode = Node->ShouldDrawCompact();

	if (bCompactMode)
	{
		UpdateCompactNode();
	}
	else
	{
		UpdateStandardNode();
	}
}

bool STaskNodeBase::RequiresSecondPassLayout() const
{
	UTaskSystemGraphNode_Base* K2Node = CastChecked<UTaskSystemGraphNode_Base>(GraphNode);
	const bool bBeadMode = K2Node->ShouldDrawAsBead();

	return bBeadMode;
}

FText STaskNodeBase::GetNodeCompactTitle() const
{
	UTaskSystemGraphNode_Base* K2Node = CastChecked<UTaskSystemGraphNode_Base>(GraphNode);
	return K2Node->GetCompactNodeTitle();
}

/** Populate the brushes array with any overlay brushes to render */
void STaskNodeBase::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const
{
	UTaskSystem* OwnerTaskSystem = FTaskUtils::GetResourceFromNode(GraphNode);

	// Display an  icon depending on the type of node and it's settings
	if (const UTaskSystemGraphNode_Base* Node = Cast<const UTaskSystemGraphNode_Base>(GraphNode))
	{
		FName ClientIcon = Node->GetCornerIcon();
		if (ClientIcon != NAME_None)
		{
			FOverlayBrushInfo IPOverlayInfo;

			IPOverlayInfo.Brush = FEditorStyle::GetBrush(ClientIcon);

			if (IPOverlayInfo.Brush)
			{
				IPOverlayInfo.OverlayOffset.X = (WidgetSize.X - (IPOverlayInfo.Brush->ImageSize.X / 2.f)) - 3.f;
				IPOverlayInfo.OverlayOffset.Y = (IPOverlayInfo.Brush->ImageSize.Y / -2.f) + 2.f;
			}

			Brushes.Add(IPOverlayInfo);
		}
	}
}

void STaskNodeBase::GetNodeInfoPopups(FNodeInfoContext* Context, TArray<FGraphInformationPopupInfo>& Popups) const
{
}

const FSlateBrush* STaskNodeBase::GetShadowBrush(bool bSelected) const
{
	const UTaskSystemGraphNode_Base* K2Node = CastChecked<UTaskSystemGraphNode_Base>(GraphNode);
	const bool bCompactMode = K2Node->ShouldDrawCompact();

	if (bSelected && bCompactMode)
	{
		return FEditorStyle::GetBrush("Graph.VarNode.ShadowSelected");
	}
	else
	{
		return SGraphNode::GetShadowBrush(bSelected);
	}
}

void STaskNodeBase::PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const
{
	TSet<UEdGraphNode*> PrevNodes;
	TSet<UEdGraphNode*> NextNodes;

	// Gather predecessor/successor nodes
	for (int32 PinIndex = 0; PinIndex < GraphNode->Pins.Num(); ++PinIndex)
	{
		UEdGraphPin* Pin = GraphNode->Pins[PinIndex];

		if (Pin->LinkedTo.Num() > 0)
		{
			if (Pin->Direction == EGPD_Input)
			{
				for (int32 LinkIndex = 0; LinkIndex < Pin->LinkedTo.Num(); ++LinkIndex)
				{
					PrevNodes.Add(Pin->LinkedTo[LinkIndex]->GetOwningNode());
				}
			}

			if (Pin->Direction == EGPD_Output)
			{
				for (int32 LinkIndex = 0; LinkIndex < Pin->LinkedTo.Num(); ++LinkIndex)
				{
					NextNodes.Add(Pin->LinkedTo[LinkIndex]->GetOwningNode());
				}
			}
		}
	}

	// Place this node smack between them
	const float Height = 0.0f;
	PositionThisNodeBetweenOtherNodes(NodeToWidgetLookup, PrevNodes, NextNodes, Height);
}



#undef LOCTEXT_NAMESPACE
