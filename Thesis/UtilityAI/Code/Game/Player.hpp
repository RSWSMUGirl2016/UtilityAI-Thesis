#pragma once
#include "Game/Actor.hpp"
#include "Game/Tile.hpp"

class Player : public Actor
{
public:
    IntVector2 m_directionExitedMap;

    Player();
	~Player();

    void ChangeType(const std::string& playerName);

    void Update();
    void Render() const;

    void FogOfWar();
    void SetCurrentlyVisibleTilesToPreviouslySeen();
    virtual void Act();
};