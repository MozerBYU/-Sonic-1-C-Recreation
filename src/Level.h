#pragma once

#include <list>
#include <vector>

#include "entities/Entity.h"
#include "entity-creator.hpp"
#include "entity-placement.hpp"
#include "entity-pool.hpp"
#include "game-loop-ticker.h"
#include "terrain-loader.hpp"

#include "entities/_index.hpp"

#include "Camera.h"
#include "Bg.h"
#include "LevelInformer.h"

#include "player-sensor.hpp"

#define TILE_HEIGHTS_BUFF_SIZE 4096
#define TILE_ANGLES_BUFF_SIZE  256

enum class GameType {
    SONIC_1,
    SONIC_2,
    SONIC_3K,
    SONIC_CD,
    // New game being added
    SONIC_4X,
};

class Level {
public:
    Level(
        terrain::Terrain& terrain,
        std::vector<EntityPlacement>& entities,
        GameType gameType,
        Screen& scr, 
        IInputMgr& input, 
        Audio& audio,
        std::string& zoneName,
        //std::string& zoneNameShort,//
        int act,
        v2f playerStartPosition,
        terrain::Store<terrain::Tile>& storeTile
    ) 
        : m_terrain(terrain)
        , cam(scr)
        , m_entityPool(cam)
        , m_screen(scr)
        , m_input(input)
        , m_audio(audio) 
        , m_gameType(gameType)
        , m_zoneName(zoneName)
        /*
        , m_zoneNameShort(zoneNameShort)
        */
        , m_act(act)
        , m_playerStartPosition(playerStartPosition)
        , m_terrainDrawer(cam, m_terrain.getChunkStore(), m_terrain.getLayout(), 255, storeTile)
        , bg(m_terrainDrawer)
    {
        EntityCreatorSonic1 ec(m_entityPool, m_terrain);
        /*
        EntityCreatorSonic2 ec(m_entityPool, m_terrain);
        EntityCreatorSonic3K ec(m_entityPool, m_terrain);
        EntityCreatorSonicCD ec(m_entityPool, m_terrain);
        EntityCreatorSonic4 ec(m_entityPool, m_terrain);
        */
        
        for (auto& plc : entities) {
            m_entityPool.create(ec.create(plc));
        }
    }
    void create();
    void free();

    bool isPlayerDead() { return playerDead; }
    bool isEnded() { return end; }

    void update();

    void draw();

    terrain::TerrainDrawer& getTerrainDrawer() { return m_terrainDrawer; } 

private:
    terrain::Terrain &m_terrain;

    EntityPool  m_entityPool;
    GameType    m_gameType;
    std::string m_zoneName;
    //std::string m_zoneNameShort;//
    int         m_act;

    Screen&     m_screen;
    IInputMgr&  m_input;
    Audio&      m_audio;

    v2f m_playerStartPosition;

    terrain::TerrainDrawer m_terrainDrawer;
    Bg bg;

    Camera cam;


    std::list<Entity*>::iterator it;
    LevelInformer* lvInformer = nullptr;

    int rings = 0;
    int score = 0;
    int time = 0;
    int tick = 0;
    bool isTimeStopped = false;
    bool isFadeOut = false;
    bool isFadeDeath = false;

    bool playerDied = false;

    float ringFrame;

    bool end;

    uint8_t fade;

private:
    void createSonic1ZoneSpecific();
    void createSonic2ZoneSpecific();
    void createSonic3KZoneSpecific();
    void createSonicCDZoneSpecific();
    void createSonic4XZoneSpecific();
    
    void createSonic1LayeringObjects();
    void createSonic2LayeringObjects();
    void createSonic3KLayeringObjects();
    void createSonicCDLayeringObjects();
    void createSonic4XLayeringObjects();

    void drawHud();

    void updateLevelSpecific();
};