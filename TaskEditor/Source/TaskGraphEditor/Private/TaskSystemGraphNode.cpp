#pragma once

#include"TaskSystemGraphNode.h"
#include"TaskSystemGraph.h"
#include"TaskSystemGraphSchema.h"
#include"Expression/TaskSystemExpression.h"
#include"Expression/TaskSystemExpressionInput.h"
#include"Expression/TaskSystemExpressionOutput.h"
#include"EdGraph/EdGraphPin.h"
#include"EdGraph/EdGraphNode.h"
#include"SEditableTextBox.h"
#include"GenericCommands.h"
#include"MultiBoxBuilder.h"
#include"GraphEditorActions.h"


#define LOCTEXT_NAMESPACE "TaskEditor"

UTaskSystemGraphNode_Base::UTaskSystemGraphNode_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {}


UEdGraphPin* UTaskSystemGraphNode_Base::GetInputPin(int32 InputIndex) const
{
	for (int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Input)
		{
			if (InputIndex == FoundInputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundInputs++;
			}
		}
	}
	return nullptr;
}

void UTaskSystemGraphNode_Base::GetInputPins(TArray<UEdGraphPin*>& OutInputPins) const
{
	OutInputPins.Empty();

	for (int32 PinIndex = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Input)
		{
			OutInputPins.Add(Pins[PinIndex]);
		}
	}
}

UEdGraphPin* UTaskSystemGraphNode_Base::GetOutputPin(int32 OutputIndex) const
{
	for (int32 PinIndex = 0, FoundOutputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Output)
		{
			if (OutputIndex == FoundOutputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundOutputs++;
			}
		}
	}
	return NULL;
}

void UTaskSystemGraphNode_Base::GetOutputPins(TArray<class UEdGraphPin*>& OutOutputPins) const
{
	OutOutputPins.Empty();

	for (int32 PinIndex = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Output)
		{
			OutOutputPins.Add(Pins[PinIndex]);
		}
	}
}

void UTaskSystemGraphNode_Base::ModifyAndCopyPersistentPinData(UEdGraphPin& TargetPin, const UEdGraphPin& SourcePin) const
{
	if (SourcePin.LinkedTo.Num() > 0)
	{
		TargetPin.Modify();

		for (int32 LinkIndex = 0; LinkIndex < SourcePin.LinkedTo.Num(); ++LinkIndex)
		{
			UEdGraphPin* OtherPin = SourcePin.LinkedTo[LinkIndex];
			OtherPin->Modify();
		}
	}

	TargetPin.CopyPersistentDataFromOldPin(SourcePin);
}


void UTaskSystemGraphNode_Base::ReplaceNode(UTaskSystemGraphNode_Base* OldNode)
{
	check(OldNode);
	check(OldNode != this);

	// Get Pins from node passed in
	TArray<UEdGraphPin*> OldInputPins;
	TArray<UEdGraphPin*> OldOutputPins;

	OldNode->GetInputPins(OldInputPins);
	OldNode->GetOutputPins(OldOutputPins);

	// Get our Input and Output pins
	TArray<UEdGraphPin*> NewInputPins;
	TArray<UEdGraphPin*> NewOutputPins;
	GetInputPins(NewInputPins);
	GetOutputPins(NewOutputPins);

	// Copy Inputs from old node
	for (int32 PinIndex = 0; PinIndex < OldInputPins.Num(); PinIndex++)
	{
		if (PinIndex < NewInputPins.Num())
		{
			ModifyAndCopyPersistentPinData(*NewInputPins[PinIndex], *OldInputPins[PinIndex]);
		}
	}

	// Copy Outputs from old node
	for (int32 PinIndex = 0; PinIndex < OldOutputPins.Num(); PinIndex++)
	{
		// If we can't find an equivalent output in this node, just use the first
		// The user will have to fix up any issues from the mismatch
		int32 FoundPinIndex = 0;

		// Try to find an equivalent output in this node
		for (int32 NewPinIndex = 0; NewPinIndex < NewOutputPins.Num(); NewPinIndex++)
		{
			if (OldOutputPins[PinIndex]->PinType == NewOutputPins[NewPinIndex]->PinType)
			{
				FoundPinIndex = NewPinIndex;
				break;
			}
		}

		if (FoundPinIndex < NewOutputPins.Num())
		{
			ModifyAndCopyPersistentPinData(*NewOutputPins[FoundPinIndex], *OldOutputPins[PinIndex]);
		}
	}

	// Break the original pin links
	for (int32 OldPinIndex = 0; OldPinIndex < OldNode->Pins.Num(); ++OldPinIndex)
	{
		UEdGraphPin* OldPin = OldNode->Pins[OldPinIndex];
		OldPin->Modify();
		OldPin->BreakAllPinLinks();
	}
}


