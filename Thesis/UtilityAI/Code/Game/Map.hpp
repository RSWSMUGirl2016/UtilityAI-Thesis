#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Engine/RHI/VertexBuffer.hpp"
#include "Game/Entity.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/Tile.hpp"
#include "Game/PathGenerator.hpp"
#include "Game/PuzzleFeature.hpp"
#include <vector>

class Map
{
    friend class PathGenerator;

public:
    MapDefinition* m_mapDefinition;
    std::vector<Entity*> m_entities;
    std::vector<Tile> m_tiles;
    IntVector2 m_dimensions;
    Player* m_player;
    std::vector<VertexBuffer*> m_vbos;
    std::vector<Vertex3> m_fillVertexes;
    std::vector<Vertex3> m_glyphVertexes;
    std::vector<Vertex3> m_debugVertexes;
    std::vector<Vertex3> m_tintVertexes;
    PathGenerator* m_currentPath;
    Path m_testPath;

    Map* m_nextMap = nullptr;
    std::string m_nextMapName = "none";
    Map* m_prevMap = nullptr;
    std::string m_prevMapName = "none";

    std::vector<PuzzleFeature*> m_puzzle;
    std::vector<int> m_solvedOrderForPuzzle;
    std::vector<int> m_playerSolvedOrderForPuzzle;
    int m_totalNumTurnsToSolve;
    int m_currentNumTurnsTowardsSolvingPuzzle = 0;

public:
    Map();
	~Map();

    Map(std::string newMapName);

    void ChangeType(const std::string& mapName);

    int GetTileIndexForTile(const Tile& tile);
    Tile* GetTileAtTileCoords(const IntVector2& tileCoords);
    IntVector2 GetTileCoordinatesForTileIndex(int tileIndex);
    Tile* GetTileForTileIndex(int tileIndex);

    void AddOccupantAtTileCoords(const IntVector2& tileCoords, Actor* actor);
    void RemoveOccupantAtTileCoords(const IntVector2& tileCoords);
    void RemoveOccupantPermanentlyAtTileCoords(const IntVector2& tileCoords);

    void AddAIOccupantAtTileCoords(const IntVector2& tileCoords, Agent* actor);
    void RemoveAIOccupantAtTileCoords(const IntVector2& tileCoords);
    void RemoveAIOccupantPermanentlyAtTileCoords(const IntVector2& tileCoords);

    void AddInventoryToTileAndRemoveFromActor(const IntVector2& tileCoords, Inventory& inventory);
    void AddInventoryToActorAndRemoveFromTile(const IntVector2& tileCoords, Actor* actor);
    void AddInventoryToActorAndRemoveFromTile(const IntVector2& tileCoords, Player* player);
    IntVector2 GetRandomAvailableFloorTile();

    void AddFeatureAtTileCoords(const IntVector2& tileCoords, Feature* feature);
    void RemoveFeatureAtTileCoords(const IntVector2& tileCoords);

    bool IsInBounds(const IntVector2& tileCoords);
    Vector2 GetWorldCenterForTileCoordinates(const IntVector2 tileCoordinates);
    IntVector2 GetTileCoordinatesForWorldPosition(const Vector2& worldPosition) const;
    SIGHT HasLineOfSight(const Vector2& startPosition, const Vector2& endPosition, Actor* usesActorsSight);
    Vector2 GetLineOfSight(const Vector2& startPosition, const Vector2& endPosition, Actor* usesActorsSight);
    RayCastResult2D AmanatidesWooRaycast(Vector2 startPosition, Vector2 endPosition);

    Path GeneratePath(const IntVector2& start, const IntVector2& destination, Actor* useGCostFor = nullptr);
    void StartSteppedPath(const IntVector2& start, const IntVector2& end, Actor* useGCostFor = nullptr);
    bool ContinueSteppedPath(Path& out_pathWhenComplete);
    int CalculateManhattanDistance(const IntVector2& start, const IntVector2& end) const;

    void Update();
    void Render();
    void RenderTiles();
    void RenderTestAStarPath();
    void ClearVertexes();
    void RandomlyPlacedExit(std::string exitType);
    void RandomlyPlacePuzzleFeatures();
};