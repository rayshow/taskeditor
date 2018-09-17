#include"LuaImportFactory.h"


ULuaImportFactory::ULuaImportFactory(FObjectInitializer const& Init)
	:Super(Init) 
{
	bEditorImport = true;
	Formats.Add("lua;lua file");
	SupportedClass = ULuaFileRefObject::StaticClass();
	bText = false;
}