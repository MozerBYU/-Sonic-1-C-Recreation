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

        const char* strLevels[94] = {
            // Sonic 1 Levels - ct 18 //
            "green_hill_1", "green_hill_2", "green_hill_3", 
            "marble_1", "marble_2", "marble_3", 
            "springyard_1", "springyard_2", "springyard_3", 
            "labyrinth_1", "labyrinth_2", "labyrinth_3",
            "starlight_1", "starlight_2", "starlight_3",
            "scrap_brain_1", "scrap_brain_2", "final",
            // Sonic 2 Levels - ct 21 //
            "emerald_hill_1", "emerald_hill_2", 
            "chemical_plant_1", "chemical_plant_2",
            "aquatic_ruin_1", "aquatic_ruin_2",
            "casino_night_1", "casino_night_2",
            "hill_top_1", "hill_top_2",
            "mystic_cave_1", "mystic_cave_2",
            "oil_ocean_1", "oil_ocean_2",
            "metropolis_1", "metropolis_2", "metropolis_3",
            "sky_chase", "wing_fortress", "death_egg", "hidden_palace",
            // Sonic 3K Levels - ct 27 //
            "angel_island_1", "angel_island_2",
            "hydrocity_1", "hydrocity_2",
            "marble_garden_1", "marble_garden_2",
            "carnival_night_1", "carnival_night_2",
            "icecap_1", "icecap_2",
            "launch_base_1", "launch_base_2",
            "mushroom_hill_1", "mushroom_hill_2",
            "flying_battery_1", "flying_battery_2",
            "sandopolis_1", "sandopolis_2",
            "lava_reef_1", "lava_reef_2", "lava_reef_3",
            "hidden_palace",
            "sky_sanctuary_1", "sky_sanctuary_2",
            "death_egg_1", "death_egg_2",
            "doomsday"
            // Sonic CD Levels - ct 22 //
            "green_grove_1", "green_grove_2", "green_grove_3",
            "rusty_ruin_1", "rusty_ruin_2", "rusty_ruin_3",
            "spring_stadium_1", "spring_stadium_2", "spring_stadium_3",
            "diamond_dust_1", "diamond_dust_2", "diamond_dust_3",
            "volcano_valley_1", "volcano_valley_2", "volcano_valley_3",
            "gene_gadget_1", "gene_gadget_2", "gene_gadget_3",
            "panic_puppet_1", "panic_puppet_2", "panic_puppet_3",
            "final_fight",
            // Sonic 4X Levels - ct 6 //
            "windy_organ_1", "windy_organ_2",
            "nuclear_wasteland_1", "nuclear_wasteland_2",
            "hardcore_palace", "doomsday_xtreme"
        };
        const char* strZones[44] = { 
            // Sonic 1 Zones - ct 6//
            "GREEN_HILL", "MARBLE", "SPRINGYARD", "LABYRINTH", "STARLIGHT", "SCRAP_BRAIN",
            // Sonic 2 Zones - ct 12//
            "EMERALD_HILL", "CHEMICAL_PLANT", "AQUATIC_RUIN", "CASINO_NIGHT", "HILL_TOP", "MYSTIC_CAVE", 
            "OIL_OCEAN", "METROPOLIS", "SKY_CHASE", "WING_FORTESS", "DEATH_EGG", "HIDDEN_PALACE",
            // Sonic 3K Zones - ct 14//
            "ANGEL_ISLAND", "HYDROCITY", "MARBLE_GARDEN", "CARNIVAL_NIGHT", "ICECAP", "LAUNCH_BASE",
            "MUSHROOM_HILL", "FLYING_BATTERY", "SANDPOLIS", "LAVA_REEF", "HIDDEN_PALACE", "SKY_SANCTUARY",
            "DEATH_EGG", "DOOMSDAY",
            // Sonic CD Zone - ct 8//
            "GREEN_GROVE", "RUSTY_RUIN", "SPRING_STADIUM", "DIAMOND_DUST", "VOLCANO_VALLEY", "GENE_GADGET",
            "PANIC_PUPPET", "FINAL_FIGHT",
            // Sonic 4X Zones - ct 4//
            "WINDY_ORGAN", "NUCLEAR_WASTELAND", "HARDCORE_PALACE", "DOOMSDAY_XTREME", 
        };

        void loadLevel(int index);

        void menuKeyHandle(sf::Event::KeyEvent key);
        void menuSettingUpdate();
        void menuPlayUpdate();

        void setWndSize(ScreenScale scale);
};