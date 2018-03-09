#include "Game/MapGenerator_RoomsAndPaths.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"

//*******************************************************************
MapGenerator_RoomsAndPaths::MapGenerator_RoomsAndPaths(float permanence)
{
    m_player = new Player();
    m_tilesPermanence = permanence;

    CreateRandomlyGeneratedMap();
    if (m_tilesPermanence == 1.0f)
    {
        RandomlyPlacedPlayer();
    }    
    RandomlyPlacedEnemy();
    
    //RandomlyPlacedFeature();
    //RandomlyPlacedExit();
}

//*******************************************************************
MapGenerator_RoomsAndPaths::~MapGenerator_RoomsAndPaths()
{
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::CreateRandomlyGeneratedMap()
{
    SetAllTilesToStone();
    CreateRooms();
    CreateHallways();
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::SetAllTilesToGrass()
{
    for (int tileCoordinateY = 0; tileCoordinateY < SCREEN_HEIGHT; tileCoordinateY++)
    {
        for (int tileCoordinateX = 0; tileCoordinateX < SCREEN_WIDTH; tileCoordinateX++)
        {
            Tile tile = Tile();
            tile.ChangeType("grass");
            tile.m_tileCoords = IntVector2(tileCoordinateX, tileCoordinateY);
            tile.m_permanence = m_tilesPermanence;
            m_tiles.push_back(tile);
        }

    }
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::SetEdgesOfMapToStone()
{
    for (int tileIndex = 0; tileIndex < MAX_NUM_TILES; ++tileIndex)
    {
        Tile& tile = m_tiles[tileIndex];
        IntVector2& coordinates = tile.m_tileCoords;
        if ((coordinates.x == 0) || (coordinates.y == 0) || (coordinates.x == (SCREEN_WIDTH - 1)) || (coordinates.y == (SCREEN_HEIGHT - 1)))
        {
            tile.ChangeType("stone wall");
        }

    }
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::SetAllTilesToStone()
{
    for (int tileCoordinateY = 0; tileCoordinateY < m_dimensions.y; tileCoordinateY++)
    {
        for (int tileCoordinateX = 0; tileCoordinateX < m_dimensions.x; tileCoordinateX++)
        {
            Tile tile = Tile();
            tile.ChangeType("stone wall");
            tile.m_tileCoords = IntVector2(tileCoordinateX, tileCoordinateY);
            tile.m_permanence = m_tilesPermanence;
            m_tiles.push_back(tile);
        }
    }
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::CreateRooms()
{
    for (int currentRoomIndex = 0; currentRoomIndex < MAX_NUM_ROOMS; currentRoomIndex++)
    {
        Room room;
        room.m_minimumWidth = (int)SCREEN_WIDTH / 5;
        room.m_maximumWidth = room.m_minimumWidth + 1;
        room.m_minimumHeight = (int)SCREEN_HEIGHT / 5;
        room.m_maximumHeight = room.m_minimumHeight + 1;

        room.m_decidedWidth = GetRandomIntInRange(room.m_minimumWidth, room.m_maximumWidth);
        room.m_decidedHeight = GetRandomIntInRange(room.m_minimumHeight, room.m_maximumHeight);

        int randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - room.m_decidedWidth - 1);
        int randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - room.m_decidedHeight - 1);

        Tile* chosenTile = ChooseRandomTileForRoom(randomX, randomY);
        bool isAbleToPopulateWithGrass = IsAreaClearForPopulatingWithGrass(chosenTile, room.m_decidedWidth, room.m_decidedHeight);
        while (!isAbleToPopulateWithGrass)
        {
            randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - room.m_decidedWidth - 1);
            randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - room.m_decidedHeight - 1);
            chosenTile = ChooseRandomTileForRoom(randomX, randomY);
            isAbleToPopulateWithGrass = IsAreaClearForPopulatingWithGrass(chosenTile, room.m_decidedWidth, room.m_decidedHeight);
        }

        SetAreaWithGrass(chosenTile, room.m_decidedWidth, room.m_decidedHeight);
        room.m_bottomLeftTileCoords = chosenTile->m_tileCoords;
        m_rooms.push_back(room);
    }
}

//*******************************************************************
Tile* MapGenerator_RoomsAndPaths::ChooseRandomTileForRoom(int randomX, int randomY)
{
    Tile* chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    while (chosenTile->m_tileDefinition->m_type == "grass")
    {
        randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
        randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);
        chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    }
    return chosenTile;
}

