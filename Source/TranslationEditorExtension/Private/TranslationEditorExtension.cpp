// Copyright 2025 Tobias Pott. All rights reserved.

#include "TranslationEditorExtension.h"
#include "TranslationEditorExtensionStyle.h"
#include "TranslationEditorExtensionCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "IPropertyTableRow.h"
#include "Editor/PropertyEditor/Private/Presentation/PropertyTable/DataSource.h"
#include "Editor/TranslationEditor/Private/TranslationUnit.h"
#include "Kismet/KismetSystemLibrary.h"

static const FName TranslationEditorExtensionTabName("TranslationEditorExtension");


static const FName UntranslatedTabName(TEXT("TranslationEditor_Untranslated"));
// Names and TabIds need to be checked with UnrealEngine's TranslationEditor internal names when porting this plugin to a different version
static const FName ReviewTabName(TEXT("TranslationEditor_Review"));
//static const FName UntranslatedTabName(TEXT("TranslationEditor_Untranslated"));
static const FName CompletedTabName(TEXT("TranslationEditor_Completed"));
//static const FName PreviewTabName(TEXT("TranslationEditor_Preview"));

static const FTabId UntranslatedTabId(UntranslatedTabName);
static const FTabId ReviewTabId(ReviewTabName);
//static const FTabId UntranslatedTabId(UntranslatedTabId);
static const FTabId CompletedTabId(CompletedTabName);
//static const FTabId PreviewTabId(PreviewTabName);

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

	PluginCommands->MapAction(FTranslationEditorExtensionCommands::Get().ToggleAllReviewedAction,
		FExecuteAction::CreateRaw(this, &FTranslationEditorExtensionModule::ToggleAllReviewedButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(FTranslationEditorExtensionCommands::Get().TransferSourceAction,
		FExecuteAction::CreateRaw(this, &FTranslationEditorExtensionModule::TransferSourceButtonClicked),
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

void FTranslationEditorExtensionModule::ToggleAllReviewedButtonClicked()
{
	TSharedPtr<SDockTab> ReviewTab;
	TSharedPtr<SWidget> ReviewContent;
	FAssetEditorToolkit* EditorToolkit;
	FString ExtName = TEXT("Toggle All Reviewed");

	if (GetWidgetsAndEditor(ReviewTabId, ReviewTab, ReviewContent, EditorToolkit))
	{
		if (EditorToolkit != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Executing %s on: %s."), *ExtName, *EditorToolkit->GetToolkitName().ToString());
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
	}
	return;
}

void FTranslationEditorExtensionModule::TransferSourceButtonClicked()
{
	TSharedPtr<SDockTab> CurrentTab;
	TSharedPtr<SWidget> CurrentContent;
	FAssetEditorToolkit* EditorToolkit;
	FString ExtName = TEXT("TransferSource");

	// try get completed tab if it is the active of the current focused editor
	bool bFoundEditor = GetWidgetsAndEditor(CompletedTabId, CurrentTab, CurrentContent, EditorToolkit);
	if (!bFoundEditor)
		// try get untranslated tab if completed wasn't found (assume it wasn't active or available)
		bFoundEditor = GetWidgetsAndEditor(UntranslatedTabId, CurrentTab, CurrentContent, EditorToolkit);
	// process further if anything was found
	if (bFoundEditor)
	{
		// get user confirmation for action (might lose data on reset)
		if (!CheckConfirmDialog(FText::GetEmpty()))
		{
			if (EditorToolkit != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Aborted execution %s on: %s."), *ExtName, *EditorToolkit->GetToolkitName().ToString());
			}
			return;
		}
		if (EditorToolkit != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Executing %s on: %s."), *ExtName, *EditorToolkit->GetToolkitName().ToString());
		}

		FChildren* ReviewContentChildren = CurrentContent->GetChildren();
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
					{
						if (!ItemTranslationUnit->Translation.Equals(ItemTranslationUnit->Source))
							ItemTranslationUnit->Translation = ItemTranslationUnit->Source;
						//// ToDo: @tpott: Unsure whether the reset to TranslationBeforeImport is ueful (is empty string while testing)
						////				might be a thing to lose the current translation (imported) after reset as I cannot save the translation on copy
						////				maybe add a dialog confirm to this extension function (seems lightl risky)
						//else
						//	ItemTranslationUnit->Translation = ItemTranslationUnit->TranslationBeforeImport;
					}
					// Uncomment for additional info log of table rows
					// UE_LOG(LogTemp, Warning, TEXT("Item: %d. TableRow: %s"), ItemIdx, *UKismetSystemLibrary::GetDisplayName(ItemObject));
					// refresh item view to reflect value changes in property table
					ItemView[ItemIdx]->Refresh();
				}

				PropertyTableWidget->RequestListRefresh();
				UE_LOG(LogTemp, Log, TEXT("PropertyTable (TreeView): %s. Rows in PropertyTable: %d"), *PropertyTableWidget->ToString(), ItemView.Num());

			}

		}
		if (EditorToolkit != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Finished work on: %s."), *EditorToolkit->GetToolkitName().ToString());
		}
		return;
	}
}

bool FTranslationEditorExtensionModule::GetWidgetsAndEditor(const FTabId& TabId, TSharedPtr<SDockTab>& OutTab, TSharedPtr<SWidget>& OutContent, FAssetEditorToolkit*& OutEditorToolkit)
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
				OutTab = TabManager->FindExistingLiveTab(TabId);
				OutContent = OutTab->GetContent();

				TSharedPtr<SWidget> ReviewParent = OutTab->GetParentWidget();
				if (IsGrandChildOf(ReviewParent, FocusedWidget))
				{
					OutEditorToolkit = static_cast<FAssetEditorToolkit*>(CurrentEditor);
					if (OutTab->IsActive())
						return true;
				}
			}
		}
	}

	return false;
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

bool FTranslationEditorExtensionModule::CheckConfirmDialog(FText Message)
{
	FText DialogText = Message.IsEmptyOrWhitespace() ? FText::FromString(TEXT("Are you sure?")) : Message;
	EAppReturnType::Type DialogReturn = FMessageDialog::Open(EAppMsgType::YesNo, DialogText);
	return DialogReturn == EAppReturnType::Yes;
}

void FTranslationEditorExtensionModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu(MenuUIExtensionPoint);
	{
		FToolMenuSection& Section = ToolbarMenu->FindOrAddSection(PluginMenuSection);
		{
			FToolMenuEntry& EntryToggleAllReviewed = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTranslationEditorExtensionCommands::Get().ToggleAllReviewedAction));
			FToolMenuEntry& EntryCopySourceToTranslation = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTranslationEditorExtensionCommands::Get().TransferSourceAction));
			EntryToggleAllReviewed.SetCommandList(PluginCommands);
			EntryCopySourceToTranslation.SetCommandList(PluginCommands);
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTranslationEditorExtensionModule, TranslationEditorExtension)