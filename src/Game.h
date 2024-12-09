#pragma once

#include "core/_index.hpp"

#include "Level.h"
#include "TitleScreen.h"
#include "level-loader.hpp"

class IGame {
    public:
        virtual void init() = 0;
        virtual void update() = 0;
        virtual bool isRunning() = 0;
};

class Game: public IGame {
    public: 
        void init();
        void update();
        bool isRunning();
    private:
        enum MenuTab {T_MAIN, T_SETTINGS, T_PLAY};
        enum ScreenScale {SS_NORMAL, SS_X2, SS_X3, SS_FULL_SCREEN};

        Audio audio;
        Screen scr;
        Keyboard keyboard;

        LevelLoader m_levelLoader = LevelLoader(scr, keyboard, audio);
        Level* m_level = nullptr;

        TitleScreen* ts = nullptr;

        int curLevel = 0;

        int menuPos = 0;
        int menuItemsCount = 0;
        MenuTab menuTab = T_MAIN;

        int wndSize = SS_NORMAL;

        const char* strLevels[18] = {
            // Sonic 1 Levels //
            "green_hill_1", "green_hill_2", "green_hill_3", 
            "marble_1", "marble_2", "marble_3", 
            "springyard_1", "springyard_2", "springyard_3", 
            "labyrinth_1", "labyrinth_2", "labyrinth_3",
            "starlight_1", "starlight_2", "starlight_3",
            "scrap_brain_1", "scrap_brain_2", "final",
            // Sonic 2 Levels //
            "emerald_hill_1", "emerald_hill_2", 
            "chemical_plant_1", "chemical_plant_2",
            "aquatic_ruin_1", "aquatic_ruin_2",
            "casino_night_1", "casino_night_2",
            "hill_top_1", "hill_top_2",
            "mystic_cave_1", "mystic_cave_2",
            "oil_ocean_1", "oil_ocean_2",
            "metropolis_1", "metropolis_2", "metropolis_3",
            "sky_chase", "wing_fortress", "death_egg", "hidden_palace",
            // Sonic 3K Levels //
            "icecap_1"
            // Sonic CD Levels //
            // Sonic 4X Levels //
        };
        const char* strZones[6] = { 
            "GREEN_HILL", "MARBLE", "SYZ", "LZ", "SLZ", "SBZ" 
        };

        void loadLevel(int index);

        void menuKeyHandle(sf::Event::KeyEvent key);
        void menuSettingUpdate();
        void menuPlayUpdate();

        void setWndSize(ScreenScale scale);
};