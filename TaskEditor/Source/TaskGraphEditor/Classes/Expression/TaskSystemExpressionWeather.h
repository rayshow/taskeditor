#pragma once

#include"ObjectMacros.h"
#include"CoreMinimal.h"
#include"UObject/ObjectMacros.h"
#include"TaskSystemExpression.h"
#include"TaskSystemExpressionInput.h"
#include"TaskSystemExpressionWeather.generated.h"

UENUM()
enum TaskSystemWeatherType {
	TWT_Rain = 0 UMETA(DisplayName = "小雨"),
	TWT_Cloudy UMETA(DisplayName = "阴天"),
	TWT_Sunny UMETA(DisplayName = "晴天"),
	TWT_MAX
};

UCLASS(MinimalAPI, DisplayName = "事件 - 天气")
class UTaskSystemExpressionWeather : public UTaskSystemExpression
{
public:
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere)
	TEnumAsByte< TaskSystemWeatherType> Type;

	UPROPERTY(EditAnywhere)
	float DuringTime;

	UPROPERTY(EditAnywhere)
	FString EventName;

	static const FName SubCatgory;

#if WITH_EDITOR
	virtual void GetCaption(TArray<FString>& OutCaptions) const override {
		OutCaptions.Add(FString(TEXT(" 事件 - 天气")));
		OutCaptions.Add(EventName);
	}
#endif

};