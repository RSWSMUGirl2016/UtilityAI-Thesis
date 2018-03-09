#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Game/Tile.hpp"
#include <vector>

class Map;
class Actor;

typedef std::vector<Tile*> Path;

class OpenNode
{
public:
    OpenNode();
    ~OpenNode();

    Tile* m_tile = nullptr;
    OpenNode* m_parent; //nullptr for start tile nullptr
    float m_localGCost = 1.0f;
    float m_totalGCost = 0.f; //g
    float m_estimatedDistanceToGoal = 0.f; //h
    float m_score = 0.f; //f = g + h
};

class PathGenerator
{
public:
    IntVector2 m_start;
    IntVector2 m_goal;
    Map* m_map = nullptr;
    Actor* m_gCostReferenceActor = nullptr;
    std::vector<OpenNode*> m_openNodes;
    Path m_finalPath;
    int m_pathID;
    
    PathGenerator();
    ~PathGenerator();
    PathGenerator(Map* map, IntVector2 start, IntVector2 end, Actor* useGCostFor);
    bool Step();
    void OpenPathNode(const IntVector2& tileCoords, OpenNode* parent);
    void BuildFinalPath(const OpenNode& goalNode);
    void OpenNeighborIfValid(OpenNode* selectedNode, const IntVector2& neighborPosition);
    OpenNode* GetAndCloseBestNode();

};