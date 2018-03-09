#pragma once
#include "Engine/Math/Vertex3.hpp"
#include "Game/Feature.hpp"
#include "Game/Inventory.hpp"
#include "Game/TileDefinition.hpp"
#include <vector>

class Actor;
class Agent;

enum StateOfPlayerSight
{
    NEVER_SEEN,
    PREVIOUSLY_SEEN,
    CURRENTLY_SEEN
};

struct RayCastResult2D
{
public:
    bool m_didImpact = false;
    Vector2 m_impactPosition = Vector2();
    IntVector2 m_impactTileCoords = IntVector2();
    float m_impactFraction = 0.f;
    Vector2 m_impactSurfaceNormal = Vector2();
};

class Tile
{
public:
    Tile();
    ~Tile();

    void ChangeType(const std::string& tileTypeName);

    void Update();
    void Render() const;
    void Render(std::vector<Vertex3>& fillVertexes, std::vector<Vertex3>& glyphVertexes, std::vector<Vertex3>& tintVertexes) const;

    void RenderFill(std::vector<Vertex3>& fillVertexes) const;
    void RenderGlyph(std::vector<Vertex3>& glyphVertexes) const;
    void RenderTint(std::vector<Vertex3>& tintVertexes) const;

    Vector2 GetCenterOfTile();
    float GetBaseGCost() { return 1.0f; }
    bool IsSolid() { return m_tileDefinition->m_isSolid; };

    Actor* m_occupant;
    Agent* m_aiOccupant;
    Feature* m_feature;
    TileDefinition* m_tileDefinition;
    IntVector2 m_tileCoords;
    Inventory m_inventory;    
    Rgba m_fillColor;
    float m_permanence;
    int m_closedInPathID = -1;
    int m_isOpenInPathID = -1;
    StateOfPlayerSight m_currentStateOfPlayerSight = NEVER_SEEN;
};