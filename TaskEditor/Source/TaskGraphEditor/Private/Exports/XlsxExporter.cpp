#include"Exports/XlsxExporter.h"
#include"FileHelper.h"
#include"Excel.h"
#include"Expression/TaskSystemExpressionSubtask.h"
#include<exception>

static void MakeHead(Excel& ex)
{
	for (int i = 0; i < EP_CELL_SIZE; ++i)
	{
		FString PosEng = FString::Printf(*EP_CELLS[i][0], 1);
		FString PosChi = FString::Printf(*EP_CELLS[i][0], 2);
		ex.Value(PosEng, EP_CELLS[i][2]);
		ex.Value(PosChi, EP_CELLS[i][1]);
	}
}

bool XlsxExporter::ExportModule(ExportItem& Item)
{
	if (Item.Exprs.Num() == 0 ||
		Item.ModuleName.IsEmpty()) return true;

	FString ModuleName = Item.ModuleName.Mid(1,Item.ModuleName.Len()-1);
	int DotPos;
	int Start;
	ModuleName.FindChar(TEXT('/'), Start);
	if (ModuleName.FindLastChar(TEXT('.'), DotPos))
	{
		FString OutFileName = ModuleName.Mid(Start+1, DotPos- Start) + "xlsx";
		FString AbsPath = FPaths::GameContentDir() + OutFileName;
		Excel ex;
		MakeHead(ex);
		for (int i = 0; i < Item.Exprs.Num(); ++i)
		{
			auto Expr = Item.Exprs[i];
			Expr->SaveToExcel(ex,i);
		}
		try {
			ex.Save(AbsPath);
		}
		catch (...) {
			return false;
		}
		
	}
	return true;
}