// Copyright 2025 Tobias Pott. All rights reserved.

#include "TranslationEditorExtensionStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FTranslationEditorExtensionStyle::StyleInstance = nullptr;

void FTranslationEditorExtensionStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FTranslationEditorExtensionStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FTranslationEditorExtensionStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("TranslationEditorExtensionStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon32x32(32.0f, 32.0f);

TSharedRef< FSlateStyleSet > FTranslationEditorExtensionStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("TranslationEditorExtensionStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("TranslationEditorExtension")->GetBaseDir() / TEXT("Resources"));

	Style->Set("TranslationEditorExtension.ToggleAllReviewedAction", new IMAGE_BRUSH_SVG(TEXT("ToggleAllReviewedIcon"), Icon20x20));
	Style->Set("TranslationEditorExtension.TransferSourceAction", new IMAGE_BRUSH_SVG(TEXT("TransferSourceIcon"), Icon20x20));
	Style->Set("TranslationEditorExtension.ClearTranslationAction", new IMAGE_BRUSH_SVG(TEXT("ClearTranslationIcon"), Icon20x20));
	return Style;
}

void FTranslationEditorExtensionStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FTranslationEditorExtensionStyle::Get()
{
	return *StyleInstance;
}
