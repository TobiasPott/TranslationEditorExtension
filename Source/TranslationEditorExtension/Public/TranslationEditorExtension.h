// Copyright 2025 Tobias Pott. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FTranslationEditorExtensionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();
	/**
	 * Traverses the parent-side hierarchy of the given widget and checks if it exists in Parent hierarchy (searching up from Widget to its root)
	 * @param Widget The widget to start from (the nested one to check if it is a grandchild of the given parent)
	 * @param Parent The widget to be assumed somewhere in the hierarchy above Widget
	 * @return true if Widget is a grandchild of Parent, false otherwise
	 */
	static bool IsGrandChildOf(const TSharedPtr<SWidget>& Widget, const TSharedPtr<SWidget>& Parent);
	
private:

	void RegisterMenus();

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
