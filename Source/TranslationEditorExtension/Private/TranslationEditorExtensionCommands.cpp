// Copyright 2025 Tobias Pott. All rights reserved.

#include "TranslationEditorExtensionCommands.h"

#define LOCTEXT_NAMESPACE "FTranslationEditorExtensionModule"

void FTranslationEditorExtensionCommands::RegisterCommands()
{
	UI_COMMAND(ToggleAllReviewedAction, "Reviewed", "Toggle 'Has Been Reviewed' on all entries", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(TransferSourceAction, "Transfer Source", "Copies the current source to translation (requires focus on the tab you want to work on).", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
