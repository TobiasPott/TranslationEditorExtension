// Copyright 2025 Tobias Pott. All rights reserved.

#include "TranslationEditorExtensionCommands.h"

#define LOCTEXT_NAMESPACE "FTranslationEditorExtensionModule"

void FTranslationEditorExtensionCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Reviewed", "Execute TranslationEditorExtension action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
