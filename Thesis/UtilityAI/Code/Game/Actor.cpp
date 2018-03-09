#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Game/Actor.hpp"
#include "Game/LootDefinition.hpp"
#include "Game/Map.hpp"

//*******************************************************************
Actor::Actor()
    : m_actorDefinition(nullptr)
    , m_tileCoords(IntVector2())
    , m_inventory(Inventory())
    , m_currentBehavior(nullptr)
{
}

//*******************************************************************
Actor::~Actor()
{
}

//*******************************************************************
void Actor::ChangeType(const std::string& actorName)
{
    m_actorDefinition = ActorDefinition::GetActorDefinition(actorName);
    
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
    
    Behavior* tempCurrentBehavior = m_actorDefinition->m_behaviors[0];
    ChooseCurrentBehavior(tempCurrentBehavior);
}

//*******************************************************************
void Actor::Render() const
{
}

//*******************************************************************
void Actor::Update()
{  
    SetActorsCurrentlyVisibleToMe();
    ChangeCurrentBehavior();

    m_currentBehavior->m_currentTileCoordsInPathIndex++;

    if (m_currentBehavior->m_type == "wander")
    {
        WanderAct();
    }
    else if (m_currentBehavior->m_type == "pursue")
    {
        PursueAct();
    }
    else if (m_currentBehavior->m_type == "flee")
    {

        FleeAct();
    }
    else if (m_currentBehavior->m_type == "attack")
    {
        AttackAct();
    }

    IntVector2 newTileCoords = m_currentBehavior->m_currentPath[m_currentBehavior->m_currentTileCoordsInPathIndex]->m_tileCoords;
    MoveToNewTile(newTileCoords);
}

//*******************************************************************
void Actor::ChangeCurrentBehavior()
{
    IntVector2 playerTileCoords = m_currentMap->m_player->m_tileCoords;
    int distanceFromPlayerToMe = m_currentMap->CalculateManhattanDistance(m_tileCoords, playerTileCoords);
    if (distanceFromPlayerToMe <= 5)
    {
        m_currentBehavior->m_target = m_currentMap->m_player;

        float highestUtility = 0.f;
        Behavior* bestNextBehavior = nullptr;
        for (int behaviorIndex = 0; behaviorIndex < (int)m_actorDefinition->m_behaviors.size(); behaviorIndex++)
        {
            Behavior* behavior = m_actorDefinition->m_behaviors[behaviorIndex];
            behavior->m_target = m_currentMap->m_player;
            if (behavior->CalculateUtility() > highestUtility)
            {
                highestUtility = behavior->CalculateUtility();
                bestNextBehavior = behavior;
            }
        }
        if (bestNextBehavior != nullptr)
        {

            ChooseCurrentBehavior(bestNextBehavior);
        }
    }
}

//*******************************************************************
void Actor::ChooseCurrentBehavior(Behavior* newBehavior)
{
    if (newBehavior->m_type == "wander")
    {
        m_currentBehavior = new WanderBehavior(newBehavior->m_type);
    }
    else if (newBehavior->m_type == "pursue")
    {
        m_currentBehavior = new PursueBehavior(newBehavior->m_type);
    }
    else if (newBehavior->m_type == "flee")
    {

        m_currentBehavior = new FleeBehavior(newBehavior->m_type);
    }
    else if (newBehavior->m_type == "attack")
    {
        m_currentBehavior = new AttackBehavior(newBehavior->m_type);
    }
}

//*******************************************************************
void Actor::WanderAct()
{
    if ((m_currentBehavior->m_currentTileCoordsInPathIndex == (int)m_currentBehavior->m_currentPath.size()) || m_currentBehavior->m_currentPath.empty())
    {
        IntVector2 goalTileCoords = m_currentMap->GetRandomAvailableFloorTile();
        m_currentBehavior->GeneratePath(this, goalTileCoords);
        while ((int)m_currentBehavior->m_currentPath.size() == 0)
        {
            goalTileCoords = m_currentMap->GetRandomAvailableFloorTile();
            m_currentBehavior->GeneratePath(this, goalTileCoords);
        }
        m_currentBehavior->m_currentTileCoordsInPathIndex++;
    }
}

