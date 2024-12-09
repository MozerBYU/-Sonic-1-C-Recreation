#pragma once

#include <filesystem>

#include "Level.h"

#include "entity-placement.hpp"
#include "entity-pool.hpp"
#include "terrain-loader.hpp"

#include "zone-enums-sonic1.hpp"
#include "zone-enums-sonic2.hpp"
#include "zone-enums-sonic3k.hpp"
#include "zone-enums-soniccd.hpp"
#include "zone-enums-sonic4x.hpp"

class LevelLoader {
public:
    LevelLoader(
        Screen& screen,
        IInputMgr& input,
        Audio& audio
    ) 
        : m_screen(screen) 
        , m_audio(audio)
        , m_input(input)
    {}

    Level* loadFromSonic1(ZoneSonic1 zone, int act);
    Level* loadFromSonic2(ZoneSonic2 zone, int act);
    Level* loadFromSonic3K(ZoneSonic3K zone, int act);
    Level* loadFromSonicCD(ZoneSonicCD zone, int act);
    Level* loadFromSonic4X(ZoneSonic4X zone, int act);

private:
    Screen&     m_screen;
    IInputMgr&  m_input;
    Audio&      m_audio;

    terrain::Terrain*                               m_terrain       = nullptr;
    std::unique_ptr<terrain::Store<terrain::Tile>>  m_storeTiles;
    std::unique_ptr<terrain::Store<terrain::Block>> m_storeBlocks;
    std::unique_ptr<terrain::Store<terrain::Chunk>> m_storeChunks;
    std::unique_ptr<terrain::Layout>                m_layout;
    
    v2f m_playerStartPosition;

    std::vector<EntityPlacement> m_entityPlacementList;
private:
    void reset();

    void sonic1LoadTerrain(std::string& sZone, std::string& sZoneAct);
    void sonic1LoadStartPosition(std::string& sZoneAct);
    void sonic1LoadObjects(std::string& sZoneAct);

};