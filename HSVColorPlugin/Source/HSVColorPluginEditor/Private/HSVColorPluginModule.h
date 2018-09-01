#pragma once

#include "HSVColorPluginPrivatePCH.h"
#include "SDockTab.h"
#include "SColorAnalysisPanel.h"

class FHSVColorPluginModule : public IModuleInterface
{
public:
	// IMoudleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// End of IModuleInterface interface

	static void TriggerTool(UClass* ToolClass) {};
	static void CreateToolListMenu(class FMenuBuilder& MenuBuilder) {};
	
	//static void OnToolWindowClosed(const TSharedRef<SWindow>& Window, UBaseEditorTool* Instance);

	static void HandleTestCommandExcute();

	static bool HandleTestCommandCanExcute();

	//open the main config window
	void OpenHSVColorWindow();

	// analysis color infos
	FReply OnAnalysisButtonClicked();

	//clear content
	FReply OnClearButtonClicked();

	//recover old color
	FReply OnResetButtonClicked();

	// Analysis Some Effect File like Particles / Blueprint with Particles
	FReply OnFromFileButtonClicked();

	//click save and exit button
	FReply OnSaveAndExit();

	//setup the tab-panel
	TSharedRef<SDockTab> SpawnTabPanel(const FSpawnTabArgs& Args);

	// Build Content
	void AnalysisParticleSystem(UParticleSystem* ParticleSystem, UObject* Base);

	void ResetSlider();

	void ResetParticleSystem();

	void ClearPanelContent();

	void ApplyColorChange();

private:

	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<SColorAnalysisPanel> ColorPanel;
	TSharedPtr<SSlider> HueSlider;
	TSharedPtr<SSlider> SaturationSlider;
	TSharedPtr<SSlider> ValueSlider;
	TSharedPtr<SNumericEntryBox<float>> HueValueText;
	TSharedPtr<SNumericEntryBox<float>> SaturationValueText;
	TSharedPtr<SNumericEntryBox<float>> ValueValueText;
	TArray<UParticleSystem*> ObjectsToSave;

	FLinearColor HSVOffset;
	TSharedPtr<SDockTab> TabWindow{};
	bool TabWindowOpened;
};