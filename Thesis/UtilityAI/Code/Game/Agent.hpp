#pragma once

#include "Game/Actor.hpp"
#include "Game/Goal.hpp"
#include "Game/Location.hpp"
#include "Game/PathGenerator.hpp"
#include "Game/Resource.hpp"
#include "Game/Tile.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVector2.hpp"

#include <vector>

class Agent : public Actor
{
private:
    std::vector<Resource*> m_resources;
    float m_needForWater;
    float m_needForShelter;
    float m_needForProtection;
    float m_needForFood;
    float m_needToReproduce;
    Location* m_home;
    float m_weightCanCarry;

private:
    float m_currentPercentageOfWater; //0.0f - thirsty, 100.0f - not thirsty
    float m_currentExposure;
    float m_currentDanger;
    float m_currentPercentageOfFood;
    float m_currentWantToReproduce;
    float m_currentResources;
    float m_currentCarryingWeight;
    int m_turnsSinceInShelter;
    int m_currentNumberOfTurns;
    float m_maxHealth;

public:
    IntVector2 m_worldLocation;
    char m_glyph;
    Rgba m_glyphColor;
    float m_health;
    int m_lifeSpan;
    Path m_currentPath;
    Goal m_currentGoal;

    std::vector<float> m_currentGoalsScores;

    std::vector<float> m_dailyWaterNeed;
    std::vector<float> m_dailyShelterNeed;
    std::vector<float> m_dailyProtectionNeed;
    std::vector<float> m_dailyFoodNeed;
    std::vector<float> m_dailyReproductionNeed;

    std::vector<Goal> m_allGoalsChosen;
    std::string m_causeOfDeath;

public:
    Agent();
    ~Agent();

private:
    void CalculateNeedForWater();
    void CalculateNeedForShelter();
    void CalculateNeedForProtection();
    void CalculateNeedForFood();
    void SetNeedForReproduction();

private:
    void CalculateCurrentPercentageOfWater();
    void CalculateCurrentExposure();
    void CalculateCurrentDanger();
    void CalculatePercentageOfFood();
    void SetPercentageOfWantingToReproduce();

private:
    float CalculateGoalUtility(Goal* currentGoal);
    bool AmIDeadFromLowHealth();

public:
    float GetAmountofTotalResource(std::string resourceName);
    float GetAmountofTotalResourceType(std::string resourceType);
    float GetAmountofWeightOfATypeOfResource(std::string resourceType);
    bool IsInShelter();
    void DrinkWater();
    void EatFood();
    void AdjustHealthBasedOnExposure();
    void UseResource(Resource* resourceUsed);

public:
    float GetNeedForWater();
    float GetNeedForShelter();
    float GetNeedForProtection();
    float GetNeedForFood();
    float GetNeedToReproduce();
    Goal GetCurrentGoal();
    std::vector<Resource*> GetResources();

public:
    void AddResourceWeightToCurrentWeight(Resource* newResource);
    void RemoveResourceWeightFromCurrentWeight(Resource* usedResource);
    void RemoveResourceWeightFromCurrentWeight(std::string usedResourceName);
    void RemoveResourceWeightFromCurrentWeight(std::string usedResourceName, float amountOfWeightRemoved);
    void SetHome(Location* newHome);
    bool SetNewGoal(const std::vector<Goal*> currentWorldGoals);
    
public:
    void CalculateNeeds();
    void Update();
};