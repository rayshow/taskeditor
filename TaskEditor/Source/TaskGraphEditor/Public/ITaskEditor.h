#pragma once

#include "Toolkits/AssetEditorToolkit.h"

class UEdGraph;
class UTaskSystemExpression;
class UtaskSystemExpressionComment;


class ITaskEditor: 
	public FAssetEditorToolkit,
	public IHasMenuExtensibility,
	public IHasToolBarExtensibility
{
	// Utilities Function
	//static UTaskSystemExpression* CreateNewTaskExpression(
	//	const UEdGraph* InGraph, UClass* InClass, const FVector2D& NodePos,
	//	bool AutoSelect, bool AutoAssignResource);
};