uint32 UTaskSystemGraphNode_Base::GetInputType(const UEdGraphPin* InputPin) const { return 0; }


void UTaskSystemGraphNode_Base::InsertNewNode(
	UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList)
{
	const UTaskSystemGraphSchema* Schema = CastChecked<UTaskSystemGraphSchema>(GetSchema());

	// The pin we are creating from already has a connection that needs to be broken. We want to "insert" the new node in between, so that the output of the new node is hooked up too
	UEdGraphPin* OldLinkedPin = FromPin->LinkedTo[0];
	check(OldLinkedPin);

	FromPin->BreakAllPinLinks();

	// Hook up the old linked pin to the first valid output pin on the new node
	for (int32 OutpinPinIdx = 0; OutpinPinIdx<Pins.Num(); OutpinPinIdx++)
	{
		UEdGraphPin* OutputPin = Pins[OutpinPinIdx];
		check(OutputPin);
		if (ECanCreateConnectionResponse::CONNECT_RESPONSE_MAKE == Schema->CanCreateConnection(OldLinkedPin, OutputPin).Response)
		{
			if (Schema->TryCreateConnection(OldLinkedPin, OutputPin))
			{
				OutNodeList.Add(OldLinkedPin->GetOwningNode());
				OutNodeList.Add(this);
			}
			break;
		}
	}

	if (Schema->TryCreateConnection(FromPin, NewLinkPin))
	{
		OutNodeList.Add(FromPin->GetOwningNode());
		OutNodeList.Add(this);
	}
}

void UTaskSystemGraphNode_Base::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	CreateInputPins();
	CreateOutputPins();
}


void UTaskSystemGraphNode_Base::ReconstructNode()
{
	Modify();
	// Break any links to 'orphan' pins
	for (int32 PinIndex = 0; PinIndex < Pins.Num(); ++PinIndex)
	{
		UEdGraphPin* Pin = Pins[PinIndex];
		TArray<class UEdGraphPin*>& LinkedToRef = Pin->LinkedTo;
		for (int32 LinkIdx = 0; LinkIdx < LinkedToRef.Num(); LinkIdx++)
		{
			UEdGraphPin* OtherPin = LinkedToRef[LinkIdx];
			// If we are linked to a pin that its owner doesn't know about, break that link
			if (!OtherPin->GetOwningNode()->Pins.Contains(OtherPin))
			{
				Pin->LinkedTo.Remove(OtherPin);
			}
		}
	}

	TArray<UEdGraphPin*> OldInputPins;
	TArray<UEdGraphPin*> OldOutputPins;
	GetInputPins(OldInputPins);
	GetOutputPins(OldOutputPins);

	// Move the existing pins to a saved array
	TArray<UEdGraphPin*> OldPins(Pins);
	Pins.Reset();

	// Recreate the new pins
	AllocateDefaultPins();

	// Get new Input and Output pins
	TArray<UEdGraphPin*> NewInputPins;
	TArray<UEdGraphPin*> NewOutputPins;
	GetInputPins(NewInputPins);
	GetOutputPins(NewOutputPins);

	for (int32 PinIndex = 0; PinIndex < OldInputPins.Num(); PinIndex++)
	{
		if (PinIndex < NewInputPins.Num())
		{
			NewInputPins[PinIndex]->MovePersistentDataFromOldPin(*OldInputPins[PinIndex]);
		}
	}

	for (int32 PinIndex = 0; PinIndex < OldOutputPins.Num(); PinIndex++)
	{
		if (PinIndex < NewOutputPins.Num())
		{
			NewOutputPins[PinIndex]->MovePersistentDataFromOldPin(*OldOutputPins[PinIndex]);
		}
	}

	// Throw away the original pins
	for (UEdGraphPin* OldPin : OldPins)
	{
		OldPin->Modify();
		UEdGraphNode::DestroyPin(OldPin);
	}

	GetGraph()->NotifyGraphChanged();
}

void UTaskSystemGraphNode_Base::RemovePinAt(const int32 PinIndex, const EEdGraphPinDirection PinDirection)
{
	Super::RemovePinAt(PinIndex, PinDirection);
	UTaskSystemGraph* TaskSystemGraph = CastChecked<UTaskSystemGraph>(GetGraph());
	TaskSystemGraph->LinkTaskSystemExpressionsFromGraph();
}


