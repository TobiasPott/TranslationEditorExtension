// Copyright 2025 Tobias Pott. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "TranslationEditorExtensionStyle.h"

class FTranslationEditorExtensionCommands : public TCommands<FTranslationEditorExtensionCommands>
{
public:

	FTranslationEditorExtensionCommands()
		: TCommands<FTranslationEditorExtensionCommands>(TEXT("TranslationEditorExtension"), NSLOCTEXT("Contexts", "TranslationEditorExtension", "TranslationEditorExtension Plugin"), NAME_None, FTranslationEditorExtensionStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
