#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Game/Inventory.hpp"
#include "Game/Stats.hpp"
#include <vector>

class Map;
class Tile;

class Entity
{
public:
    Map* m_currentMap;
    Tile* m_currentTile;
    Inventory m_inventory;
    std::vector<Stats> m_stats;
    std::string m_name;
    int m_currentHealth;
    int m_totalHealth;
    int m_damage;
    int m_defense;
    char m_glyph;
    Rgba m_gylphColor;
    Rgba m_fillColor;
    std::string m_faction;
    int m_totalWeight;
    int m_currentWeight;
    bool m_playerTouched;

	Entity();
	~Entity();

    virtual void Update();
    void Render();
};