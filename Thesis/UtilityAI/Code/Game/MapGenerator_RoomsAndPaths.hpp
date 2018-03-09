#pragma once
#include "Game/MapGenerator.hpp"

class Tile;

struct Room
{
    int m_minimumWidth;
    int m_maximumWidth;
    int m_minimumHeight;
    int m_maximumHeight;

    int m_decidedWidth;
    int m_decidedHeight;

    IntVector2 m_bottomLeftTileCoords;
};

struct Hallway
{
    int length;
};

class MapGenerator_RoomsAndPaths : public MapGenerator
{
public:
    std::vector<Room> m_rooms;

    MapGenerator_RoomsAndPaths(float permanence);
    ~MapGenerator_RoomsAndPaths();

    void CreateRandomlyGeneratedMap();
    void SetAllTilesToGrass();
    void SetEdgesOfMapToStone();
    void SetAllTilesToStone();
    void CreateRooms();

    Tile* ChooseRandomTileForRoom(int randomX, int randomY);
    bool IsAreaClearForPopulatingWithGrass(Tile* chosenTile, int roomWidth, int roomHeight);
    void SetAreaWithGrass(Tile* chosenTile, int roomWidth, int roomHeight);
    void CreateHallways();
    void CreateHallwayFromOneRoomToANeighbor(IntVector2 startPoint, IntVector2 endPoint);

    void RandomlyPlacedPlayer();
    void RandomlyPlacedEnemy();
    void RandomlyPlacedFeature();
    void RandomlyPlacedExit();
};