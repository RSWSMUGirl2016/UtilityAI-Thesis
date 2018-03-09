#include "Game/PathGenerator.hpp"
#include "Game/Map.hpp"

//*******************************************************************
PathGenerator::PathGenerator()
{
}

//*******************************************************************
PathGenerator::PathGenerator(Map* map, IntVector2 start, IntVector2 end, Actor* useGCostFor)
    : m_map(map)
    , m_start(start)
    , m_goal(end)
    , m_gCostReferenceActor(useGCostFor)
{
    OpenPathNode(start, nullptr);
    static int nextPathID = 1;
    m_pathID = nextPathID++;
}

//*******************************************************************
PathGenerator::~PathGenerator()
{
    for (int openNodeIndex = 0; openNodeIndex < (int)m_openNodes.size(); openNodeIndex++)
    {
        delete m_openNodes[openNodeIndex];
        m_openNodes[openNodeIndex] = nullptr;
    }
    m_openNodes.clear();
}

//*******************************************************************
bool PathGenerator::Step()
{
    if (m_openNodes.empty())
    {
        return true;
    }
    OpenNode* selectedNode = GetAndCloseBestNode();
    if (selectedNode->m_tile->m_tileCoords == m_goal)
    {
        BuildFinalPath(*selectedNode);
        return true;
    }
    OpenNeighborIfValid(selectedNode, selectedNode->m_tile->m_tileCoords + IntVector2(1, 0));
    OpenNeighborIfValid(selectedNode, selectedNode->m_tile->m_tileCoords + IntVector2(-1, 0));
    OpenNeighborIfValid(selectedNode, selectedNode->m_tile->m_tileCoords + IntVector2(0, 1));
    OpenNeighborIfValid(selectedNode, selectedNode->m_tile->m_tileCoords + IntVector2(0, -1));
    return false;
}

//*******************************************************************
void PathGenerator::OpenPathNode(const IntVector2& tileCoords, OpenNode* parent)
{
    float parentGTotal = parent ? parent->m_totalGCost : 0.f;
    OpenNode* newOpenNode = new OpenNode();
    newOpenNode->m_tile = m_map->GetTileAtTileCoords(tileCoords);
    newOpenNode->m_parent = parent;
    newOpenNode->m_estimatedDistanceToGoal = (float)m_map->CalculateManhattanDistance(tileCoords, m_goal);
    newOpenNode->m_localGCost = newOpenNode->m_tile->GetBaseGCost();
    newOpenNode->m_totalGCost = parentGTotal + newOpenNode->m_localGCost;
    newOpenNode->m_score = newOpenNode->m_totalGCost + newOpenNode->m_estimatedDistanceToGoal;
    newOpenNode->m_tile->m_isOpenInPathID = m_pathID;
    m_openNodes.push_back(newOpenNode);
}

//*******************************************************************
void PathGenerator::BuildFinalPath(const OpenNode& goalNode)
{
    const OpenNode* currentNode = &goalNode;
    while (currentNode)
    {
        m_finalPath.push_back(currentNode->m_tile);
        currentNode = currentNode->m_parent;
    }
}

//*******************************************************************
void PathGenerator::OpenNeighborIfValid(OpenNode* selectedNode, const IntVector2& neighborPosition)
{
    if (!m_map->IsInBounds(neighborPosition))
        return;    
    Tile* neighborTile = m_map->GetTileAtTileCoords(neighborPosition);
    if (neighborTile->m_closedInPathID == m_pathID)
        return;
    if (neighborTile->IsSolid())
        return;
    OpenPathNode(neighborPosition, selectedNode);
}

//*******************************************************************
OpenNode* PathGenerator::GetAndCloseBestNode()
{
    float bestFScore = FLT_MAX;
    OpenNode* bestNode = nullptr;
    int bestIndex = 0;
    for (int openIndex = 0; openIndex < (int)m_openNodes.size(); ++openIndex)
    {
        OpenNode* openNode = m_openNodes[openIndex];
        if (openNode->m_score < bestFScore)
        {
            bestFScore = openNode->m_score;
            bestNode = openNode;
            bestIndex = openIndex;
        }
    }
    m_openNodes[bestIndex] = m_openNodes.back();
    m_openNodes.pop_back();
    bestNode->m_tile->m_closedInPathID = m_pathID;
    return bestNode;
}

//*******************************************************************
OpenNode::OpenNode()
{
}

//*******************************************************************
OpenNode::~OpenNode()
{
    m_tile = nullptr;
    m_parent = nullptr;
}
