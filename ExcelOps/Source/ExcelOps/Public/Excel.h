#pragma once


#if WITH_EXCELOPS_ENABLE
#include"xlnt/xlnt.hpp"
#endif

#include"CoreMinimal.h"


class Excel
{
public:
	//new Excel
	Excel()
		:Success{false}
#if WITH_EXCELOPS_ENABLE
		,wb{}
	{
		ws = wb.active_sheet();
		Success = true;
#else
	{
#endif
	}

	//Read Excel
	Excel(const FString& Name)
		:Success {false}
#if WITH_EXCELOPS_ENABLE
		,wb{}
	{
		try {
			wb.load(*Name);
			ws = wb.active_sheet();
		}
		catch(...){
			return;
		}
		Success = true;
#else
	{
#endif
	}

	Excel& Value(const FString& Pos, TCHAR const* Val)
	{
#if WITH_EXCELOPS_ENABLE
		ws.cell(TCHAR_TO_UTF8(*Pos)).value(TCHAR_TO_UTF8(Val));
#endif
		return *this;
	}

	Excel& Value(const FString& Pos, FString const& Val)
	{
		return Value(Pos, *Val);
	}

	template<typename T>
	Excel& Value(const FString& Pos, T const& Val) {
#if WITH_EXCELOPS_ENABLE
		ws.cell(TCHAR_TO_UTF8(*Pos)).value(Val);
#endif
		return *this;
	}

	Excel& ClearCell(const FString& Pos) {
#if WITH_EXCELOPS_ENABLE
		ws.clear_cell(TCHAR_TO_UTF8(*Pos));
#endif
		return *this;
	}

	template<typename T>
	T Value(const FString& Pos) {
#if WITH_EXCELOPS_ENABLE
		return ws.cell(TCHAR_TO_UTF8(*Pos)).value<T>();
#else
		return T();
#endif
	}

	template<>
	FString Value<FString>(const FString& Pos) {
#if WITH_EXCELOPS_ENABLE
		return UTF8_TO_TCHAR(ws.cell(TCHAR_TO_UTF8(*Pos)).value<std::string>().c_str());
#else
		return FString();
#endif
	}


	bool CellExists(const FString& Pos) {
#if WITH_EXCELOPS_ENABLE
		return ws.has_cell(TCHAR_TO_UTF8(*Pos));
#else
		return false;
#endif
	}

	void Save(const FString& Name) {
#if WITH_EXCELOPS_ENABLE
		wb.save(*Name);
#endif
	}


	operator bool() { return Success; }

private:

#if WITH_EXCELOPS_ENABLE
	xlnt::workbook wb;
	xlnt::worksheet ws;
#endif
	bool Success;
};