void UTaskSystemGraphNode_Base::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin != NULL)
	{
		const UTaskSystemGraphSchema* Schema = CastChecked<UTaskSystemGraphSchema>(GetSchema());

		TSet<UEdGraphNode*> NodeList;

		// auto-connect from dragged pin to first compatible pin on the new node
		for (int32 i = 0; i<Pins.Num(); i++)
		{
			UEdGraphPin* Pin = Pins[i];
			check(Pin);
			FPinConnectionResponse Response = Schema->CanCreateConnection(FromPin, Pin);
			if (ECanCreateConnectionResponse::CONNECT_RESPONSE_MAKE == Response.Response)
			{
				if (Schema->TryCreateConnection(FromPin, Pin))
				{
					NodeList.Add(FromPin->GetOwningNode());
					NodeList.Add(this);
				}
				break;
			}
			else if (ECanCreateConnectionResponse::CONNECT_RESPONSE_BREAK_OTHERS_A == Response.Response)
			{
				InsertNewNode(FromPin, Pin, NodeList);
				break;
			}
		}

		// Send all nodes that received a new pin connection a notification
		for (auto It = NodeList.CreateConstIterator(); It; ++It)
		{
			UEdGraphNode* Node = (*It);
			Node->NodeConnectionListChanged();
		}
	}
}

void UTaskSystemGraphNode::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	UTaskSystemGraph* Graph = CastChecked<UTaskSystemGraph>(GetGraph());

	if (Context.Node)
	{
		// Break all links
		Context.MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
		Context.MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().HideNoConnectionPins);
		Context.MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().ShowAllPins);
		 
		Context.MenuBuilder->BeginSection("TaskEditorNodeOperation");
		{
			Context.MenuBuilder->AddMenuEntry(FGenericCommands::Get().Delete);
			Context.MenuBuilder->AddMenuEntry(FGenericCommands::Get().Cut);
			Context.MenuBuilder->AddMenuEntry(FGenericCommands::Get().Copy);
			Context.MenuBuilder->AddMenuEntry(FGenericCommands::Get().Duplicate);
		}
		Context.MenuBuilder->EndSection();
	}

}

bool UTaskSystemGraphNode_Base::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA(UTaskSystemGraphSchema::StaticClass());
}

UTaskSystemGraphNode::UTaskSystemGraphNode(FObjectInitializer const& Initializer)
	: UTaskSystemGraphNode_Base(Initializer) {}


void UTaskSystemGraphNode::PostCopyNode() {

}


//TSharedRef<SWidget>	UTaskSystemGraphNode::GetDefaultValueWidget()
//{
//	return SNew(SEditableTextBox);
//}

void UTaskSystemGraphNode::RecreateAndLinkNode() {
	// Throw away the original pins
	TMap<FName, TArray<UEdGraphPin*>> RecordLinks;
	for (int32 PinIndex = 0; PinIndex < Pins.Num(); ++PinIndex)
	{
		UEdGraphPin* Pin = Pins[PinIndex];
		Pin->Modify();
		//Pin->BreakAllPinLinks();
		for (auto ToPin : Pin->LinkedTo) {
			check(ToPin->LinkedTo.Find(Pin) != INDEX_NONE);
			ToPin->LinkedTo.Remove(Pin);
		}
		RecordLinks.Add(Pin->PinName, MoveTemp(Pin->LinkedTo));
		UEdGraphNode::DestroyPin(Pin);
	}
	Pins.Empty();

	AllocateDefaultPins();

	for (int32 PinIndex = 0; PinIndex < Pins.Num(); ++PinIndex)
	{
		UEdGraphPin* Pin = Pins[PinIndex];
		auto LinkTo = RecordLinks.Find(Pin->PinName);
		if (LinkTo) {
			for (auto ToPin : *LinkTo) {
				check(ToPin->LinkedTo.Find(Pin)==INDEX_NONE);
				ToPin->LinkedTo.Add(Pin);
			}
			Pin->LinkedTo = MoveTemp(*LinkTo);
		}
		
	}

	//CastChecked<UTaskSystemGraph>(GetGraph())->LinkTaskSystemExpressionsFromGraph();
}


int32 UTaskSystemGraphNode::GetOutputIndex(const UEdGraphPin* OutputPin)
{
	TArray<UEdGraphPin*> OutputPins;
	GetOutputPins(OutputPins);

	for (int32 Index = 0; Index < OutputPins.Num(); ++Index)
	{
		if (OutputPin == OutputPins[Index])
		{
			return Index;
		}
	}
	return -1;
}

uint32 UTaskSystemGraphNode::GetOutputType(const UEdGraphPin* OutputPin)
{
	return Expression->GetOutputType(GetOutputIndex(OutputPin));
}


//~ Begin UEdGraphNode Interface.
bool UTaskSystemGraphNode::CanPasteHere(const UEdGraph* TargetGraph) const
{
	return true;
}

