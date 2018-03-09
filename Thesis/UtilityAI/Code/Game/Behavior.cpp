#include "Game/Behavior.hpp"
#include "Game/Actor.hpp"
#include "Game/Tile.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"

//*******************************************************************
Behavior* Behavior::Create(const tinyxml2::XMLElement& xmlElement)
{
    std::string type;
    type = ParseXmlAttribute(xmlElement, "type", type);
    if (type == "wander")
    {
        return new WanderBehavior(xmlElement);
    }
    else if (type == "pursue")
    {
        return new PursueBehavior(xmlElement);
    }
    else if (type == "flee")
    {
        return new FleeBehavior(xmlElement);
    }
    else if (type == "attack")
    {
        return new AttackBehavior(xmlElement);
    }
    return nullptr;
}

//*******************************************************************
Behavior::Behavior()
    : m_target(nullptr)
{
}

//*******************************************************************
Behavior::~Behavior()
{
}

//*******************************************************************
float Behavior::CalculateUtility()
{
    return 0.f;
}

//*******************************************************************
void Behavior::GeneratePath(Actor* myself, IntVector2 goalPoint)
{
    UNUSED(myself);
    UNUSED(goalPoint);
}

//*******************************************************************
void Behavior::Attack(Actor* myself)
{
    UNUSED(myself);
}

//*******************************************************************
WanderBehavior::WanderBehavior()
{
}

//*******************************************************************
WanderBehavior::WanderBehavior(std::string type)
{
    m_type = type;
}

//*******************************************************************
float WanderBehavior::CalculateUtility()
{
    if (m_target != nullptr)
        return 0.f;
    return 1.f;
}

//*******************************************************************
void WanderBehavior::GeneratePath(Actor* myself, IntVector2 goalPoint)
{
    m_currentPath = myself->m_currentMap->GeneratePath(myself->m_tileCoords, goalPoint, nullptr);
    std::reverse(m_currentPath.begin(), m_currentPath.end());
    m_currentTileCoordsInPathIndex = 0;
}

//*******************************************************************
WanderBehavior::~WanderBehavior()
{
}

//*******************************************************************
WanderBehavior::WanderBehavior(const tinyxml2::XMLElement& xmlElement)
{
    m_type = "wander";
    m_numberOfSteps = ParseXmlAttribute(xmlElement, "steps", m_numberOfSteps);
}

//*******************************************************************
PursueBehavior::PursueBehavior()
{

}

//*******************************************************************
PursueBehavior::PursueBehavior(const tinyxml2::XMLElement& xmlElement)
{
    m_type = "pursue";
    m_numberOfSteps = ParseXmlAttribute(xmlElement, "steps", m_numberOfSteps);
}

//*******************************************************************
PursueBehavior::PursueBehavior(std::string type)
{
    m_type = "pursue";
}

//*******************************************************************
float PursueBehavior::CalculateUtility()
{
    if (m_target == nullptr)
        return 0.f;
    float utility = 1.f;
    if (float(m_target->m_currentHealth / m_target->m_totalHealth) < 0.5f)
    {
        utility += 0.5f;
    }
    return utility;
}

//*******************************************************************
void PursueBehavior::GeneratePath(Actor* myself, IntVector2 goalPoint)
{
    m_currentPath = myself->m_currentMap->GeneratePath(myself->m_tileCoords, goalPoint, nullptr);
    std::reverse(m_currentPath.begin(), m_currentPath.end());
    m_currentTileCoordsInPathIndex = 0;
}

//*******************************************************************
PursueBehavior::~PursueBehavior()
{

}

//*******************************************************************
FleeBehavior::FleeBehavior()
{

}

//*******************************************************************
FleeBehavior::FleeBehavior(std::string type)
{
    m_type = type;
}

//*******************************************************************
FleeBehavior::~FleeBehavior()
{

}

//*******************************************************************
FleeBehavior::FleeBehavior(const tinyxml2::XMLElement& xmlElement)
{
    m_type = "flee";
    m_numberOfSteps = ParseXmlAttribute(xmlElement, "steps", m_numberOfSteps);
}

//*******************************************************************
float FleeBehavior::CalculateUtility()
{
    if (m_target == nullptr)
        return 0.f;
    float utility = 1.f;
    if (float(m_target->m_currentHealth / m_target->m_totalHealth) > 0.5f)
    {
        utility += 1.0f;
    }
    return utility;
}

//*******************************************************************
void FleeBehavior::GeneratePath(Actor* myself, IntVector2 goalPoint)
{
    m_currentPath = myself->m_currentMap->GeneratePath(myself->m_tileCoords, goalPoint, nullptr);
    std::reverse(m_currentPath.begin(), m_currentPath.end());
    m_currentTileCoordsInPathIndex = 0;
}

//*******************************************************************
AttackBehavior::AttackBehavior()
{

}

AttackBehavior::AttackBehavior(std::string type)
{
    m_type = type;
}

//*******************************************************************
AttackBehavior::~AttackBehavior()
{

}

//*******************************************************************
AttackBehavior::AttackBehavior(const tinyxml2::XMLElement& xmlElement)
{
    m_type = "attack";
    m_range = ParseXmlAttribute(xmlElement, "range", m_range);
}

//*******************************************************************
float AttackBehavior::CalculateUtility()
{
    if (m_target == nullptr)
        return 0.f;
    float utility = 1.f;
    if (float(m_target->m_currentHealth / m_target->m_totalHealth) <= 0.5f)
    {
        utility += 1.0f;
    }
    return utility;
}

//*******************************************************************
void AttackBehavior::GeneratePath(Actor* myself, IntVector2 goalPoint)
{
    m_currentPath = myself->m_currentMap->GeneratePath(myself->m_tileCoords, goalPoint, nullptr);
    std::reverse(m_currentPath.begin(), m_currentPath.end());
    m_currentTileCoordsInPathIndex = 0;
}

//*******************************************************************
void AttackBehavior::Attack(Actor* myself)
{
    int rangeFromTarget = m_target->m_currentMap->CalculateManhattanDistance(myself->m_tileCoords, m_target->m_tileCoords);
    if (rangeFromTarget <= m_range)
    {
        myself->Attack(*m_target);
    }
}

