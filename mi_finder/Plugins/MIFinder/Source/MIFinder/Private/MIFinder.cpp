// Copyright Epic Games, Inc. All Rights Reserved.

#include "MIFinder.h"
#include "MIFinderStyle.h"
#include "MIFinderCommands.h"
#include "LevelEditor.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "PropertyCustomizationHelpers.h"

static const FName MIFinderTabName("MIFinder");

#define LOCTEXT_NAMESPACE "FMIFinderModule"

void FMIFinderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FMIFinderStyle::Initialize();
	FMIFinderStyle::ReloadTextures();

	FMIFinderCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMIFinderCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FMIFinderModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMIFinderModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MIFinderTabName, FOnSpawnTab::CreateRaw(this, &FMIFinderModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FMIFinderTabTitle", "MIFinder"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FMIFinderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMIFinderStyle::Shutdown();

	FMIFinderCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MIFinderTabName);
}

TSharedRef<SDockTab> FMIFinderModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FMIFinderModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("MIFinder.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				//SearchRoot
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				//.AutoHeight()
				//.FillHeight(2.0)
				[
					SNew(STextBlock)
						//.Font(FAppStyle::Get().GetFontStyle("SmallFontBold"))
						//.ColorAndOpacity( FAppStyle::Get().GetSlateColor(  "Colors.White" ) )
						.Text(NSLOCTEXT("AssetSearchTab","SearchRootText", "SearchRootPath"))
				]
				+SVerticalBox::Slot()
				[
					SNew(SEditableTextBox)
					.Text(NSLOCTEXT("AssetSearchTab","SearchRootTextBox", "SearchRootPathBox"))
				]
				+SVerticalBox::Slot()
				[
					SNew(STextBlock)
					//.Font(FAppStyle::Get().GetFontStyle("SmallFontBold"))
					//.ColorAndOpacity( FAppStyle::Get().GetSlateColor(  "Colors.White" ) )
					.Text(NSLOCTEXT("AssetSearchTab","RootMaterialText", "Root Material"))
				]
				+SVerticalBox::Slot()
				[
					SNew(SObjectPropertyEntryBox)
						.AllowedClass(UMaterial::StaticClass())
						//.OnObjectChanged(this, &FYourClass::OnAssetSelected) // コールバック関数
				]

				//SelectFunctions
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						. AutoHeight()
						[
							SNew(STextBlock)
							//.Font(FAppStyle::Get().GetFontStyle("SmallFontBold"))
							//.ColorAndOpacity( FAppStyle::Get().GetSlateColor(  "Colors.White" ) )
							.Text(NSLOCTEXT("FunctionsSelectTab","MateriallayerText", "Material Layer"))
						]
						+SVerticalBox::Slot()
						. AutoHeight()
						[
							SNew(SObjectPropertyEntryBox)
							.AllowedClass(UMaterial::StaticClass())
							//.OnObjectChanged(this, &FYourClass::OnAssetSelected) // コールバック関数
						]
						
					]
					+SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SVerticalBox)
							+SVerticalBox::Slot()
							. AutoHeight()
							[
								SNew(STextBlock)
								//.Font(FAppStyle::Get().GetFontStyle("SmallFontBold"))
								//.ColorAndOpacity( FAppStyle::Get().GetSlateColor(  "Colors.White" ) )
								.Text(NSLOCTEXT("FunctionsSelectTab","MaterialBlendText", "Material Blend"))
							]
							+SVerticalBox::Slot()
							. AutoHeight()
							[
								SNew(SObjectPropertyEntryBox)
								.AllowedClass(UMaterial::StaticClass())
								//.OnObjectChanged(this, &FYourClass::OnAssetSelected) // コールバック関数
							]
						
						]
				]
				// Parameters
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					//.FillWidth(1.0)
					[
						SNew(STextBlock)
							//.Font(FAppStyle::Get().GetFontStyle("SmallFontBold"))
							//.ColorAndOpacity( FAppStyle::Get().GetSlateColor(  "Colors.White" ) )
							.Text(NSLOCTEXT("ParametersTab","StaticSwitchText", "Static Switch"))
					]
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
							//.Font(FAppStyle::Get().GetFontStyle("SmallFontBold"))
							//.ColorAndOpacity( FAppStyle::Get().GetSlateColor(  "Colors.White" ) )
							.Text(NSLOCTEXT("ParametersTab","TexturePathText", "Texture"))
					]
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
							//.Font(FAppStyle::Get().GetFontStyle("SmallFontBold"))
							//.ColorAndOpacity( FAppStyle::Get().GetSlateColor(  "Colors.White" ) )
							.Text(NSLOCTEXT("ParametersTab","ScalarText", "Scalar"))
					]
				]
			]
		];
}

void FMIFinderModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(MIFinderTabName);
}

void FMIFinderModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMIFinderCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMIFinderCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMIFinderModule, MIFinder)