FText UTaskSystemGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	TArray<FString> Captions;
	Expression->GetCaption(Captions);
 
	int32 CaptionIndex = Captions.Num() - 1;

	FTextBuilder NodeTitle;
	NodeTitle.AppendLine(Captions[CaptionIndex]);

	for (; CaptionIndex > 0; )
	{
		CaptionIndex--;
		NodeTitle.AppendLine(Captions[CaptionIndex]);
	}
	return NodeTitle.ToText();
	 
}

FLinearColor UTaskSystemGraphNode::GetNodeTitleColor() const
{
	return FLinearColor(0.69,0.227,0.596);
}

FText UTaskSystemGraphNode::GetTooltipText() const
{
	return LOCTEXT("TaskSystemNodeTitleTT", "Task System Node Title Tool Tip");
}

void UTaskSystemGraphNode::PrepareForCopying()
{
	if (Expression)
	{
		// Temporarily take ownership of the MaterialExpression, so that it is not deleted when cutting
		Expression->Rename(NULL, this, REN_DontCreateRedirectors);
	}
}

void UTaskSystemGraphNode::PostPlacedNewNode()
{
}

void UTaskSystemGraphNode::NodeConnectionListChanged()
{
	Super::NodeConnectionListChanged();

	const UEdGraphSchema* Schema = GetSchema();
	if (Schema != nullptr)
	{
		Schema->ForceVisualizationCacheClear();
	}
}

void UTaskSystemGraphNode::OnRenameNode(const FString& NewName)
{
	Expression->Modify(); 
	Expression->MarkPackageDirty();
}

void UTaskSystemGraphNode::OnUpdateCommentText(const FString& NewComment)
{

}

void UTaskSystemGraphNode::OnCommentBubbleToggled(bool bInCommentBubbleVisible)
{

}

void UTaskSystemGraphNode::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{

}

FString UTaskSystemGraphNode::GetDocumentationExcerptName() const
{
	return FString("DocumentExcerptName");
}

bool UTaskSystemGraphNode::CanUserDeleteNode() const
{
	return true;
}


void UTaskSystemGraphNode::CreateInputPins()
{
	const TArray<FTaskSystemExpressionInput*> ExpressionInputs = Expression->GetInputs();

	for (int32 Index = 0; Index < ExpressionInputs.Num(); ++Index)
	{
		FTaskSystemExpressionInput* Input = ExpressionInputs[Index];
		FName InputName = Input->InputName;

		InputName = GetShortenPinName(InputName);

		UEdGraphPin* NewPin = CreatePin(EGPD_Input,
			Input->InputEnumToName(),
			Input->InputSubEnumToName(),
			Expression, InputName);
		NewPin->PinType.PinSubCategoryObject = Expression;
		check(NewPin->PinType.PinSubCategoryObject.Get());
		if (NewPin->PinName.IsNone())
		{
			// Makes sure pin has a name for lookup purposes but user will never see it
			NewPin->PinName = CreateUniquePinName(TEXT("Input"));
			NewPin->PinFriendlyName = FText::FromString(TEXT(" "));
		}
	}
}

void UTaskSystemGraphNode::CreateOutputPins()
{
	TArray<FTaskSystemExpressionOutput>& Outputs = Expression->GetOutputs();
	for ( FTaskSystemExpressionOutput& ExpressionOutput : Outputs)
	{
		FName PinCategory;
		FName PinSubCategory;
		FName OutputName;


		//if (Expression->bShowOutputNameOnPin)
		{
			OutputName = ExpressionOutput.OutputName;
		}
		UEdGraphPin* NewPin = CreatePin(EGPD_Output, 
			ExpressionOutput.OutputEnumToName(), 
			ExpressionOutput.OutputSubEnumToName(),
			Expression, OutputName);
		check(NewPin->PinType.PinSubCategoryObject.Get());
		if (NewPin->PinName.IsNone())
		{
			// Makes sure pin has a name for lookup purposes but user will never see it
			NewPin->PinName = CreateUniquePinName(TEXT("Output"));
			NewPin->PinFriendlyName = FText::FromString(TEXT(" "));
		}
	}
}

int32 UTaskSystemGraphNode::GetInputIndex(const UEdGraphPin* InputPin) const
{
	TArray<UEdGraphPin*> InputPins;
	GetInputPins(InputPins);

	for (int32 Index = 0; Index < InputPins.Num(); ++Index)
	{
		if (InputPin == InputPins[Index])
		{
			return Index;
		}
	}

	return -1;
}

uint32 UTaskSystemGraphNode::GetInputType(const UEdGraphPin* InputPin) const
{
	return Expression->GetInputType(GetInputIndex(InputPin));
}



#undef LOCTEXT_NAMESPACE 