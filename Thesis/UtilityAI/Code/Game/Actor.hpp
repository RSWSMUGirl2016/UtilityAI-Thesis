#pragma once
#include "Game/ActorDefinition.hpp"
#include "Game/Entity.hpp"

class Actor : public Entity
{
public:
    Actor();
    ~Actor();

    void ChangeType(const std::string& actorName);

    void Update();
    void Render() const;


    void ChangeCurrentBehavior();
    void ChooseCurrentBehavior(Behavior* newBehavior);
    void WanderAct();
    void PursueAct();
    void FleeAct();
    void AttackAct();
    virtual void CheckMyNeighborsForAnOpponent();
    virtual void MoveToNewTile(IntVector2 newTileCoords);
    virtual void Attack(Actor& opponent);
    virtual void SetCurrentHealth(int newCurrentHealth);

    virtual bool IsDead();
    virtual void OnDeath(Actor& opponent);

    virtual void RandomlyPopulateInventory();
    bool ActorExistsInLootDefinition(std::vector<Actor>& actors, Actor myTypeOfActor);
    virtual void AddItemstoPossibleItemsForInventory(std::vector<Item>& possibleItems, std::vector<Item*> newItems);
    virtual void AddItemsToInventory(std::vector<Item>& possibleItems);
    void AddLootedItemstoInventory(std::vector<Item*> newItems);
    void AddStatsBoostsFromItemsInInventory();

    void SetActorsCurrentlyVisibleToMe();
    virtual void ClearActorsCurrentlyVisibleToMe();

    ActorDefinition* m_actorDefinition;
    IntVector2 m_tileCoords;
    Inventory m_inventory;
    std::map<EquipSlot, Item*> m_equippedItems;
    Behavior* m_currentBehavior;
    std::vector<int> m_tilesCurrentlyVisibleToMe;
    std::vector<Actor> m_actorsCurrentlyVisibleToMe;
    float m_lengthOfSight = 10.f;
};