//*******************************************************************
void Actor::PursueAct()
{
    m_currentBehavior->m_target = m_currentMap->m_player;
    IntVector2 goalTileCoords = m_currentMap->m_player->m_tileCoords;
    m_currentBehavior->GeneratePath(this, goalTileCoords);
    m_currentBehavior->m_currentTileCoordsInPathIndex++;
}

//*******************************************************************
void Actor::FleeAct()
{
    m_currentBehavior->m_target = m_currentMap->m_player;
    std::vector<IntVector2> allPossibleTileCoords;

    int counter = 0;
    if (counter < 10)
    {
        IntVector2 possibleTileCoords = m_currentMap->GetRandomAvailableFloorTile();
        allPossibleTileCoords.push_back(possibleTileCoords);
        counter++;
    }

    IntVector2 goalTileCoords = allPossibleTileCoords[0];
    for (int possiblesIndex = 1; possiblesIndex < ((int)allPossibleTileCoords.size() - 1); possiblesIndex++)
    {
        int currentManhattan = m_currentMap->CalculateManhattanDistance(m_currentMap->m_player->m_tileCoords, goalTileCoords);
        int nextManhattan = m_currentMap->CalculateManhattanDistance(m_currentMap->m_player->m_tileCoords, allPossibleTileCoords[possiblesIndex + 1]);
        if (currentManhattan < nextManhattan)
        {
            goalTileCoords = allPossibleTileCoords[possiblesIndex + 1];
        }
    }

    m_currentBehavior->GeneratePath(this, goalTileCoords);
    while ((int)m_currentBehavior->m_currentPath.size() == 0)
    {
        goalTileCoords = m_currentMap->GetRandomAvailableFloorTile();
        m_currentBehavior->GeneratePath(this, goalTileCoords);
    }
    m_currentBehavior->m_currentTileCoordsInPathIndex++;
}

//*******************************************************************
void Actor::AttackAct()
{
    m_currentBehavior->m_target = m_currentMap->m_player;
    IntVector2 goalTileCoords = m_currentMap->m_player->m_tileCoords;
    m_currentBehavior->GeneratePath(this, goalTileCoords);
    m_currentBehavior->m_currentTileCoordsInPathIndex++;
}

//*******************************************************************
void Actor::CheckMyNeighborsForAnOpponent()
{
    IntVector2 northTileCoords = m_currentTile->m_tileCoords + IntVector2(1, 0);
    IntVector2 southTileCoords = m_currentTile->m_tileCoords + IntVector2(-1, 0);
    IntVector2 eastTileCoords = m_currentTile->m_tileCoords + IntVector2(0, 1);
    IntVector2 westTileCoords = m_currentTile->m_tileCoords + IntVector2(0, -1);

    if (m_currentMap->GetTileAtTileCoords(northTileCoords)->m_occupant != nullptr)
    {
        Actor& opponent = *m_currentMap->GetTileAtTileCoords(northTileCoords)->m_occupant;
        if (opponent.m_faction == m_faction)
            return;
        Attack(opponent);
    }
    if (m_currentMap->GetTileAtTileCoords(southTileCoords)->m_occupant != nullptr)
    {
        Actor& opponent = *m_currentMap->GetTileAtTileCoords(southTileCoords)->m_occupant;
        if (opponent.m_faction == m_faction)
            return;
        Attack(opponent);
    }
    if (m_currentMap->GetTileAtTileCoords(eastTileCoords)->m_occupant != nullptr)
    {
        Actor& opponent = *m_currentMap->GetTileAtTileCoords(eastTileCoords)->m_occupant;
        if (opponent.m_faction == m_faction)
            return;
        Attack(opponent);
    }
    if (m_currentMap->GetTileAtTileCoords(westTileCoords)->m_occupant != nullptr)
    {
        Actor& opponent = *m_currentMap->GetTileAtTileCoords(westTileCoords)->m_occupant;
        if (opponent.m_faction == m_faction)
            return;
        Attack(opponent);
    }
}

