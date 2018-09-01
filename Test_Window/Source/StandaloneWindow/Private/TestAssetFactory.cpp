// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "TestAssetFactory.h"
#include "TestAsset.h"

#define LOCTEXT_NAMESPACE "MyObject"

/////////////////////////////////////////////////////
// UMyObjectFactory

UMyObjectFactory::UMyObjectFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UMyObject::StaticClass();
}

UObject* UMyObjectFactory::FactoryCreateNew(UClass* Class, UObject* InParent,
	FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UMyObject* NewObjectAsset = NewObject<UMyObject>(InParent, Class, Name, Flags);
	return NewObjectAsset;
}

#undef LOCTEXT_NAMESPACE