#pragma once

#include "Game/Resource.hpp"
#include "Engine/Math/IntVector2.hpp"

#include <vector>

enum AnimalType
{
    ANIMAL_CHICKEN,
    ANIMAL_BEAR,
    ANIMAL_WOLF,
    ANIMAL_DEER,
    ANIMAL_RABBIT
};

class Animal
{
private:
    std::vector<Resource*> m_resources;
    AnimalType m_animalType;
    float m_health;
    float m_attack;
    float m_defense;
    IntVector2 m_worldLocation;

public:
    void Attack();
    void Move();
    std::vector<Resource*> DropResources();
    void Die();
};