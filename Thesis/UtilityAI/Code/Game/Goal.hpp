#pragma once

#include "Game/GoalAction.hpp"
#include "Game/Resource.hpp"

#include <functional>
#include <vector>

class Agent;

enum Tier
{
    TIER_ONE,
    TIER_TWO,
    TIER_THREE
};

enum BuildType 
{
    INDIVIDUAL,
    GROUP
};

class Goal
{
public:
    std::vector<Resource> m_neededResources;
    std::vector<float> m_importanceOfEachResource;
    std::vector<float> m_minimumOfEachResource;
    std::vector<float> m_maximumOfEachResource;
    std::vector<GoalAction> m_goalActions;
    std::string m_type;
    std::string m_name;
    Tier m_tier;
    IntVector2 m_desiredLocation;
    std::string m_desiredLocationTileType;
    std::function<float(Agent*, Goal*)> m_goalCalculation;
    BuildType m_buildType;

public:
    Goal();
    ~Goal();

public:
    GoalAction GetCurrentGoalAction();
    void RunCurrentGoalAction(Agent* agent);
    std::string GetType();
    std::string GetGoalName();
    Tier GetGoalTier();
    BuildType GetGoalBuildType();
};