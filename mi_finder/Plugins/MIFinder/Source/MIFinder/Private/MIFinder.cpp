// Copyright Epic Games, Inc. All Rights Reserved.

#include "MIFinder.h"
#include "MIFinderStyle.h"
#include "MIFinderCommands.h"
#include "MIFinderWidget.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Materials/Material.h"
#include "Materials/MaterialFunctionMaterialLayer.h"
#include "Materials/MaterialFunctionMaterialLayerBlend.h"
#include "WidgetLayoutParam.h"

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

	auto MainWidget =  SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					BuildRootMaterialBox()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					BuildSelectLayerFunctionBox()
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0)
				[
					BuildParametersBox()
				]
			]
		];
	BuildStaticSwitchBox();
	BuildTextureBox();
	BuildScalarBox();
	return MainWidget;
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildRootMaterialBox()
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.FillWidth(1.0f)
	.Padding(WidgetLayoutParam::WidgetPadding)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
			.Text(NSLOCTEXT("AssetSearchTab","SearchRootText", "SearchRootPath"))
		]
		+SVerticalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SBox)
			.HeightOverride(WidgetLayoutParam::EditableTextBoxHeight)
			[
				SNew(SEditableTextBox)
				.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
				.Text(NSLOCTEXT("AssetSearchTab","SearchRootTextBox", "/Game"))
			]
		]
	]
	+ SHorizontalBox::Slot()
	.FillWidth(1.0f)
	.Padding(WidgetLayoutParam::WidgetPadding)
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
			.Text(NSLOCTEXT("AssetSearchTab","RootMaterialText", "Root Material"))
		]
		+SVerticalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SObjectPropertyEntryBox)
				.AllowedClass(UMaterialInterface::StaticClass())
				.ObjectPath(TAttribute<FString>::CreateLambda([this]()
				{
					if(!SearchRootMaterial.IsValid())
					{
						return FString{};
					}
					return SearchRootMaterial.Get()->GetPathName();
				}))
				.OnObjectChanged(FOnSetObject::CreateRaw(this, &FMIFinderModule::OnRootMaterialChanged))
		]
	];
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildSelectLayerFunctionBox()
{
	return SNew(SHorizontalBox)
	       +SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(WidgetLayoutParam::WidgetPadding)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
					.Text(NSLOCTEXT("FunctionsSelectTab","MateriallayerText", "Material Layer"))
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SObjectPropertyEntryBox)
					.AllowedClass(UMaterialFunctionMaterialLayer::StaticClass())
					.ObjectPath(TAttribute<FString>::CreateLambda([this]()
					{
						if(!MaterialLayerAsset.IsValid())
						{
							return FString{};
						}
						return MaterialLayerAsset.Get()->GetPathName();
					}))
					.OnObjectChanged(FOnSetObject::CreateRaw(this, &FMIFinderModule::OnRootMaterialLayerChanged))
				]
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(WidgetLayoutParam::WidgetPadding)
		    [
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				. AutoHeight()
				
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
					.Text(NSLOCTEXT("FunctionsSelectTab","MaterialBlendText", "Material Blend"))
				]
				+SVerticalBox::Slot()
				. AutoHeight()
				.Padding(WidgetLayoutParam::WidgetPadding)
				[
					SNew(SObjectPropertyEntryBox)
					.AllowedClass(UMaterialFunctionMaterialLayerBlend::StaticClass())
					.ObjectPath(TAttribute<FString>::CreateLambda([this]()
									{
										if(!MaterialBlendAsset.IsValid())
										{
											return FString{};
										}
										return MaterialBlendAsset.Get()->GetPathName();
									}))
									.OnObjectChanged(FOnSetObject::CreateRaw(this, &FMIFinderModule::OnRootMaterialBlendChanged))
			]
		];
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildParametersBox()
{
	auto ret = SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.Padding(WidgetLayoutParam::WidgetPadding)
			.FillWidth(1.0f)
			.VAlign(VAlign_Top)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
					.Text(NSLOCTEXT("ParametersTab","StaticSwitchText", "Static Switch"))
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					BuildStaticSwitchParameterHeader()
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.FillHeight(1.0f)
				[
					SAssignNew(StaticSwitchScrollBox, SScrollBox)
					.Orientation(Orient_Vertical)
					 + SScrollBox::Slot()
					 [
					 	SAssignNew(StaticSwitchInnerBox, SVerticalBox)
					 ]
				]
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(WidgetLayoutParam::WidgetPadding)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					SNew(STextBlock).Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
					.Text(NSLOCTEXT("ParametersTab","TexturePathText", "Texture"))
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					BuildTextureParameterHeader()
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.FillHeight(1.0f)
				[
					SAssignNew(TextureParameterScrollBox, SScrollBox)
					.Orientation(Orient_Vertical)
					 + SScrollBox::Slot()
					 [
						 SAssignNew(TextureParameterInnerBox, SVerticalBox)
					 ]
				]
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.Padding(WidgetLayoutParam::WidgetPadding)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					SNew(STextBlock)
					.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
					.Text(NSLOCTEXT("ParametersTab","ScalarText", "Scalar"))
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.AutoHeight()
				[
					BuildScalarParameterHeader()
				]
				+ SVerticalBox::Slot()
				.Padding(WidgetLayoutParam::WidgetPadding)
				.FillHeight(1.0f)
				[
					SAssignNew(ScalarParameterScrollBox, SScrollBox)
					.Orientation(Orient_Vertical)
					 + SScrollBox::Slot()
					 [
						 SAssignNew(ScalarParameterInnerBox, SVerticalBox)
					 ]
				]
			];
	return ret;
}

