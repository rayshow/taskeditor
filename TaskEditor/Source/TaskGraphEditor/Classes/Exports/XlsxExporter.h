#pragma once
#include "Excel.h"
#include "Object.h"
#include "Expression/TaskSystemExpression.h"

struct ExportItem
{
	FString                        ModuleName;
	TArray<UTaskSystemExpression*> Exprs;
};

class XlsxExporter
{
public:

	static bool ExportModule(ExportItem& Item);

};