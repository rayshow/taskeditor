#pragma once

#include "Factories/Factory.h"
#include "Factories/ImportSettings.h"
#include "Paths.h"
#include "LuaFileRefObject.h"
#include "Editor.h"
#include "LuaImportFactory.generated.h"


#define LOCTEXT_NAMESPACE "LuaBrowser"

UCLASS(hidecategories = Object)
class ULuaImportFactory: public UFactory, public IImportSettingsParser
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
		OutExtensions.Add(TEXT("lua;lua file"));
	}

	virtual UObject* ImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, bool& OutCanceled)
	{
		ULuaFileRefObject* NewLuaRef = NewObject<ULuaFileRefObject>(InOuter, ULuaFileRefObject::StaticClass(), InName, Flags);
		NewLuaRef->RefPath = Filename;
		return InOuter;
	}


	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
		EObjectFlags Flags, const FString& Filename, const TCHAR* Parms,
		FFeedbackContext* Warn, bool& bOutOperationCanceled) override
	{
		FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Parms);
		ULuaFileRefObject* NewLuaRef = NewObject<ULuaFileRefObject>(InParent, ULuaFileRefObject::StaticClass(), InName, Flags);
		NewLuaRef->RefPath = Filename;
		return NewLuaRef;
	}
	

	virtual bool DoesSupportClass(UClass *Class) override {
		return Class->GetClass()->IsChildOf<ULuaFileRefObject>();
	}

	virtual bool FactoryCanImport(const FString& Filename) override {
		const FString Extension = FPaths::GetExtension(Filename);
		if (Extension == TEXT("lua")) {
			return true;
		}
		return false;
	}

	virtual	IImportSettingsParser* GetImportSettingsParser() override { return this; }

	virtual void ParseFromJson(TSharedRef<class FJsonObject> ImportSettingsJson) override
	{

	}
};

#undef LOCTEXT_NAMESPACE