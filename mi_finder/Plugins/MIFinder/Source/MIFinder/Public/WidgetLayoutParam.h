#pragma once

class WidgetLayoutParam
{
public:
	static constexpr float WidgetPadding = 2.5f;
	static constexpr float TextFontSize = 12.0f;
	static constexpr float EditableTextBoxHeight = TextFontSize + 12.0f;

	static constexpr float StaticSwitchParameterRowRatioIsActive = 0.25f;
	static constexpr float StaticSwitchParameterRowRatioParamName = 2.0f;
	static constexpr float StaticSwitchParameterRowRatioCondition = .25f; 


	static constexpr float TextureParameterRowRatioIsActive = 0.1f;
	static constexpr float TextureParameterRowRatioParamName = 1.0f;
	static constexpr float TextureParameterRowRatioTextureName = 1.0f;
	static constexpr float TextureParameterRowRatioCondition = 0.1f;

	static constexpr float ScalarParameterRowRatioIsActive = 0.1f;
	static constexpr float ScalarParameterRowRatioParamName = 1.0f;
	static constexpr float ScalarParameterRowRatioFloatValue = 0.1f; 
	static constexpr float ScalarParameterRowRatioConditionType = 0.1f; 
};
