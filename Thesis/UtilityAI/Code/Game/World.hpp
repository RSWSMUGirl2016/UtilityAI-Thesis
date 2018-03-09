#pragma once

#include "Game/Agent.hpp"
#include "Game/Animal.hpp"
#include "Game/Location.hpp"
#include "Game/Map.hpp"

#include <map>

const int MAX_NUM_AGENTS_IN_DEBUG_TEXT = 4;

class World
{
public:
    std::vector<Agent*> m_agents;
    std::vector<Resource*> m_resources;
    std::vector<Goal*> m_worldGoals;
    std::vector<Location*> m_locations;
    std::vector<Animal*> m_animals;

    std::map<int, int> m_dayAndPopulationData;
    std::map<int, int> m_dayAndIndividualGoalsChosen;
    std::map<int, int> m_dayAndGroupGoalsChosen;

    std::map<int, int> m_turnAndPopulationData;
    std::map<int, int> m_turnAndIndividualGoalsChosen; //light blue
    std::map<int, int> m_turnAndGroupGoalsChosen; //purple
    std::map<int, int> m_turnAndWaterGoalsData; //Blue
    std::map<int, int> m_turnAndFoodGoalsData; //Red
    std::map<int, int> m_turnAndShelterGoalsData; //Purple
    std::map<int, int> m_turnAndProtectionGoalsData; //Green
    std::map<int, int> m_turnAndReproductionGoalsData; //Orange
    std::map<int, int> m_turnAndGenericGoalsData; //White

    std::vector<char> m_agentsThatAreDead;
    std::vector<std::string> m_agentsCauseOfDeath;

    int m_currentTurn;
    int m_currentDay;
    Tier m_currentTier;
    int m_currentDayPopulation;
    int m_previousDayPopulation;
    int m_currentDayDeaths;
    int m_currentDayNewPopulants;

    float m_growthRate;
    float m_deathRate;
    float m_birthRate;

    std::vector<float> m_worldsDailyWaterNeed;
    std::vector<float> m_worldsDailyShelterNeed;
    std::vector<float> m_worldsDailyProtectionNeed;
    std::vector<float> m_worldsDailyFoodNeed;
    std::vector<float> m_worldsDailyReproductionNeed;

    std::vector<int> m_currentNumberOfResourcesPerTurn;

private:
    float m_populationsNeedForWater;
    float m_populationsNeedForShelter;
    float m_populationsNeedForProtection;
    float m_populationsNeedForFood;
    float m_populationsNeedForReproduction;

    int m_maxNumberOfWaterTiles;
    int m_maxNumberOfWoodTiles;
    int m_maxNumberOfMetalTiles;
    int m_maxNumberOfRockTiles;
    int m_maxNumberOfFoodTiles;
    int m_maxNumberOfAnimalTiles;

    int m_numberOfIndividualGoalsDaily;
    int m_numberOfGroupGoalsDaily;

public:
    Map* m_map;
    std::vector<GoalAction*> m_goalActions;

public:
    World();
    World(std::string mapName);
    ~World();

private:
    void CalculateGrowthRate();
    void CalculateDeathRate();
    void CalculateBirthRate();
    bool IsGrowthRateStagnant();
    bool IsDeathRateOverComingGrowth();

private:
    void CalculatePopulationsNeedForWater();
    void CalculatePopulationsNeedForShelter();
    void CalculatePopulationsNeedForProtection();
    void CalculatePopulationsNeedForFood();
    void CalculatePopulationsNeedForReproduction();

public:
    int GetCurrentTurn();
    int GetCurrentDay();
    bool AgentDiedOfDisease();
    void UpdateTypesOfGoalsData();
    void UpdateAgents();
    void UpdateLocations();
    void UpdateDay();
    void CalculateWorldNeeds();
    void UpdateResourcesPerTurnData();

public:
    void UpdateWorld();
    float GetGrowthRate();
    float GetDeathRate();
    void ChangeAgentsGoalToNewGoal(Goal* newGoal, Agent* agentToChange);
    void AddNewAGentToWorld();
    bool RemoveLocationFromWorld(GoalAction* action, Location* locationToRemove);
    bool RemoveAgentFromWorld(GoalAction* action, Agent* agentToRemove);
    void AddRandomTestAgentToSimulation();
    IntVector2 FindRandomStartingDirtTileForAgent();
    IntVector2 FindRandomStartingDirtTile();
    IntVector2 FindRandomStartingWaterTileForAgent();
    void ReplinishWoodInWorld();
    void ChangeRandomDirtTileToWood();
    IntVector2 FindRandomResourceTileForAgent(std::string resourceType);

public:
    void Render() const;
    void RenderWorldInformationDebug() const;
    void RenderAgentsInformationDebug() const;
    void RenderInfoForOneAgentDebugText(Agent* currentAgent, int screenWidthMultiplier) const;
    void RenderGoalScoresForOneAgentDebugText(Agent* currentAgent, int screenWidthMultiplier) const;
    void RenderAgentsGoalDebugText(Agent* agent, int screenHeightMultiplier) const;
    void RenderAgentsResourcesDebugText(Agent* agent, int screenHeightMultiplier) const;

