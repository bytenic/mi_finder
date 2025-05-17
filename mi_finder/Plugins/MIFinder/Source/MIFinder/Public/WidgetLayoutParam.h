#pragma once

class WidgetLayoutParam
{
public:
	static constexpr float WidgetPadding = 2.5f;
	static constexpr float ParameterTextFontSize = 11.0f;
	static constexpr float MaterialParameterTextFontSize = 10.0f;
	static constexpr float EditableTextBoxHeight = ParameterTextFontSize + 12.0f;

	static constexpr float StaticSwitchParameterRowRatioIsActive = 0.25f;
	static constexpr float StaticSwitchParameterRowRatioParamName = 1.0f;
	static constexpr float StaticSwitchParameterRowRatioCondition = .25f; 
	
	static constexpr float TextureParameterRowRatioIsActive = 0.2f;
	static constexpr float TextureParameterRowRatioParamName = 1.0f;
	static constexpr float TextureParameterRowRatioTextureName = 1.0f;
	static constexpr float TextureParameterRowRatioCondition = 0.2f;

	static constexpr float ScalarParameterRowRatioIsActive = 0.25f;
	static constexpr float ScalarParameterRowRatioParamName = 1.0f;
	static constexpr float ScalarParameterRowRatioFloatValue = 0.25f; 
	static constexpr float ScalarParameterRowRatioConditionType = 0.25f; 
};
