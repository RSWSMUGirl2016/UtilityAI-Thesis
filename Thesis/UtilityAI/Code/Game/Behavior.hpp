#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include <vector>

class Actor;
class Tile;

enum SIGHT
{
    IN_SIGHT,
    NOT_IN_SIGHT,
    ENEMY_IN_SIGHT
};

class Behavior
{
public:
    std::string m_type;
    Actor* m_target;
    std::vector<Tile*> m_currentPath;
    int m_currentTileCoordsInPathIndex;

    virtual Behavior* Clone() const = 0;
    static Behavior* Create(const tinyxml2::XMLElement& xmlElement);

    Behavior();
    ~Behavior();

    virtual float CalculateUtility();    
    virtual void GeneratePath(Actor* myself, IntVector2 goalPoint);
    virtual void Attack(Actor* myself);
};

class WanderBehavior : public Behavior
{
public:
    int m_numberOfSteps;    

    virtual Behavior* Clone() const override
    {
        return new WanderBehavior(*this);
    }

    WanderBehavior();
    ~WanderBehavior();

    WanderBehavior(const tinyxml2::XMLElement& xmlElement);
    WanderBehavior(std::string type);


    float CalculateUtility();
    void GeneratePath(Actor* myself, IntVector2 goalPoint);
};

class PursueBehavior : public Behavior
{
public:
    int m_numberOfSteps;

    virtual Behavior* Clone() const override
    {
        return new PursueBehavior(*this);
    }

    PursueBehavior();
    ~PursueBehavior();

    PursueBehavior(const tinyxml2::XMLElement& xmlElement);
    PursueBehavior(std::string type);

    float CalculateUtility();
    void GeneratePath(Actor* myself, IntVector2 goalPoint);
};

class FleeBehavior : public Behavior
{
public:
    int m_numberOfSteps;

    virtual Behavior* Clone() const override
    {
        return new FleeBehavior(*this);
    }

    FleeBehavior();
    ~FleeBehavior();

    FleeBehavior(const tinyxml2::XMLElement& xmlElement);
    FleeBehavior(std::string type);

    float CalculateUtility();
    void GeneratePath(Actor* myself, IntVector2 goalPoint);
};

class AttackBehavior : public Behavior
{
public:
    int m_range;

    virtual Behavior* Clone() const override
    {
        return new AttackBehavior(*this);
    }

    AttackBehavior();
    ~AttackBehavior();

    AttackBehavior(const tinyxml2::XMLElement& xmlElement);
    AttackBehavior(std::string type);

    float CalculateUtility();
    void GeneratePath(Actor* myself, IntVector2 goalPoint);
    void Attack(Actor* myself);
};