//*******************************************************************
void Actor::MoveToNewTile(IntVector2 newTileCoords)
{
    if (m_currentMap->GetTileAtTileCoords(newTileCoords)->m_tileDefinition->m_isSolid || (m_currentMap->GetTileAtTileCoords(newTileCoords)->m_occupant != nullptr))
    {                     
        return;
    }        

    if (m_currentMap->GetTileAtTileCoords(newTileCoords)->m_feature != nullptr)
    {
        if (m_currentMap->GetTileAtTileCoords(newTileCoords)->m_feature->m_isSolid)
            return;
    }

    m_currentMap->RemoveOccupantAtTileCoords(m_currentTile->m_tileCoords);
    m_currentMap->AddOccupantAtTileCoords(newTileCoords, this);
    m_currentTile = m_currentMap->GetTileAtTileCoords(newTileCoords);
}

//*******************************************************************
void Actor::Attack(Actor& opponent)
{
    int opponentHealth = opponent.m_currentHealth;
    //int opponentDamage = opponent.m_damage;
    int opponentDefense = opponent.m_defense;

    //int myHealth = m_currentHealth;
    int myDamage = m_damage;
    //int myDefense = m_defense;

    int overallDamageToAttacker = myDamage - opponentDefense;
    int opponentNewHealth;
    if (overallDamageToAttacker > 0)
    {
        opponentNewHealth = opponentHealth - overallDamageToAttacker;
        opponent.SetCurrentHealth(opponentNewHealth);

        std::string debuggerMessage = opponent.m_name + "'s new health is " + std::to_string(opponentNewHealth);
        DebuggerPrintf(debuggerMessage.c_str());

        bool isOpponentDead = opponent.IsDead();
        if (isOpponentDead)
        {
            opponent.OnDeath(opponent);
        }
    }
}

//*******************************************************************
void Actor::SetCurrentHealth(int newCurrentHealth)
{
    m_currentHealth = newCurrentHealth;
}

//*******************************************************************
bool Actor::IsDead()
{
    int health = m_currentHealth;
    if (health < 1)
    {
        return true;
    }
    return false;
}

//*******************************************************************
void Actor::OnDeath(Actor& opponent)
{
    m_currentMap->RemoveOccupantPermanentlyAtTileCoords(opponent.m_tileCoords);
}

//*******************************************************************
void Actor::RandomlyPopulateInventory()
{
    Actor myTypeOfActor = Actor();
    myTypeOfActor.ChangeType(m_name);

    std::vector<Item> possibleItems = LootDefinition::GetItemsBasedOnActor(m_name);
    if ((int)possibleItems.size() > 0)
    {
        AddItemstoPossibleItemsForInventory(possibleItems, myTypeOfActor.m_inventory.m_items);
        AddItemsToInventory(possibleItems);
    }    
}

//*******************************************************************
bool Actor::ActorExistsInLootDefinition(std::vector<Actor>& actors, Actor myTypeOfActor)
{
    for (int actorsIndex = 0; actorsIndex < (int)actors.size(); actorsIndex++)
    {
        if (myTypeOfActor.m_name == actors[actorsIndex].m_name)
        {
            return true;
        }
    }
    return false;
}

//*******************************************************************
void Actor::AddItemstoPossibleItemsForInventory(std::vector<Item>& possibleItems, std::vector<Item*> newItems)
{
    for (int newItemsIndex = 0; newItemsIndex < (int)newItems.size(); newItemsIndex++)
    {
        int currentWeightSpaceLeftInInventory = m_totalWeight - m_currentWeight;
        if ((m_currentWeight <= m_totalWeight) && (newItems[newItemsIndex]->m_weight <= currentWeightSpaceLeftInInventory))
        {
            Item item = Item();
            item.ChangeType(newItems[newItemsIndex]->m_name);
            possibleItems.push_back(item);
            m_currentWeight += newItems[newItemsIndex]->m_weight;
        }
    }
}

