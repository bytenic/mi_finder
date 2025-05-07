// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class SHorizontalBox;
class SDockTab;
class UMaterialInterface;
class UMaterialFunctionMaterialLayer;
class UMaterialFunctionMaterialLayerBlend;
class SScrollBox;

class FMIFinderModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:
	void RegisterMenus();
	TSharedRef<SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SHorizontalBox> BuildRootMaterialBox();
	TSharedRef<SHorizontalBox> BuildSelectLayerFunctionBox();
	TSharedRef<SHorizontalBox> BuildParametersBox();
	void OnRootMaterialChanged(const FAssetData & InAssetData);
	void OnRootMaterialLayerChanged(const FAssetData & InAssetData);
	void OnRootMaterialBlendChanged(const FAssetData & InAssetData);

	void BuildStaticSwitchBox();
	
private:
	TSharedPtr<FUICommandList> PluginCommands;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> SearchRootMaterial{};
	UPROPERTY()
	TObjectPtr<UMaterialFunctionMaterialLayer> MaterialLayerAsset{};
	UPROPERTY()
	TObjectPtr<UMaterialFunctionMaterialLayerBlend> MaterialBlendAsset{};	

	
	TSharedPtr<SScrollBox> StaticSwitchScrollBox{};
	TSharedPtr<SVerticalBox> StaticSwitchInnerBox{};

	
private:
	static constexpr float LayoutPadding = 2.5f;
	static constexpr float FontSize = 12.0f;
	static constexpr float EditableTextBoxHeight = FontSize + 12.0f;
};
