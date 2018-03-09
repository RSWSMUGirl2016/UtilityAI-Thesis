#include "Game/Player.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Game/Map.hpp"

//*******************************************************************
Player::Player()
{
}

//*******************************************************************
Player::~Player()
{
}

//*******************************************************************
void Player::ChangeType(const std::string& playerName)
{
    m_actorDefinition = ActorDefinition::GetActorDefinition(playerName);
    
    m_name = m_actorDefinition->m_name;
    m_currentHealth = m_actorDefinition->m_currentHealth;
    m_totalHealth = m_actorDefinition->m_totalHealth;
    m_damage = m_actorDefinition->m_damage;
    m_defense = m_actorDefinition->m_defense;
    m_gylphColor = m_actorDefinition->m_glyphColors[0];
    m_fillColor = m_actorDefinition->m_fillColors[0];
    m_glyph = m_actorDefinition->m_glyphs[0];
    m_faction = m_actorDefinition->m_faction;
    m_totalWeight = m_actorDefinition->m_totalWeight;
    m_currentWeight = 0;
}

//*******************************************************************
void Player::Update()
{

}

//*******************************************************************
void Player::Render() const
{

}

//*******************************************************************
void Player::FogOfWar()
{
    SetCurrentlyVisibleTilesToPreviouslySeen();
    ClearActorsCurrentlyVisibleToMe();
    m_tilesCurrentlyVisibleToMe.clear();

    Tile* currentTile = m_currentMap->GetTileAtTileCoords(m_tileCoords);
    currentTile->m_currentStateOfPlayerSight = CURRENTLY_SEEN;
    m_tilesCurrentlyVisibleToMe.push_back(m_currentMap->GetTileIndexForTile(*currentTile));

    std::vector<Tile>& allTilesInCurrentMap = m_currentMap->m_tiles;
    Vector2 startPosition = m_currentMap->GetWorldCenterForTileCoordinates(m_tileCoords);
    
    for (int allTilesInCurrentMapIndex = 0; allTilesInCurrentMapIndex < (int)allTilesInCurrentMap.size(); allTilesInCurrentMapIndex++)
    {
        Tile& tile = allTilesInCurrentMap[allTilesInCurrentMapIndex];
        Vector2 endPosition = m_currentMap->GetWorldCenterForTileCoordinates(tile.m_tileCoords);
        RayCastResult2D result = m_currentMap->AmanatidesWooRaycast(startPosition, endPosition);

        float distanceFromStartToEnd = (float)m_currentMap->CalculateManhattanDistance(m_tileCoords, tile.m_tileCoords);
        if ((!result.m_didImpact) && (distanceFromStartToEnd <= m_lengthOfSight))
        {
            tile.m_currentStateOfPlayerSight = CURRENTLY_SEEN;
            m_tilesCurrentlyVisibleToMe.push_back(allTilesInCurrentMapIndex);
            if (tile.m_occupant != nullptr)
            {
                m_actorsCurrentlyVisibleToMe.push_back(*tile.m_occupant);
            }
        }
    }
}

//*******************************************************************
void Player::SetCurrentlyVisibleTilesToPreviouslySeen()
{
    for (int tilesCurrentlyVisibleToMeIndex = 0; tilesCurrentlyVisibleToMeIndex < (int)m_tilesCurrentlyVisibleToMe.size(); tilesCurrentlyVisibleToMeIndex++)
    {
        Tile* tile = m_currentMap->GetTileForTileIndex(m_tilesCurrentlyVisibleToMe[tilesCurrentlyVisibleToMeIndex]);
        if (tile->m_currentStateOfPlayerSight == CURRENTLY_SEEN)
        {
            tile->m_currentStateOfPlayerSight = PREVIOUSLY_SEEN;
        }
    }
}

//*******************************************************************
void Player::Act()
{
    FogOfWar();
    CheckMyNeighborsForAnOpponent();
    m_currentMap->m_currentNumTurnsTowardsSolvingPuzzle++;
}