//*******************************************************************
void Actor::AddItemsToInventory(std::vector<Item>& possibleItems)
{
    for (int possibleItemsIndex = 0; possibleItemsIndex < (int)possibleItems.size(); possibleItemsIndex++)
    {
        Item* newItem = new Item();
        newItem->ChangeType(possibleItems[possibleItemsIndex].m_name);
        m_inventory.m_items.push_back(newItem);
    }
}

//*******************************************************************
void Actor::AddLootedItemstoInventory(std::vector<Item*> newItems)
{
    for (int newItemsIndex = 0; newItemsIndex < (int)newItems.size(); newItemsIndex++)
    {
        int currentWeightSpaceLeftInInventory = m_totalWeight - m_currentWeight;
        if ((m_currentWeight <= m_totalWeight) && (newItems[newItemsIndex]->m_weight <= currentWeightSpaceLeftInInventory))
        {
            Item* newItem = new Item();
            newItem->ChangeType(newItems[newItemsIndex]->m_name);
            m_inventory.m_items.push_back(newItem);
            m_currentWeight += newItems[newItemsIndex]->m_weight;
        }
    }
}

//*******************************************************************
void Actor::AddStatsBoostsFromItemsInInventory()
{
    std::vector<Item*>& myCurrentItems = m_inventory.m_items;
    for (int myCurrentItemsIndex = 0; myCurrentItemsIndex < (int)myCurrentItems.size(); myCurrentItemsIndex++)
    {
        EquipSlot itemEquipSlot = myCurrentItems[myCurrentItemsIndex]->m_equipSlot;
        if (itemEquipSlot != NONE)
        {
            std::map<EquipSlot, Item*>::iterator equipSlotAndItemMapIterator;
            equipSlotAndItemMapIterator = m_equippedItems.find(itemEquipSlot);
            if (equipSlotAndItemMapIterator != m_equippedItems.end())
            {
                Item* equippedItem = equipSlotAndItemMapIterator->second;
                if((equippedItem->m_health > myCurrentItems[myCurrentItemsIndex]->m_health) || (equippedItem->m_defense > myCurrentItems[myCurrentItemsIndex]->m_defense) || (equippedItem->m_damage > myCurrentItems[myCurrentItemsIndex]->m_damage))
                    continue;
            }

            bool isItemAnInstantUse = myCurrentItems[myCurrentItemsIndex]->m_instantUse;
            if (!isItemAnInstantUse)
            {
                m_equippedItems[itemEquipSlot] = myCurrentItems[myCurrentItemsIndex];

                m_currentHealth += myCurrentItems[myCurrentItemsIndex]->m_health;
                m_defense += myCurrentItems[myCurrentItemsIndex]->m_defense;
                m_damage += myCurrentItems[myCurrentItemsIndex]->m_damage;
            }
        }
    }
}

//*******************************************************************
void Actor::SetActorsCurrentlyVisibleToMe()
{
    ClearActorsCurrentlyVisibleToMe();
    m_tilesCurrentlyVisibleToMe.clear();

    Tile* currentTile = m_currentMap->GetTileAtTileCoords(m_tileCoords);
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
            m_tilesCurrentlyVisibleToMe.push_back(allTilesInCurrentMapIndex);
            if (tile.m_occupant != nullptr)
            {
                m_actorsCurrentlyVisibleToMe.push_back(*tile.m_occupant);
            }
        }
    }
}

//*******************************************************************
void Actor::ClearActorsCurrentlyVisibleToMe()
{
    m_actorsCurrentlyVisibleToMe.clear();
}