void FMIFinderModule::BuildStaticSwitchBox()
{
	if(!StaticSwitchInnerBox.IsValid())
		return;

	StaticSwitchInnerBox->ClearChildren();
	for(int i = 0 ; i< 300; i++)
	{
		StaticSwitchInnerBox->AddSlot()
		.AutoHeight()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SStaticSwitchParameterWidget)
			.InItem(MakeShareable<StaticSwitchParameterDataObject>(new StaticSwitchParameterDataObject()))
		];
	}
}

void FMIFinderModule::BuildTextureBox()
{
	if(!TextureParameterInnerBox.IsValid())
		return;

	TextureParameterInnerBox->ClearChildren();
	for(int i = 0 ; i< 300; i++)
	{
		TextureParameterInnerBox->AddSlot()
		.AutoHeight()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(STextureParameterWidget)
			.InItem(MakeShareable<TextureParameterDataObject>(new TextureParameterDataObject()))
		];
	}
}

void FMIFinderModule::BuildScalarBox()
{
	if(!ScalarParameterInnerBox.IsValid())
		return;

	ScalarParameterInnerBox->ClearChildren();
	for(int i = 0 ; i< 300; i++)
	{
		ScalarParameterInnerBox->AddSlot()
		.AutoHeight()
		.Padding(WidgetLayoutParam::WidgetPadding)
		[
			SNew(SScalarParameterWidget)
			.InItem(MakeShareable<FScalarParameterDataObject>(new FScalarParameterDataObject()))
		];
	}
}

void FMIFinderModule::OnRootMaterialChanged(const FAssetData& InAssetData)
{
	if(auto MaterialInterface = Cast<UMaterialInstance>(InAssetData.GetAsset()))
	{
		SearchRootMaterial = MaterialInterface;
	}
}

void FMIFinderModule::OnRootMaterialLayerChanged(const FAssetData& InAssetData)
{
	if(auto MaterialLayer = Cast<UMaterialFunctionMaterialLayer>(InAssetData.GetAsset()))
	{
		MaterialLayerAsset = MaterialLayer;
	}
}

void FMIFinderModule::OnRootMaterialBlendChanged(const FAssetData& InAssetData)
{
	if(auto MaterialBlend = Cast<UMaterialFunctionMaterialLayerBlend>(InAssetData.GetAsset()))
	{
		MaterialBlendAsset = MaterialBlend;
	}
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildStaticSwitchParameterHeader()
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioIsActive)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
		.Text(NSLOCTEXT("StaticSwitchParameterHeader","IsEnable", "Active"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioParamName)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
		.Text(NSLOCTEXT("StaticSwitchParameterHeader","Name", "Name"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::StaticSwitchParameterRowRatioCondition)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
		.Text(NSLOCTEXT("StaticSwitchParameterHeader","Condition", "Condition"))
	];
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildTextureParameterHeader()
{
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.HAlign(HAlign_Left)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioIsActive)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
			.Text(NSLOCTEXT("TextureParameterHeader","IsEnable", "Active"))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.HAlign(HAlign_Left)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioParamName)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
			.Text(NSLOCTEXT("TextureParameterHeader","Name", "Name"))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.HAlign(HAlign_Left)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioTextureName)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
			.Text(NSLOCTEXT("TextureParameterHeader","Texture", "Name"))
		]
		+ SHorizontalBox::Slot()
		.Padding(WidgetLayoutParam::WidgetPadding)
		.HAlign(HAlign_Left)
		.FillWidth(WidgetLayoutParam::TextureParameterRowRatioCondition)
		[
			SNew(STextBlock)
			.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
			.Text(NSLOCTEXT("TextureParameterHeader","Condition", "Condition"))
		];
}

TSharedRef<SHorizontalBox> FMIFinderModule::BuildScalarParameterHeader()
{
	return SNew(SHorizontalBox)
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioIsActive)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
		.Text(NSLOCTEXT("ScalarParameterHeader","IsEnable", "Active"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioParamName)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
		.Text(NSLOCTEXT("ScalarParameterHeader","Name", "Name"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioConditionType)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
		.Text(NSLOCTEXT("ScalarParameterHeader","ScalarValue", "Value"))
	]
	+ SHorizontalBox::Slot()
	.Padding(WidgetLayoutParam::WidgetPadding)
	.HAlign(HAlign_Left)
	.FillWidth(WidgetLayoutParam::ScalarParameterRowRatioConditionType)
	[
		SNew(STextBlock)
		.Font(FSlateFontInfo(FCoreStyle::GetDefaultFont(),WidgetLayoutParam::TextFontSize))
		.Text(NSLOCTEXT("ScalarParameterHeader","Condition", "Type"))
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