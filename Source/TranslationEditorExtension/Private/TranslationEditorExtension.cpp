// Copyright 2025 Tobias Pott. All rights reserved.

#include "TranslationEditorExtension.h"
#include "TranslationEditorExtensionStyle.h"
#include "TranslationEditorExtensionCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "IPropertyTableRow.h"
#include "Editor/PropertyEditor/Private/Presentation/PropertyTable/DataSource.h"
#include "Editor/TranslationEditor/Private/TranslationUnit.h"

static const FName TranslationEditorExtensionTabName("TranslationEditorExtension");
// Names and TabIds need to be checked with UnrealEngine's TranslationEditor internal names when porting this plugin to a different version
static const FName ReviewTabName("TranslationEditor_Review");
static const FTabId ReviewTabId(ReviewTabName);

// Names for where to place the extension and what to name its section
static const FName MenuUIExtensionPoint("AssetEditor.TranslationEditor.ToolBar");
static const FName PluginMenuSection("TranslationEditorExtension");


#define LOCTEXT_NAMESPACE "FTranslationEditorExtensionModule"

void FTranslationEditorExtensionModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FTranslationEditorExtensionStyle::Initialize();
	FTranslationEditorExtensionStyle::ReloadTextures();

	FTranslationEditorExtensionCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTranslationEditorExtensionCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FTranslationEditorExtensionModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTranslationEditorExtensionModule::RegisterMenus));
}

void FTranslationEditorExtensionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FTranslationEditorExtensionStyle::Shutdown();

	FTranslationEditorExtensionCommands::Unregister();
}

void FTranslationEditorExtensionModule::PluginButtonClicked()
{
	FSlateApplication& SlateApp = FSlateApplication::Get();
	const TSharedPtr<SWidget> FocusedWidget = SlateApp.GetKeyboardFocusedWidget();

	if (FocusedWidget.IsValid())
	{
		IAssetEditorInstance* CurrentEditor = nullptr;
		UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
		TArray<UObject*> EditedAssets = AssetEditorSubsystem->GetAllEditedAssets();

		for (int i = 0; i < EditedAssets.Num(); i++)
		{
			UTranslationUnit* TranslationUnit = Cast<UTranslationUnit>(EditedAssets[i]);
			if (IsValid(TranslationUnit))
				CurrentEditor = AssetEditorSubsystem->FindEditorForAsset(TranslationUnit, false);
			if (CurrentEditor != nullptr)
			{
				// find editor for the translation unit asset
				TSharedPtr<FTabManager> TabManager = CurrentEditor->GetAssociatedTabManager();
				TSharedPtr<SDockTab> ReviewTab = TabManager->FindExistingLiveTab(ReviewTabId);
				TSharedPtr<SWidget> ReviewContent = ReviewTab->GetContent();

				TSharedPtr<SWidget> ReviewParent = ReviewTab->GetParentWidget();
				if (IsGrandChildOf(ReviewParent, FocusedWidget))
				{
					FAssetEditorToolkit* EditorToolkit = static_cast <FAssetEditorToolkit*>(CurrentEditor);
					if (EditorToolkit != nullptr)
					{
						UE_LOG(LogTemp, Warning, TEXT("Working on: %s."), *EditorToolkit->GetToolkitName().ToString());
					}

					FChildren* ReviewContentChildren = ReviewContent->GetChildren();
					if (ReviewContentChildren->Num() > 0)
					{
						TSharedPtr<SWidget> FirstChild = ReviewContentChildren->GetChildAt(0);
						SWidget* ChildWidget = FirstChild.Get();

						// the first child of the TranslationEditor border is assumed to be a SPropertyTable (which is a private type in the PropertyEditor module)
						//	the SPropertyTable is derived from STreeView which provides access to its items and the referenced asset object
						STreeView<TSharedRef<IPropertyTableRow>>* PropertyTableWidget = static_cast<STreeView<TSharedRef<IPropertyTableRow>>*>(ChildWidget);
						if (PropertyTableWidget != nullptr)
						{
							TArrayView<const TSharedRef<IPropertyTableRow>> ItemView = PropertyTableWidget->GetRootItems();
							for (int ItemIdx = 0; ItemIdx < ItemView.Num(); ItemIdx++)
							{
								TSharedRef<IDataSource> Source = ItemView[ItemIdx]->GetDataSource();
								//FPropertyPath* PropPath = Source.Get().AsPropertyPath().Get();
								UObject* ItemObject = Source.Get().AsUObject().Get();

								UTranslationUnit* ItemTranslationUnit = Cast<UTranslationUnit>(ItemObject);
								if (ItemTranslationUnit != nullptr)
									ItemTranslationUnit->HasBeenReviewed = !ItemTranslationUnit->HasBeenReviewed;
								// Uncomment for additional info log of table rows
								// UE_LOG(LogTemp, Warning, TEXT("Item: %d. TableRow: %s"), ItemIdx, *UKismetSystemLibrary::GetDisplayName(PropObject));
							}

							PropertyTableWidget->RequestListRefresh();
							UE_LOG(LogTemp, Log, TEXT("PropertyTable (TreeView): %s. Rows in PropertyTable: %d"), *PropertyTableWidget->ToString(), ItemView.Num());

						}

					}
					if (EditorToolkit != nullptr)
					{
						UE_LOG(LogTemp, Warning, TEXT("Finished work on: %s."), *EditorToolkit->GetToolkitName().ToString());
					}
					break;
				}
			}
		}
	}

}

bool FTranslationEditorExtensionModule::IsGrandChildOf(const TSharedPtr<SWidget>& Widget, const TSharedPtr<SWidget>& Parent)
{
	// ToDo: Put this search into a function
	bool bEndSearch = false;
	TSharedPtr<SWidget> WidgetParent = Widget->GetParentWidget();
	while (WidgetParent.Get() != nullptr)
	{
		WidgetParent = WidgetParent->GetParentWidget();
		if (WidgetParent.Get() == Parent.Get())
		{
			if (Widget.IsValid() && Parent.IsValid()) { UE_LOG(LogTemp, Log, TEXT("IsGrandChildOf.Found: %s in path of %s."), *Widget->ToString(), *Parent->ToString()); }
			return true;
		}
	}
	if (Widget.IsValid() && Parent.IsValid()) { UE_LOG(LogTemp, Log, TEXT("IsGrandChildOf.NoResult: %s not in path of %s."), *Widget->ToString(), *Parent->ToString()); }
	return false;
}

void FTranslationEditorExtensionModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(MenuUIExtensionPoint);
	{
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection(PluginMenuSection);
		{
			FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTranslationEditorExtensionCommands::Get().PluginAction));
			Entry.SetCommandList(PluginCommands);
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTranslationEditorExtensionModule, TranslationEditorExtension)