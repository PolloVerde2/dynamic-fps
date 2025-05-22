#include <Geode/Geode.hpp>

using namespace geode::prelude;

void changeFramerate(int target) {
	// stolen code from prism menu
	auto GM = GameManager::sharedState();
	auto app = CCApplication::sharedApplication();
	app->setAnimationInterval(1.0 / static_cast<double>(target));
	// ^^^ is this even necessary
	GM->m_customFPSTarget = target;
	GM->setGameVariable("0116", true);
	GM->updateCustomFPS();
}

// Load settings
int menuFPS = Mod::get()->getSettingValue<int64_t>("menu-fps");
int playFPS = Mod::get()->getSettingValue<int64_t>("play-fps");

#include <Geode/loader/SettingV3.hpp>
$execute {
    listenForSettingChanges("menu-fps", [](int value) {
		// Changes framerate as new setting is applied
		changeFramerate(value);
		menuFPS = value;
    });
	listenForSettingChanges("play-fps", [](int value) {
		playFPS = value;
    });
}

#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	bool init() {	
		if (!MenuLayer::init()) {
			return false;
		}

		/* 
			Ensures game starts at target framerate
			(most likely where menu shaders will apply)
		*/
		changeFramerate(menuFPS);

		return true;
	}
};

#include <Geode/modify/PlayLayer.hpp>
class $modify(PlayLayer) {
	void onQuit() {
		changeFramerate(menuFPS);

		PlayLayer::onQuit();
	}

	void onEnterTransitionDidFinish() { // nice name bro
		changeFramerate(playFPS);

		PlayLayer::onEnterTransitionDidFinish();
	}
};