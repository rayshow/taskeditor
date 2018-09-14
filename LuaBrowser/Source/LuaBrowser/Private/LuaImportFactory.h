#pragma once

#include "Factories/Factory.h"
#include "Paths.h"
#include "LuaFileRefObject.h"
#include "LuaImportFactory.generated.h"


#define LOCTEXT_NAMESPACE "LuaBrowser"

UCLASS(hidecategories = Object)
class ULuaImportFactory: public UFactory
{
public:
	GENERATED_UCLASS_BODY()

	virtual  FText GetDisplayName() const override
	{
		return LOCTEXT("CSVImportFactoryDescription", "Lua Importor");
	}

	bool ReimportCSV(UObject* Obj)
	{
		

		return true;
	}

	/** Creates a list of file extensions supported by this factory */
	void GetSupportedFileExtensions(TArray<FString>& OutExtensions) const
	{

	}

	virtual UObject* ImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, bool& OutCanceled)
	{
		ULuaFileRefObject* NewLuaRef = NewObject<ULuaFileRefObject>(InClass, ULuaFileRefObject::StaticClass(), InName, Flags);
		NewLuaRef->RefPath = Filename;
		return NewLuaRef;
	}

	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
		EObjectFlags Flags, const FString& Filename, const TCHAR* Parms,
		FFeedbackContext* Warn, bool& bOutOperationCanceled) override
	{
		ULuaFileRefObject* NewLuaRef = NewObject<ULuaFileRefObject>(InParent, ULuaFileRefObject::StaticClass(), InName, Flags);
		NewLuaRef->RefPath = Filename;
		return NewLuaRef;
	}
	

	virtual bool DoesSupportClass(UClass *Class) override {
		return false; 
	}

	virtual bool FactoryCanImport(const FString& Filename) override {
		const FString Extension = FPaths::GetExtension(Filename);
		if (Extension == TEXT("lua")) {
			return true;
		}
		return false;
	}


};

#undef LOCTEXT_NAMESPACE