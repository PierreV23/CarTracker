#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "GuiBase.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class CarTracker: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	//void onUnload() override; // Uncomment and implement if you need a unload method
	void ballOnTop();
	//void RenderSettings() override;
	//void SetImGuiContext(uintptr_t ctx) override;

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window
	//void SetImGuiContext(uintptr_t ctx) override; // why does this not work, it should use the one from guibase right?
};