    void RenderAgentsCurrentLocation(Agent* agent, int screenHeightMultiplier) const;
    void RenderAgentsNeedsDebugText(Agent* agent, int screenHeightMultiplier) const;
    void RenderAgentsHealthAndLifeSpanDebugText(Agent* agent, int screenHeightMultiplier) const;
private:
    void RegisterGoalActionDefinition(ActionKey newActionKey, GoalAction &newGoalAction);
    LocationDataFromFile GetLocationData(int locationTypeKey);
    void ChangeTileToDirtTile(Tile* tile);
    void ChangeTileToWoodTile(Tile* tile);
    void AddResourcesToAgent(int numberOfResources, std::string typeOfResource, Agent* agent);
    int GetNumberOfTypeOfTile(std::string tileType);
    void SetupGoalActionDefinitions();

private:
    bool TestCallbackFunction(Agent* currentAgent);
    bool FindClosestTileForGoal(Agent* currentAgent);
    bool GeneratePathToRiver(Agent* currentAgent);
    bool AddWaterResources(Agent* currentAgent);
    bool GeneratePathToCrops(Agent* currentAgent);
    bool AddFoodResourcesFromCrops(Agent* currentAgent);
    bool GeneratePathToDesiredLocation(Agent* currentAgent);
    bool UseNecessaryResources(Agent* currentAgent);
    bool GetNecessaryResourcesShelter(Agent* currentAgent);
    bool GetNecessaryResourcesProtection(Agent* currentAgent);
    bool AddResourceFromWorldLocationShelter(Agent* currentAgent);
    bool CompleteHut(Agent* currentAgent);
    bool AddResourceFromWorldLocationProtection(Agent* currentAgent);
    bool MakeRope(Agent* currentAgent);
    bool FollowPath(Agent* currentAgent);
    bool MakeAxe(Agent* currentAgent);
    bool GoToHome(Agent* currentAgent);
    bool CompleteCommunityHousing(Agent* currentAgent);
    bool CompleteFoodStorage(Agent* currentAgent);
    bool CompleteWaterPipe(Agent* currentAgent);
    bool AddWaterResourcesFromPipe(Agent* currentAgent);
    bool ReproduceANewAgent(Agent* currentAgent);
    bool AttackAnimal(Agent* currentAgent);
    bool AddFoodResourcesFromAnimals(Agent* currentAgent);

    bool CompleteWell(Agent* currentAgent);
public:
    float GoalsResourcesCalculation(Agent* agent, Goal* goal);

private:
    bool MakeSword(Agent* currentAgent);
    bool MakeShield(Agent* currentAgent);
    bool CompleteFarm(Agent* currentAgent);
    bool CompleteRockMine(Agent* currentAgent);
    bool CompleteMetalMine(Agent* currentAgent);
    void SetupGoals();
};

static int s_currentAgentGlyph = 65;