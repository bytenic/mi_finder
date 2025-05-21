// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "MIFinderWidget.h"

class FToolBarBuilder;
class FMenuBuilder;
class SHorizontalBox;
class SDockTab;
class UMaterial;
class UMaterialFunctionMaterialLayer;
class UMaterialFunctionMaterialLayerBlend;
class SScrollBox;

class FMIFinderModule : public IModuleInterface
{
public :
	struct MaterialParameterWrapper
	{
		TArray<TSharedPtr<StaticSwitchParameterDataObject>> StaticSwitchParameters{};
		TArray<TSharedPtr<TextureParameterDataObject>> TextureParameters{};
		TArray<TSharedPtr<FScalarParameterDataObject>> ScalarParameters{};

		MaterialParameterWrapper();
		~MaterialParameterWrapper() = default;
		void ClearAll();
		void BuildParameterFormMaterial(const UMaterial* Material);
	};
	
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

	TSharedRef<SHorizontalBox> BuildStaticSwitchParameterHeader();
	TSharedRef<SHorizontalBox> BuildTextureParameterHeader();
	TSharedRef<SHorizontalBox> BuildScalarParameterHeader();

	void ClearAllParameterWidget();
	void BuildParameterWidget();
	
private:
	TSharedPtr<FUICommandList> PluginCommands;
	
	TWeakObjectPtr<UMaterial> SearchRootMaterial{};
	TWeakObjectPtr<UMaterialFunctionMaterialLayer> MaterialLayerAsset{};
	TWeakObjectPtr<UMaterialFunctionMaterialLayerBlend> MaterialBlendAsset{};	
	
	TSharedPtr<SScrollBox> StaticSwitchScrollBox{};
	TSharedPtr<SVerticalBox> StaticSwitchInnerBox{};

	TSharedPtr<SScrollBox> TextureParameterScrollBox{};
	TSharedPtr<SVerticalBox> TextureParameterInnerBox{};

	TSharedPtr<SScrollBox> ScalarParameterScrollBox{};
	TSharedPtr<SVerticalBox> ScalarParameterInnerBox{};

	MaterialParameterWrapper MaterialParameters{};
};
