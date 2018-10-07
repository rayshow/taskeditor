#include"Exports/XlsxExporter.h"
#include"FileHelper.h"
#include"Excel.h"

bool XlsxExporter::ExportModule(ExportItem& Item)
{

	FString ModuleName = Item.ModuleName;
	int DotPos;
	if (ModuleName.FindLastChar(TEXT('.'), DotPos))
	{
		FString OutFileName = ModuleName.Mid(0, DotPos) + "xlsx";
		FString AbsPath = FPaths::GameContentDir() + OutFileName;
		Excel ex;
		ex.Value("");
	}
	

	return true;
}