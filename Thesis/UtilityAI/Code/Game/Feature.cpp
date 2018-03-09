#include "Game/Feature.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"

//*******************************************************************
Feature::Feature()
{
}

//*******************************************************************
Feature::~Feature()
{
}

//*******************************************************************
void Feature::ChangeType(const std::string& featureName)
{
    m_featureDefinition = FeatureDefinition::GetFeatureDefinition(featureName);

    m_name = m_featureDefinition->m_type;
    m_type = m_featureDefinition->m_type;
    m_isSolid = m_featureDefinition->m_isSolid;
    m_gylphColor = m_featureDefinition->m_gylphColor;
    m_fillColor = m_featureDefinition->m_fillColor;
    m_glyph = m_featureDefinition->m_glyph;
    m_faction = "NA";

    m_playerTouched = false;
}

//*******************************************************************
void Feature::Update()
{
    if (m_playerTouched)
    {
        Activate();
    }
}

//*******************************************************************
void Feature::Render() const
{
}

//*******************************************************************
void Feature::Activate()
{
    /*if (m_type == "chest")
    {
        m_currentMap->RemoveFeatureAtTileCoords(m_currentTile->m_tileCoords);
    }

    if ((m_featureDefinition->m_type == "front door"))
    {
        g_theGame->m_theWorld->m_currentMap = m_currentMap->m_nextMap;
        g_theGame->m_theWorld->m_currentMap->m_player->m_currentMap = g_theGame->m_theWorld->m_currentMap;
        g_theGame->m_theWorld->m_currentMap->m_currentNumTurnsTowardsSolvingPuzzle = 0;
    }*/
}