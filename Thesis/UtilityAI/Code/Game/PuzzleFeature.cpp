#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/PuzzleFeature.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include <stdlib.h>

//*******************************************************************
PuzzleFeature::PuzzleFeature()
{
}

//*******************************************************************
PuzzleFeature::~PuzzleFeature()
{
}

//*******************************************************************
void PuzzleFeature::ChangeType(const std::string& puzzleFeatureName)
{
    m_featureDefinition = FeatureDefinition::GetFeatureDefinition(puzzleFeatureName);

    m_name = m_featureDefinition->m_type;
    m_type = m_featureDefinition->m_type;
    m_isSolid = m_featureDefinition->m_isSolid;
    m_gylphColor = m_featureDefinition->m_gylphColor;
    m_fillColor = m_featureDefinition->m_fillColor;
    m_glyph = m_featureDefinition->m_glyph;
    m_faction = "NA";
    m_subType = "puzzle";

    m_playerTouched = false;
}

//*******************************************************************
void PuzzleFeature::Update()
{
    if (m_playerTouched)
    {
        Activate();
        m_playerTouched = false;
    }
}

//*******************************************************************
void PuzzleFeature::Activate()
{
    if (std::find(m_currentMap->m_playerSolvedOrderForPuzzle.begin(), m_currentMap->m_playerSolvedOrderForPuzzle.end(), m_indexInSolvedOrder) != m_currentMap->m_playerSolvedOrderForPuzzle.end())
        return;

    m_currentMap->m_playerSolvedOrderForPuzzle.push_back(m_indexInSolvedOrder);

    int sizeOfPlayerSolution = (int)m_currentMap->m_playerSolvedOrderForPuzzle.size();
    if (m_currentMap->m_solvedOrderForPuzzle[sizeOfPlayerSolution - 1] == m_indexInSolvedOrder)
    {
        m_gylphColor = Rgba(0, 0, 255, 255);
    }
    else
    {
        for (int puzzleIndex = 0; puzzleIndex < (int)m_currentMap->m_puzzle.size(); puzzleIndex++)
        {
            m_currentMap->m_puzzle[puzzleIndex]->m_gylphColor = Rgba(255, 255, 0, 255);
        }
        m_currentMap->m_playerSolvedOrderForPuzzle.clear();
    }    

    if (sizeOfPlayerSolution == (int)m_currentMap->m_solvedOrderForPuzzle.size() && (m_currentMap->m_nextMap != nullptr))
    {
        for (int mapEntitiesIndex = 0; mapEntitiesIndex < (int)m_currentMap->m_entities.size(); mapEntitiesIndex++)
        {
            if (m_currentMap->m_entities[mapEntitiesIndex]->m_name == "front door")
            {
                m_currentMap->m_entities.erase(m_currentMap->m_entities.begin() + mapEntitiesIndex);

                Feature* exitFeature = new Feature();
                exitFeature->ChangeType(m_currentMap->m_entities[mapEntitiesIndex]->m_name);
                exitFeature->m_featureDefinition->m_subType = "exit";                
                m_currentMap->RandomlyPlacedExit(m_currentMap->m_entities[mapEntitiesIndex]->m_name);
            }

            
        }
    }

    if ((sizeOfPlayerSolution == (int)m_currentMap->m_solvedOrderForPuzzle.size()) && (m_currentMap->m_nextMap == nullptr))
    {
        g_theGame->m_currentGameState = VICTORY;
        m_currentMap->m_totalNumTurnsToSolve = m_currentMap->m_totalNumTurnsToSolve;
        m_currentMap->m_currentNumTurnsTowardsSolvingPuzzle = 0;
        return;
    }
}
