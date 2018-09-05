#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNode.h"
#include "TaskSystemGraphNode.generated.h"

UCLASS(MinimalAPI)
class UTaskSystemGraphNode_Base : public UEdGraphNode
{
public:

	GENERATED_UCLASS_BODY()

	/** Create all of the input pins required */
	virtual void CreateInputPins() {};
	/** Create all of the output pins required */
	virtual void CreateOutputPins() {};
	/** Is this the undeletable root node */
	virtual bool IsRootNode() const { return false; }
	/** Get a single Input Pin via its index */
	class UEdGraphPin* GetInputPin(int32 InputIndex) const;
	/** Get all of the Input Pins */
	void GetInputPins(TArray<class UEdGraphPin*>& OutInputPins) const;
	/** Get a single Output Pin via its index */
	class UEdGraphPin* GetOutputPin(int32 OutputIndex) const;
	/** Get all of the Output Pins */
	void GetOutputPins(TArray<class UEdGraphPin*>& OutOutputPins) const;
	/** Replace a given node with this one, changing all pin links */
	void ReplaceNode(UTaskSystemGraphNode_Base* OldNode);

	/** Get the Material Expression input index from an input pin */
	virtual int32 GetInputIndex(const UEdGraphPin* InputPin) const { return -1; }
	/** Get the Material value type of an input pin */
	virtual uint32 GetInputType(const UEdGraphPin* InputPin) const;

	/**
	* Handles inserting the node between the FromPin and what the FromPin was original connected to
	*
	* @param FromPin			The pin this node is being spawned from
	* @param NewLinkPin		The new pin the FromPin will connect to
	* @param OutNodeList		Any nodes that are modified will get added to this list for notification purposes
	*/
	void InsertNewNode(UEdGraphPin* FromPin, UEdGraphPin* NewLinkPin, TSet<UEdGraphNode*>& OutNodeList);

	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual void ReconstructNode() override;
	virtual void RemovePinAt(const int32 PinIndex, const EEdGraphPinDirection PinDirection) override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	//virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual FString GetDocumentationLink() const override { return FString(); }
	//~ End UEdGraphNode Interface.


	virtual FText GetToolTipHeading() const { return FText::FromString("HelloHeading"); };
	virtual bool ShouldDrawCompact() const { return false; }
	virtual bool ShouldDrawAsBead() const { return false; }
	virtual FText GetCompactNodeTitle() const { return GetNodeTitle(ENodeTitleType::FullTitle); }
	virtual FName GetCornerIcon() const { return FName(); }

protected:
	void ModifyAndCopyPersistentPinData(UEdGraphPin& TargetPin, const UEdGraphPin& SourcePin) const;
};


UCLASS(MinimalAPI)
class UTaskSystemGraphNode : public UTaskSystemGraphNode_Base
{
	GENERATED_UCLASS_BODY()

	/** Material Expression this node is representing */
	UPROPERTY()
	class UTaskSystemExpression* Expression;

	/** Set to true when Expression Preview compiles, so we can update SGraphNode */
	bool bPreviewNeedsUpdate;

	/** Set to true if this expression causes an error in the material */
	bool bIsErrorExpression;

	/** Set to true if this expression is currently being previewed */
	bool bIsPreviewExpression;

	///** Checks if Material Editor is in realtime mode, so we update SGraphNode every frame */
	//FRealtimeStateGetter RealtimeDelegate;

	///** Marks the Material Editor as dirty so that user prompted to apply change */
	//FSetMaterialDirty MaterialDirtyDelegate;

	/** Called when the preview material attached to this graph node needs to be updated */
	FSimpleDelegate InvalidatePreviewMaterialDelegate;

public:
	/** Fix up the node's owner after being copied */
	void PostCopyNode();

	/** Recreate this node's pins and relink the Graph from the Material */
	void RecreateAndLinkNode();

	/** Get the Material Expression output index from an output pin */
	int32 GetOutputIndex(const UEdGraphPin* OutputPin);
	/** Get the Material value type of an output pin */
	uint32 GetOutputType(const UEdGraphPin* OutputPin);

	//~ Begin UObject Interface
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override {}
	virtual void PostEditImport() override{}
	virtual void PostDuplicate(bool bDuplicateForPIE) override{}
	//~ End UObject Interface

	//~ Begin UEdGraphNode Interface.
	virtual bool CanPasteHere(const UEdGraph* TargetGraph) const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual void PrepareForCopying() override;
	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;

	virtual void PostPlacedNewNode() override;
	virtual void NodeConnectionListChanged() override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual void OnUpdateCommentText(const FString& NewComment) override;
	virtual void OnCommentBubbleToggled(bool bInCommentBubbleVisible) override;
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	virtual FString GetDocumentationExcerptName() const override;
	virtual bool CanUserDeleteNode() const override;
	//virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	//~ End UEdGraphNode Interface.

	//~ Begin UMaterialGraphNode_Base Interface
	virtual void CreateInputPins() override;
	virtual void CreateOutputPins() override;
	virtual int32 GetInputIndex(const UEdGraphPin* InputPin) const override;
	virtual uint32 GetInputType(const UEdGraphPin* InputPin) const override;
	//~ End UMaterialGraphNode_Base Interface

	/** Will return the shorten pin name to use based on long pin name */
	static FName GetShortenPinName(const FName PinName) { return PinName; }


private:
	/** Make sure the MaterialExpression is owned by the Material */
	void ResetMaterialExpressionOwner();

	/** Get the parameter name from the Material Expression */
	FString GetParameterName() const { return FString{ "Parameters" }; }

	/** Sets the Material Expression's parameter name */
	void SetParameterName(const FString& NewName) {}

	/** Should expression use the bool pin colour for its title */
	static bool UsesBoolColour(UTaskSystemExpression* Expression);

	/** Should expression use the float pin colour for its title */
	static bool UsesFloatColour(UTaskSystemExpression* Expression);

	/** Should expression use the vector pin colour for its title */
	static bool UsesVectorColour(UTaskSystemExpression* Expression);

	/** Should expression use the object pin colour for its title */
	static bool UsesObjectColour(UTaskSystemExpression* Expression);

	/** Should expression use the event node colour for its title */
	static bool UsesEventColour(UTaskSystemExpression* Expression);
};