//*******************************************************************
bool MapGenerator_RoomsAndPaths::IsAreaClearForPopulatingWithGrass(Tile* chosenTile, int roomWidth, int roomHeight)
{
    IntVector2 tileCoords = chosenTile->m_tileCoords;
    for (int tileCoordinateX = tileCoords.x; tileCoordinateX < (tileCoords.x + roomWidth); tileCoordinateX++)
    {
        for (int tileCoordinateY = tileCoords.y; tileCoordinateY < (tileCoords.y + roomHeight); tileCoordinateY++)
        {
            Tile* currentTile = GetTileAtTileCoords(IntVector2(tileCoordinateX, tileCoordinateY));
            if (tileCoordinateX > (SCREEN_WIDTH - 2) || tileCoordinateY >(SCREEN_HEIGHT - 2))
            {
                return false;
            }
            if (currentTile->m_tileDefinition->m_type == "grass")
            {
                return false;
            }
        }
    }
    return true;
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::SetAreaWithGrass(Tile* chosenTile, int roomWidth, int roomHeight)
{
    IntVector2 tileCoords = chosenTile->m_tileCoords;
    for (int tileCoordinateX = tileCoords.x; tileCoordinateX < (tileCoords.x + roomWidth); tileCoordinateX++)
    {
        for (int tileCoordinateY = tileCoords.y; tileCoordinateY < (tileCoords.y + roomHeight); tileCoordinateY++)
        {
            Tile* currentTile = GetTileAtTileCoords(IntVector2(tileCoordinateX, tileCoordinateY));
            currentTile->ChangeType("grass");
            currentTile->m_permanence = m_tilesPermanence;
        }
    }
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::CreateHallways()
{
    for (int roomIndex = 0; roomIndex < (int)m_rooms.size(); roomIndex++)
    {
        IntVector2 roomTileCoords = IntVector2(m_rooms[roomIndex].m_bottomLeftTileCoords.x, m_rooms[roomIndex].m_bottomLeftTileCoords.y);
        IntVector2 neighborToConnectToTileCoords;
        if (roomIndex == ((int)m_rooms.size() - 1))
        {
            neighborToConnectToTileCoords = m_rooms[0].m_bottomLeftTileCoords;
        }
        else
        {
            neighborToConnectToTileCoords = m_rooms[roomIndex + 1].m_bottomLeftTileCoords;
        }

        CreateHallwayFromOneRoomToANeighbor(roomTileCoords, neighborToConnectToTileCoords);
    }
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::CreateHallwayFromOneRoomToANeighbor(IntVector2 startPoint, IntVector2 endPoint)
{
    int xDirection = endPoint.x - startPoint.x;
    int yDirection = endPoint.y - startPoint.y;

    int upOrRightFirst = GetRandomIntInRange(0, 1);

    if (upOrRightFirst == 0)
    {
        if (xDirection < 0)
        {
            for (int currentXIndex = 0; currentXIndex > xDirection; currentXIndex--)
            {
                Tile* currentTile = GetTileAtTileCoords(IntVector2(startPoint.x + currentXIndex, startPoint.y));
                currentTile->ChangeType("grass");
                currentTile->m_permanence = m_tilesPermanence;
            }
        }
        else
        {
            for (int currentXIndex = 0; currentXIndex < xDirection; currentXIndex++)
            {
                Tile* currentTile = GetTileAtTileCoords(IntVector2(startPoint.x + currentXIndex, startPoint.y));
                currentTile->ChangeType("grass");
                currentTile->m_permanence = m_tilesPermanence;
            }
        }

        if (yDirection < 0)
        {
            for (int currentYIndex = 0; currentYIndex < yDirection; currentYIndex++)
            {
                Tile* currentTile = GetTileAtTileCoords(IntVector2(endPoint.x, startPoint.y + currentYIndex));
                currentTile->ChangeType("grass");
                currentTile->m_permanence = m_tilesPermanence;
            }
        }
        else
        {
            for (int currentYIndex = 0; currentYIndex > yDirection; currentYIndex--)
            {
                Tile* currentTile = GetTileAtTileCoords(IntVector2(endPoint.x, startPoint.y + currentYIndex));
                currentTile->ChangeType("grass");
                currentTile->m_permanence = m_tilesPermanence;
            }
        }
    }
    else
    {
        if (yDirection < 0)
        {
            for (int currentYIndex = 0; currentYIndex > yDirection; currentYIndex--)
            {
                Tile* currentTile = GetTileAtTileCoords(IntVector2(startPoint.x, startPoint.y + currentYIndex));
                currentTile->ChangeType("grass");
                currentTile->m_permanence = m_tilesPermanence;
            }
        }
        else
        {
            for (int currentYIndex = 0; currentYIndex < yDirection; currentYIndex++)
            {
                Tile* currentTile = GetTileAtTileCoords(IntVector2(startPoint.x, startPoint.y + currentYIndex));
                currentTile->ChangeType("grass");
                currentTile->m_permanence = m_tilesPermanence;
            }
        }

        if (xDirection < 0)
        {
            for (int currentXIndex = 0; currentXIndex > xDirection; currentXIndex--)
            {
                Tile* currentTile = GetTileAtTileCoords(IntVector2(startPoint.x + currentXIndex, endPoint.y));
                currentTile->ChangeType("grass");
                currentTile->m_permanence = m_tilesPermanence;
            }
        }
        else
        {
            for (int currentXIndex = 0; currentXIndex < xDirection; currentXIndex++)
            {
                Tile* currentTile = GetTileAtTileCoords(IntVector2(startPoint.x + currentXIndex, endPoint.y));
                currentTile->ChangeType("grass");
                currentTile->m_permanence = m_tilesPermanence;
            }
        }
    }
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::RandomlyPlacedPlayer()
{
    m_player->ChangeType("player");

    int randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
    int randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);

    Tile* chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    while (chosenTile->m_tileDefinition->m_type != "grass" || chosenTile->m_occupant != nullptr || chosenTile->m_feature != nullptr || (int)chosenTile->m_inventory.m_items.size() > 0)
    {
        randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
        randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);
        chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    }

    m_player->m_currentTile = chosenTile;
    AddOccupantAtTileCoords(m_player->m_currentTile->m_tileCoords, m_player);
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::RandomlyPlacedEnemy()
{
    Actor* actor = new Actor();
    actor->ChangeType("goblin");
    actor->RandomlyPopulateInventory();

    int randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
    int randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);

    Tile* chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    while (chosenTile->m_tileDefinition->m_type != "grass" || chosenTile->m_occupant != nullptr || chosenTile->m_feature != nullptr || (int)chosenTile->m_inventory.m_items.size() > 0)
    {
        randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
        randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);
        chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    }

    actor->m_currentTile = chosenTile;
    AddOccupantAtTileCoords(actor->m_currentTile->m_tileCoords, actor);
    m_entities.push_back(actor);
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::RandomlyPlacedFeature()
{
    Feature* feature = new Feature();
    feature->ChangeType("chest");

    int randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
    int randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);

    Tile* chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    while (chosenTile->m_tileDefinition->m_type != "grass" || chosenTile->m_occupant != nullptr || chosenTile->m_feature != nullptr || (int)chosenTile->m_inventory.m_items.size() > 0)
    {
        randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
        randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);
        chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    }

    feature->m_currentTile = chosenTile;
    AddFeatureAtTileCoords(feature->m_currentTile->m_tileCoords, feature);
    m_entities.push_back(feature);
}

//*******************************************************************
void MapGenerator_RoomsAndPaths::RandomlyPlacedExit()
{
    Feature* feature = new Feature();
    feature->ChangeType("front door");

    int randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
    int randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);

    Tile* chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    while (chosenTile->m_tileDefinition->m_type != "grass" || chosenTile->m_occupant != nullptr || chosenTile->m_feature != nullptr || (int)chosenTile->m_inventory.m_items.size() > 0)
    {
        randomX = GetRandomIntInRange(1, (int)SCREEN_WIDTH - 2);
        randomY = GetRandomIntInRange(1, (int)SCREEN_HEIGHT - 2);
        chosenTile = this->GetTileAtTileCoords(IntVector2(randomX, randomY));
    }

    feature->m_currentTile = chosenTile;
    AddFeatureAtTileCoords(feature->m_currentTile->m_tileCoords, feature);
    m_entities.push_back(feature);
}
