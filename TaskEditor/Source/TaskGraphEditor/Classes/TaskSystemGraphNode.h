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
	
	// InputPin Operation
	virtual void CreateInputPins() {}; 
	class UEdGraphPin* GetInputPin(int32 InputIndex) const;
	void GetInputPins(TArray<class UEdGraphPin*>& OutInputPins) const;
	virtual int32 GetInputIndex(const UEdGraphPin* InputPin) const { return -1; }
	virtual uint32 GetInputType(const UEdGraphPin* InputPin) const;

	// OutPin Operation
	virtual void CreateOutputPins() {}; 
	class UEdGraphPin* GetOutputPin(int32 OutputIndex) const;
	void GetOutputPins(TArray<class UEdGraphPin*>& OutOutputPins) const;

	// Node Operation
	void ReplaceNode(UTaskSystemGraphNode_Base* OldNode);
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
public:
	GENERATED_UCLASS_BODY()
		 
	UPROPERTY()
	class UTaskSystemExpression* Expression;
  
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
};

