#include "Game/World.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"

const int NUM_STARTING_AGENTS = 5;

//*******************************************************************
World::World()
    : m_map(nullptr)
{}

//*******************************************************************
World::World(std::string mapName)
    : m_currentTier(TIER_ONE)
    , m_currentTurn(0)
    , m_currentDay(0)
    , m_currentDayPopulation(0)
    , m_previousDayPopulation(0)
    , m_currentDayDeaths(0)
    , m_currentDayNewPopulants(0)
    , m_growthRate(0.0f)
    , m_deathRate(0.0f)
    , m_birthRate(0.0f)
    , m_populationsNeedForWater(0.0f)
    , m_populationsNeedForShelter(0.0f)
    , m_populationsNeedForProtection(0.0f)
    , m_populationsNeedForFood(0.0f)
    , m_populationsNeedForReproduction(0.0f)
    , m_maxNumberOfWaterTiles(0)
    , m_maxNumberOfWoodTiles(0)
    , m_maxNumberOfMetalTiles(0)
    , m_maxNumberOfRockTiles(0)
    , m_maxNumberOfFoodTiles(0)
    , m_maxNumberOfAnimalTiles(0)
    , m_numberOfIndividualGoalsDaily(0)
    , m_numberOfGroupGoalsDaily(0)
    , m_map(nullptr)
    , m_agents{}
    , m_resources{}
    , m_worldGoals{}
    , m_locations{}
    , m_animals{}
    , m_dayAndPopulationData{}
    , m_goalActions{}
    , m_dayAndIndividualGoalsChosen{}
    , m_dayAndGroupGoalsChosen{}
    , m_turnAndPopulationData{}
    , m_turnAndWaterGoalsData{}
    , m_turnAndFoodGoalsData{}
    , m_turnAndShelterGoalsData{}
    , m_turnAndProtectionGoalsData{}
    , m_turnAndReproductionGoalsData{}
    , m_turnAndGenericGoalsData{}
    , m_currentNumberOfResourcesPerTurn{}
    , m_agentsThatAreDead{}
    , m_agentsCauseOfDeath{}
{
    s_currentAgentGlyph = 65;

    m_map = new Map(mapName);
    SetupGoalActionDefinitions();
    SetupGoals();

    m_dayAndPopulationData[0] = 0;
    m_dayAndIndividualGoalsChosen[0] = 0;
    m_dayAndGroupGoalsChosen[0] = 0;

    m_turnAndPopulationData[0] = 0;
    m_turnAndWaterGoalsData[0] = 0;
    m_turnAndFoodGoalsData[0] = 0;
    m_turnAndShelterGoalsData[0] = 0;
    m_turnAndProtectionGoalsData[0] = 0;
    m_turnAndReproductionGoalsData[0] = 0;
    m_turnAndGenericGoalsData[0] = 0;

    m_turnAndIndividualGoalsChosen[0] = 0;
    m_turnAndGroupGoalsChosen[0] = 0;

    m_worldsDailyWaterNeed.push_back(0.0f);
    m_worldsDailyShelterNeed.push_back(0.0f);
    m_worldsDailyProtectionNeed.push_back(0.0f);
    m_worldsDailyFoodNeed.push_back(0.0f);
    m_worldsDailyReproductionNeed.push_back(0.0f);

    for (int testAgentIndex = 0; testAgentIndex < NUM_STARTING_AGENTS; testAgentIndex++)
    {
        AddRandomTestAgentToSimulation();
    }

    m_maxNumberOfWaterTiles = GetNumberOfTypeOfTile("water");
    m_maxNumberOfWoodTiles = GetNumberOfTypeOfTile("wood");
    m_maxNumberOfMetalTiles = GetNumberOfTypeOfTile("metal");
    m_maxNumberOfRockTiles = GetNumberOfTypeOfTile("rock");
    m_maxNumberOfFoodTiles = GetNumberOfTypeOfTile("food");
    m_maxNumberOfAnimalTiles = GetNumberOfTypeOfTile("animal");

    int totalNumberOfResources = m_maxNumberOfWaterTiles + m_maxNumberOfWoodTiles + m_maxNumberOfMetalTiles + m_maxNumberOfRockTiles + m_maxNumberOfFoodTiles + m_maxNumberOfAnimalTiles;
    m_currentNumberOfResourcesPerTurn.push_back(totalNumberOfResources);
}

//*******************************************************************
World::~World()
{
    for (Agent* currentAgent : m_agents)
    {
        if (currentAgent != nullptr)
        {
            delete currentAgent;
            currentAgent = nullptr;
        }
    }
    m_agents.clear();

    for (Resource* currentResource : m_resources)
    {
        if (currentResource != nullptr)
        {
            delete currentResource;
            currentResource = nullptr;
        }
    }
    m_resources.clear();

    for (Goal* currentGoal : m_worldGoals)
    {
        if (currentGoal != nullptr)
        {
            delete currentGoal;
            currentGoal = nullptr;
        }
    }
    m_worldGoals.clear();

    for (Location* currentLocation : m_locations)
    {
        if (currentLocation != nullptr)
        {
            delete currentLocation;
            currentLocation = nullptr;
        }
    }
    m_locations.clear();

    for (Animal* currentAnimal : m_animals)
    {
        if (currentAnimal != nullptr)
        {
            delete currentAnimal;
            currentAnimal = nullptr;
        }
    }
    m_animals.clear();

    if (m_map != nullptr)
    {
        delete m_map;
        m_map = nullptr;
    }
}

//*******************************************************************
void World::CalculateGrowthRate()
{
    if (m_currentDay <= 2)
        return;
    m_growthRate = ((m_currentDayPopulation - m_previousDayPopulation) / m_previousDayPopulation) * 100.0f;
}

//*******************************************************************
void World::CalculateDeathRate()
{
    if (m_currentDay <= 2)
        return;
    m_deathRate = ((float)m_currentDayDeaths / (float)m_currentDayPopulation);
}

//*******************************************************************
void World::CalculateBirthRate()
{
    if (m_currentDayPopulation == 0)
    {
        m_birthRate = 0.0f;
        return;
    }
    m_birthRate = ((float)m_currentDayNewPopulants / (float)m_currentDayPopulation);
}

//*******************************************************************
bool World::IsGrowthRateStagnant()
{
    //http://www.biology.ualberta.ca/courses.hp/bio208/Lecture_Notes_files/Biol208_Lecture19_PopulationGrowth.pdf

    if (m_currentDay == 0)
        return false;
    
    float rmax = m_birthRate - m_deathRate;
    float carryingCapacity = 25.0f;

    float populationRateOfChange = (rmax * m_currentDayPopulation) * (1.0f - (m_currentDayPopulation / carryingCapacity));

    //#TO_DO: Do test values to figure out what works best here
    if (populationRateOfChange == 0.0f)
        return true;

    return false;
}

//*******************************************************************
bool World::IsDeathRateOverComingGrowth()
{
    if (m_deathRate > m_growthRate)
        return true;
    return false;
}

//*******************************************************************
void World::CalculatePopulationsNeedForWater()
{
    float totalPopulationsNeedForWater = 0.0f;
    int agentsSize = (int)m_agents.size();
    for (int agentIndex = 0; agentIndex < agentsSize; agentIndex++)
    {
        Agent* currentAgent = m_agents[agentIndex];
        totalPopulationsNeedForWater += currentAgent->GetNeedForWater();
    }
    m_populationsNeedForWater = totalPopulationsNeedForWater / (float)agentsSize;
}

//*******************************************************************
void World::CalculatePopulationsNeedForShelter()
{
    float totalPopulationsNeedForShelter = 0.0f;
    int agentsSize = (int)m_agents.size();
    for (int agentIndex = 0; agentIndex < agentsSize; agentIndex++)
    {
        Agent* currentAgent = m_agents[agentIndex];
        totalPopulationsNeedForShelter += currentAgent->GetNeedForShelter();
    }
    m_populationsNeedForShelter = totalPopulationsNeedForShelter / (float)agentsSize;
}

//*******************************************************************
void World::CalculatePopulationsNeedForProtection()
{
    float totalPopulationsNeedForProtection = 0.0f;
    int agentsSize = (int)m_agents.size();
    for (int agentIndex = 0; agentIndex < agentsSize; agentIndex++)
    {
        Agent* currentAgent = m_agents[agentIndex];
        totalPopulationsNeedForProtection += currentAgent->GetNeedForProtection();
    }
    m_populationsNeedForProtection = totalPopulationsNeedForProtection / (float)agentsSize;
}

//*******************************************************************
void World::CalculatePopulationsNeedForFood()
{
    float totalPopulationsNeedForFood = 0.0f;
    int agentsSize = (int)m_agents.size();
    for (int agentIndex = 0; agentIndex < agentsSize; agentIndex++)
    {
        Agent* currentAgent = m_agents[agentIndex];
        totalPopulationsNeedForFood += currentAgent->GetNeedForFood();
    }
    float averagePopulationsNeedForFood = totalPopulationsNeedForFood / (float)agentsSize; //already normalized 0.0f to 1.0f

    m_populationsNeedForFood = SmoothStop4(averagePopulationsNeedForFood) * 0.75f;
}

//*******************************************************************
void World::CalculatePopulationsNeedForReproduction()
{
    int agentsSize = (int)m_agents.size();
    int numDays = (int)m_dayAndPopulationData.size();
    int largestPopulation = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_dayAndPopulationData[dayIndex] > largestPopulation)
            largestPopulation = m_dayAndPopulationData[dayIndex];
    }

    float fractionOfCurrentPopulation = (float)agentsSize / (float)NUM_STARTING_AGENTS;
    if (largestPopulation > 0)
        fractionOfCurrentPopulation = (float)agentsSize / (float)largestPopulation;
    if (agentsSize > largestPopulation)
        fractionOfCurrentPopulation = (float)agentsSize / (float)agentsSize;

    float currentPercentageOfPopulation = fractionOfCurrentPopulation * 100.0f;
    float maximumNeedForReproduction = 100.0f;
    float currentNeedForReproduction = maximumNeedForReproduction - currentPercentageOfPopulation;

    float reproductionFraction = currentNeedForReproduction / maximumNeedForReproduction;

    float utilityScoreForNeedForReproduction = reproductionFraction;
    utilityScoreForNeedForReproduction = SmoothStop4(utilityScoreForNeedForReproduction);  
 
    m_populationsNeedForReproduction = utilityScoreForNeedForReproduction;
}

//*******************************************************************
int World::GetCurrentTurn()
{
    return m_currentTurn;
}

//*******************************************************************
int World::GetCurrentDay()
{
    return m_currentDay;
}

//*******************************************************************
bool World::AgentDiedOfDisease()
{

    int numDays = (int)m_dayAndPopulationData.size();
    int largestPopulation = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_dayAndPopulationData[dayIndex] > largestPopulation)
            largestPopulation = m_dayAndPopulationData[dayIndex];
    }
    if (largestPopulation == 0)
        largestPopulation = NUM_STARTING_AGENTS;

    float chanceOfAgentDying = GetRandomFloatInRange(-1.0f, 1.0f);
    float maximumChanceToDieFraction = (float)m_agents.size() / (float)largestPopulation;
    float utilityOfMaximumChanceToDie = 1.0f - maximumChanceToDieFraction;

    if (chanceOfAgentDying >= utilityOfMaximumChanceToDie)
    {
        int agentToDie = GetRandomIntInRange(0, (int)m_agents.size() - 1);
        m_agentsThatAreDead.push_back(m_agents[agentToDie]->m_glyph);
        m_agentsCauseOfDeath.push_back("disease");
        RemoveAgentFromWorld(nullptr, m_agents[agentToDie]);
        return true;
    }
    return false;
}

//*******************************************************************
void World::UpdateTypesOfGoalsData()
{
    int currentNumberOfWaterGoals = 0;
    int currentNumberOfFoodGoals = 0;
    int currentNumberOfShelterGoals = 0;
    int currentNumberOfProtectionGoals = 0;
    int currentNumberOfReproductionGoals = 0;
    int currentNumberOfGenericGoals = 0;

    int currentNumberOfIndividualGoals = 0;
    int currentNumberOfGroupGoals = 0;

    for (int indexOfCurrentAgent = 0; indexOfCurrentAgent < (int)m_agents.size(); indexOfCurrentAgent++)
    {
        Agent* currentAgent = m_agents[indexOfCurrentAgent];
        Goal agentsCurrentGoal = currentAgent->GetCurrentGoal();
        std::string currentGoalType = agentsCurrentGoal.GetType();
        BuildType currentGoalBuildType = agentsCurrentGoal.GetGoalBuildType();

        if (currentGoalType.compare("water") == 0)
            currentNumberOfWaterGoals++;
        else if (currentGoalType.compare("food") == 0)
            currentNumberOfFoodGoals++;
        else if (currentGoalType.compare("shelter") == 0)
            currentNumberOfShelterGoals++;
        else if (currentGoalType.compare("protection") == 0)
            currentNumberOfProtectionGoals++;
        else if (currentGoalType.compare("reproduction") == 0)
            currentNumberOfReproductionGoals++;
        else if (currentGoalType.compare("generic") == 0)
            currentNumberOfGenericGoals++;

        if (currentGoalBuildType == INDIVIDUAL)
            currentNumberOfIndividualGoals++;
        else
            currentNumberOfGroupGoals++;
    }

    m_turnAndWaterGoalsData[m_currentTurn] = currentNumberOfWaterGoals;
    m_turnAndFoodGoalsData[m_currentTurn] = currentNumberOfFoodGoals;
    m_turnAndShelterGoalsData[m_currentTurn] = currentNumberOfShelterGoals;
    m_turnAndProtectionGoalsData[m_currentTurn] = currentNumberOfProtectionGoals;
    m_turnAndReproductionGoalsData[m_currentTurn] = currentNumberOfReproductionGoals;
    m_turnAndGenericGoalsData[m_currentTurn] = currentNumberOfGenericGoals;

    m_turnAndIndividualGoalsChosen[m_currentTurn] = currentNumberOfIndividualGoals;
    m_turnAndGroupGoalsChosen[m_currentTurn] = currentNumberOfGroupGoals;
}

//*******************************************************************
void World::UpdateAgents()
{
    for (int indexOfCurrentAgent = 0; indexOfCurrentAgent < (int)m_agents.size(); indexOfCurrentAgent++)
    {
        Agent* currentAgent = m_agents[indexOfCurrentAgent];

        currentAgent->CalculateNeeds();
        bool isANewGoal = currentAgent->SetNewGoal(m_worldGoals);

        if (isANewGoal)
        {
            if (currentAgent->m_currentGoal.GetGoalBuildType() == INDIVIDUAL)
                m_numberOfIndividualGoalsDaily++;
            else
                m_numberOfGroupGoalsDaily++;
        }

        currentAgent->Update();

        if ((currentAgent->m_lifeSpan <= 0) || (currentAgent->m_health <= 0.0f))
        {
            m_agentsThatAreDead.push_back(currentAgent->m_glyph);
            m_agentsCauseOfDeath.push_back(currentAgent->m_causeOfDeath);
            RemoveAgentFromWorld(nullptr, currentAgent);
        }
    }
}

//*******************************************************************
void World::UpdateLocations()
{
    for (int indexOfCurrentLocation = 0; indexOfCurrentLocation < (int)m_locations.size(); indexOfCurrentLocation++)
    {
        Location* currentLocation = m_locations[indexOfCurrentLocation];
        currentLocation->Update();
        currentLocation->ReplinishTiles(m_map);
        if (currentLocation->GetHealth() <= 0.0f)
            RemoveLocationFromWorld(nullptr, currentLocation);
    }
}

//*******************************************************************
void World::UpdateDay()
{
    m_currentDay++;
    m_currentDayPopulation = (int)m_agents.size();    
    m_previousDayPopulation = m_currentDayPopulation - m_currentDayNewPopulants + m_currentDayDeaths;

    m_dayAndPopulationData[m_currentDay] = m_currentDayPopulation;
    m_dayAndIndividualGoalsChosen[m_currentDay] = m_numberOfIndividualGoalsDaily;
    m_dayAndGroupGoalsChosen[m_currentDay] = m_numberOfGroupGoalsDaily;

    CalculateBirthRate();
    CalculateDeathRate();

    m_currentDayNewPopulants = 0;
    m_currentDayDeaths = 0;

    m_numberOfIndividualGoalsDaily = 0;
    m_numberOfGroupGoalsDaily = 0;

    CalculateWorldNeeds(); 
    float currentWorldWaterNeed = m_populationsNeedForWater;
    m_worldsDailyWaterNeed.push_back(currentWorldWaterNeed);
    float currentWorldShelterNeed = m_populationsNeedForShelter;
    m_worldsDailyShelterNeed.push_back(currentWorldShelterNeed);
    float currentWorldProtectionNeed = m_populationsNeedForProtection;
    m_worldsDailyProtectionNeed.push_back(currentWorldProtectionNeed);
    float currentWorldFoodNeed = m_populationsNeedForFood;
    m_worldsDailyFoodNeed.push_back(currentWorldFoodNeed);
    float currentWorldReproductionNeed = m_populationsNeedForReproduction;
    m_worldsDailyReproductionNeed.push_back(currentWorldReproductionNeed);


    for (int indexOfCurrentAgent = 0; indexOfCurrentAgent < (int)m_agents.size(); indexOfCurrentAgent++)
    {
        Agent* currentAgent = m_agents[indexOfCurrentAgent];

        currentAgent->CalculateNeeds();
        float currentWaterNeed = currentAgent->GetNeedForWater();
        currentAgent->m_dailyWaterNeed.push_back(currentWaterNeed);
        float currentShelterNeed = currentAgent->GetNeedForShelter();
        currentAgent->m_dailyShelterNeed.push_back(currentShelterNeed);
        float currentProtectionNeed = currentAgent->GetNeedForProtection();
        currentAgent->m_dailyProtectionNeed.push_back(currentProtectionNeed);
        float currentFoodNeed = currentAgent->GetNeedForFood();
        currentAgent->m_dailyFoodNeed.push_back(currentFoodNeed);
        float currentReproductionNeed = currentAgent->GetNeedToReproduce();
        currentAgent->m_dailyReproductionNeed.push_back(currentReproductionNeed);
    }
}

//*******************************************************************
void World::CalculateWorldNeeds()
{
    CalculatePopulationsNeedForWater();
    CalculatePopulationsNeedForShelter();
    CalculatePopulationsNeedForProtection();
    CalculatePopulationsNeedForFood();
    CalculatePopulationsNeedForReproduction();
}

//*******************************************************************
void World::UpdateResourcesPerTurnData()
{
    int currentWaterTiles = GetNumberOfTypeOfTile("water");
    int currentWoodTiles = GetNumberOfTypeOfTile("wood");
    int currentMetalTiles = GetNumberOfTypeOfTile("metal");
    int currentRockTiles = GetNumberOfTypeOfTile("rock");
    int currentFoodTiles = GetNumberOfTypeOfTile("food");
    int currentAnimalTiles = GetNumberOfTypeOfTile("animal");

    int totalNumberOfResources = currentWaterTiles + currentWoodTiles + currentMetalTiles + currentRockTiles + currentFoodTiles + currentAnimalTiles;
    m_currentNumberOfResourcesPerTurn.push_back(totalNumberOfResources);
}

//*******************************************************************
void World::UpdateWorld()
{
    CalculateGrowthRate();
    CalculateWorldNeeds();

    UpdateAgents();
    UpdateLocations();
    ReplinishWoodInWorld();
    UpdateResourcesPerTurnData();

    m_currentTurn++;
    m_turnAndPopulationData[m_currentTurn] = (int)m_agents.size();
    UpdateTypesOfGoalsData();

    if (m_currentTurn % 24 == 0)
    {
        //AgentDiedOfDisease();
        UpdateDay();
    }
}

//*******************************************************************
float World::GetGrowthRate()
{
    return m_growthRate;
}

//*******************************************************************
float World::GetDeathRate()
{
    return m_deathRate;
}

//*******************************************************************
void World::ChangeAgentsGoalToNewGoal(Goal* newGoal, Agent* agentToChange)
{
    UNUSED(newGoal);
    UNUSED(agentToChange);
}

//*******************************************************************
void World::AddNewAGentToWorld()
{

}

//*******************************************************************
bool World::RemoveLocationFromWorld(GoalAction* action, Location* locationToRemove)
{
    UNUSED(action);

    int locationIndex = 0;
    for (Location* currentLocation : m_locations)
    {
        if (currentLocation == locationToRemove)
            break;
        locationIndex++;
    }

    int locationWidth = locationToRemove->GetLocationSize().x;
    int locationHeight = locationToRemove->GetLocationSize().y;

    for (int xIndex = 0; xIndex < locationWidth; xIndex++)
    {
        for (int yIndex = 0; yIndex < locationHeight; yIndex++)
        {
            IntVector2 newLocationsWorldLocation = locationToRemove->GetWorldLocation();
            Tile* tile = m_map->GetTileAtTileCoords(IntVector2(newLocationsWorldLocation.x + xIndex, newLocationsWorldLocation.y + yIndex));
            ChangeTileToDirtTile(tile);
        }
    }

    m_locations.erase(m_locations.begin() + locationIndex);
    return true;
}

//*******************************************************************
bool World::RemoveAgentFromWorld(GoalAction* action, Agent* agentToRemove)
{
    UNUSED(action);

    int agentIndex = 0;
    for (Agent* currentAgent : m_agents)
    {
        if (currentAgent == agentToRemove)
            break;
        agentIndex++;
    }

    m_map->RemoveAIOccupantAtTileCoords(m_agents[agentIndex]->m_worldLocation);
    m_agents.erase(m_agents.begin() + agentIndex);
    m_currentDayDeaths++;
    CalculateDeathRate();
    return true;
}

//*******************************************************************
void World::AddRandomTestAgentToSimulation()
{
    Agent* agent = new Agent();
    agent->m_glyph = (char)s_currentAgentGlyph;
    s_currentAgentGlyph++;
    if (s_currentAgentGlyph > 90)
        s_currentAgentGlyph = 65;

    IntVector2 newStartingTileForAgent = FindRandomStartingDirtTileForAgent();
    agent->m_worldLocation = newStartingTileForAgent;
    agent->m_health = 100.0f;
    agent->m_lifeSpan = 7500;

    agent->AddResourceWeightToCurrentWeight(new Resource("water"));
    agent->AddResourceWeightToCurrentWeight(new Resource("water"));//5.0 lbs
    agent->AddResourceWeightToCurrentWeight(new Resource("food"));
    agent->AddResourceWeightToCurrentWeight(new Resource("food"));//5.0 lbs

    Tile* tile = m_map->GetTileAtTileCoords(agent->m_worldLocation);
    tile->m_aiOccupant = agent;
    m_map->m_tiles[m_map->GetTileIndexForTile(*tile)] = *tile;

    int numberOfDaysPassedInSim = m_currentDay + 1;
    for (int numberOfDaysIndex = 0; numberOfDaysIndex < numberOfDaysPassedInSim; numberOfDaysIndex++)
    {
        agent->m_dailyWaterNeed.push_back(0.0f);
        agent->m_dailyShelterNeed.push_back(0.0f);
        agent->m_dailyProtectionNeed.push_back(0.0f);
        agent->m_dailyFoodNeed.push_back(0.0f);
        agent->m_dailyReproductionNeed.push_back(0.0f);
    }

    agent->CalculateNeeds();

    m_agents.push_back(agent);
    m_currentDayNewPopulants++;
    CalculateBirthRate();
}

//*******************************************************************
IntVector2 World::FindRandomStartingDirtTileForAgent()
{
    int randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
    int randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
    Tile* randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));

    while (randomTile->m_tileDefinition->m_type.compare("dirt") != 0)
    {
        randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
        randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
        randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));
    }

    return IntVector2(randomXCoordinate, randomYCoordinate);
}

//*******************************************************************
IntVector2 World::FindRandomStartingDirtTile()
{
    int randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
    int randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
    Tile* randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));

    while (randomTile->m_tileDefinition->m_type.compare("dirt") != 0)
    {
        randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
        randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
        randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));
    }

    return IntVector2(randomXCoordinate, randomYCoordinate);
}

//*******************************************************************
IntVector2 World::FindRandomStartingWaterTileForAgent()
{
    int randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
    int randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
    Tile* randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));

    while (randomTile->m_tileDefinition->m_type.compare("water") != 0)
    {
        randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
        randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
        randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));
    }

    return IntVector2((5 - randomXCoordinate) + randomXCoordinate, randomYCoordinate);
}

//*******************************************************************
void World::ReplinishWoodInWorld()
{
    int currentWoodInWorld = GetNumberOfTypeOfTile("wood");
    int currentNumberOfDirtTiles = GetNumberOfTypeOfTile("dirt");
    int percentageOfDirtTilesToChange = (int)((float)currentNumberOfDirtTiles * 0.15f);

    int numberOfMissingWood = m_maxNumberOfWoodTiles - currentWoodInWorld;
    if(m_maxNumberOfWoodTiles < 25)
        numberOfMissingWood = percentageOfDirtTilesToChange - currentWoodInWorld;

    for (int numberOfWoodToPopulate = 0; numberOfWoodToPopulate < numberOfMissingWood; numberOfWoodToPopulate++)
        ChangeRandomDirtTileToWood();
}

//*******************************************************************
void World::ChangeRandomDirtTileToWood()
{
    int randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
    int randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
    Tile* randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));

    while (randomTile->m_tileDefinition->m_type.compare("dirt") != 0)
    {
        randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
        randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
        randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));
    }
    
    Tile* dirtTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));
    ChangeTileToWoodTile(dirtTile);
}

//*******************************************************************
IntVector2 World::FindRandomResourceTileForAgent(std::string resourceType)
{
    int randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
    int randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
    Tile* randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));

    while (randomTile->m_tileDefinition->m_type.compare(resourceType) != 0)
    {
        randomXCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.y - 1);
        randomYCoordinate = GetRandomIntInRange(0, m_map->m_dimensions.x - 1);
        randomTile = m_map->GetTileAtTileCoords(IntVector2(randomXCoordinate, randomYCoordinate));
    }

    return IntVector2(randomXCoordinate, randomYCoordinate);
}

//*******************************************************************
void World::Render() const
{
    m_map->Render();
    RenderWorldInformationDebug();
    RenderAgentsInformationDebug();
}

//*******************************************************************
void World::RenderWorldInformationDebug() const
{
    int textWidthLocation = 200;
    int textHeightLocation = (int)ORHTO_HEIGHT;
    float widthMultiplier = 1.0f;
    float textHeight = TILE_HEIGHT * 0.95f;
    float barWidth = TILE_WIDTH * 5.0f;



    std::string waterString = Stringf("World Water Utility: %4.2f", m_populationsNeedForWater);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation * (int)widthMultiplier, (int)textHeightLocation, (int)textHeight, waterString.c_str(), Rgba::WHITE);

    float colorBarWidth = (m_populationsNeedForWater / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + barWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::WHITE, Rgba::WHITE);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + colorBarWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::BLUE, Rgba::BLUE);
    widthMultiplier++;


    std::string foodString = Stringf("World Food Utility: %4.2f", m_populationsNeedForFood);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation * (int)widthMultiplier, (int)textHeightLocation, (int)textHeight, foodString.c_str(), Rgba::WHITE);

    colorBarWidth = (m_populationsNeedForFood / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + barWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::WHITE, Rgba::WHITE);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + colorBarWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::RED, Rgba::RED);
    widthMultiplier++;


    std::string shelterString = Stringf("World Shelter Utility: %4.2f", m_populationsNeedForShelter);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation * (int)widthMultiplier, (int)textHeightLocation, (int)textHeight, shelterString.c_str(), Rgba::WHITE);

    colorBarWidth = (m_populationsNeedForShelter / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + barWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::WHITE, Rgba::WHITE);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + colorBarWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::PURPLE, Rgba::PURPLE);
    widthMultiplier++;


    std::string protectionString = Stringf("World Protection Utility: %4.2f", m_populationsNeedForProtection);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation * (int)widthMultiplier, (int)textHeightLocation, (int)textHeight, protectionString.c_str(), Rgba::WHITE);

    colorBarWidth = (m_populationsNeedForProtection / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + barWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::WHITE, Rgba::WHITE);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + colorBarWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::GREEN, Rgba::GREEN);
    widthMultiplier++;


    std::string reproductionString = Stringf("World Reproduction Utility: %4.2f", m_populationsNeedForReproduction);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation * (int)widthMultiplier, (int)textHeightLocation, (int)textHeight, reproductionString.c_str(), Rgba::WHITE);

    colorBarWidth = (m_populationsNeedForReproduction / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + barWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::WHITE, Rgba::WHITE);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation * widthMultiplier, textHeightLocation - (textHeight * 1.5f), 0.0f), Vector3((textWidthLocation * widthMultiplier) + colorBarWidth, textHeightLocation - (textHeight * 1.5f), 0.0f), Rgba::ORANGE, Rgba::ORANGE);
}

//*******************************************************************
void World::RenderAgentsInformationDebug() const
{
    std::vector<Agent*> worldAgents = m_agents;

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_fontTexture);
    g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);

    for (int agentIndex = 0; agentIndex < MAX_NUM_AGENTS_IN_DEBUG_TEXT; agentIndex++)
    {
        if ((agentIndex + g_theGame->m_agentToDebugIndex) > ((int)m_agents.size() - 1))
            return;
        Agent* currentAgent = worldAgents[agentIndex + g_theGame->m_agentToDebugIndex];

        if (!g_theGame->m_doIRenderGoalScoresOfAgent)
            RenderInfoForOneAgentDebugText(currentAgent, agentIndex);
        else
            RenderGoalScoresForOneAgentDebugText(currentAgent, agentIndex);
    }
}

//*******************************************************************
void World::RenderInfoForOneAgentDebugText(Agent* currentAgent, int screenWidthMultiplier) const
{
    float heightAboveMap = SCREEN_HEIGHT * TILE_HEIGHT;
    float textWidthLocation = 1.0f + ((float)screenWidthMultiplier * (TILE_WIDTH * 22.0f));
    float textHeightLocation = TILE_HEIGHT * 0.95f;
    float textHeight = TILE_HEIGHT * 0.95f;
    float textTab = TILE_WIDTH * 2.0f;
    float heightMultiplier = 22.0f;
    float barWidth = TILE_WIDTH * 5.0f;
    Rgba needBeingFulfilledColor = Rgba::YELLOW;

    std::string agentNameString = Stringf("Name: %c", currentAgent->m_glyph);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, agentNameString.c_str(), Rgba::WHITE);
    heightMultiplier--;

    Goal currentGoal = currentAgent->GetCurrentGoal();
    std::string currentGoalName = currentGoal.GetGoalName();
    std::string goalNameString = Stringf("Current Goal: %s", currentGoalName.c_str());
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, goalNameString.c_str(), Rgba::WHITE);
    heightMultiplier--;

    std::string currentGoalActionString = currentGoal.GetCurrentGoalAction().GetCurrentGoalActionName();
    std::string actionNameString = Stringf("Current Action: %s", currentGoalActionString.c_str());
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + (int)textTab, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeightLocation, actionNameString.c_str(), Rgba::WHITE);
    heightMultiplier--;

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, "Current Needs: ", Rgba::WHITE);
    heightMultiplier--;



    Rgba waterUtilityColor = Rgba::WHITE;
    if (currentGoal.GetType().compare("water") == 0)
        waterUtilityColor = needBeingFulfilledColor;

    std::string waterString = Stringf("Water Need: %4.2f", currentAgent->GetNeedForWater());
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + (int)textTab, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, waterString.c_str(), waterUtilityColor);
    heightMultiplier--;

    float colorBarWidth = (currentAgent->GetNeedForWater() / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + barWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), waterUtilityColor, waterUtilityColor);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + colorBarWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Rgba::BLUE, Rgba::BLUE);
    heightMultiplier--;



    Rgba foodUtilityColor = Rgba::WHITE;
    if (currentGoal.GetType().compare("food") == 0)
        foodUtilityColor = needBeingFulfilledColor;

    std::string foodString = Stringf("Food Need: %4.2f", currentAgent->GetNeedForFood());
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + (int)textTab, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, foodString.c_str(), foodUtilityColor);
    heightMultiplier--;

    colorBarWidth = (currentAgent->GetNeedForFood() / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + barWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), foodUtilityColor, foodUtilityColor);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + colorBarWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Rgba::RED, Rgba::RED);
    heightMultiplier--;
    


    Rgba shelterUtilityColor = Rgba::WHITE;
    if (currentGoal.GetType().compare("shelter") == 0)
        shelterUtilityColor = needBeingFulfilledColor;

    std::string shelterString = Stringf("Shelter Need: %4.2f", currentAgent->GetNeedForShelter());
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + (int)textTab, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, shelterString.c_str(), shelterUtilityColor);
    heightMultiplier--;

    colorBarWidth = (currentAgent->GetNeedForShelter() / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + barWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), shelterUtilityColor, shelterUtilityColor);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + colorBarWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Rgba::PURPLE, Rgba::PURPLE);
    heightMultiplier--;



    Rgba protectionUtilityColor = Rgba::WHITE;
    if (currentGoal.GetType().compare("protection") == 0)
        protectionUtilityColor = needBeingFulfilledColor;

    std::string protectionString = Stringf("Protection Need: %4.2f", currentAgent->GetNeedForProtection());
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + (int)textTab, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, protectionString.c_str(), protectionUtilityColor);
    heightMultiplier--;

    colorBarWidth = (currentAgent->GetNeedForProtection() / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + barWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), protectionUtilityColor, protectionUtilityColor);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + colorBarWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Rgba::GREEN, Rgba::GREEN);
    heightMultiplier--;



    Rgba reproductionUtilityColor = Rgba::WHITE;
    if (currentGoal.GetType().compare("reproduction") == 0)
        reproductionUtilityColor = needBeingFulfilledColor;

    std::string reproductionString = Stringf("Reproduction Need: %4.2f", currentAgent->GetNeedToReproduce());
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + (int)textTab, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, reproductionString.c_str(), reproductionUtilityColor);
    heightMultiplier--;

    colorBarWidth = (currentAgent->GetNeedToReproduce() / 1.0f) * barWidth;
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + barWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), reproductionUtilityColor, reproductionUtilityColor);
    g_theSimpleRenderer->DrawLine2D(textHeight / 2.0f, Vector3(textWidthLocation + textTab, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Vector3(textWidthLocation + textTab + colorBarWidth, heightAboveMap + (textHeightLocation * (heightMultiplier - 0.5f)), 0.0f), Rgba::ORANGE, Rgba::ORANGE);
    heightMultiplier--;



    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, "Current Resources: ", Rgba::WHITE);
    heightMultiplier--;

    std::vector<Resource*> agentsResources = currentAgent->GetResources();
    int agentsResourcesSize = (int)agentsResources.size();
    for (int agentsResourceIndex = 0; agentsResourceIndex < agentsResourcesSize; agentsResourceIndex++)
    {
        std::string resourcesString = "";
        Resource* currentResource = agentsResources[agentsResourceIndex];
        std::string currentResourceName = currentResource->GetName();

        resourcesString += Stringf("%s, weight(%f), numUses(%i)", currentResourceName.c_str(), currentResource->GetWeight(), currentResource->GetNumberOfUses());
        g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + (int)textTab, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, resourcesString.c_str(), Rgba::WHITE);
        heightMultiplier--;
    }

    std::string healthString = Stringf("Current Health: %4.2f", currentAgent->m_health);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, healthString.c_str(), Rgba::WHITE);
    heightMultiplier--;

    std::string lifeSpanString = Stringf("Current LifeSpan: %i", currentAgent->m_lifeSpan);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, lifeSpanString.c_str(), Rgba::WHITE);
    heightMultiplier--;

    std::string worldLocationString = Stringf("Current WorldLocation: %i,%i", currentAgent->m_worldLocation.x, currentAgent->m_worldLocation.y);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, worldLocationString.c_str(), Rgba::WHITE);
    heightMultiplier--;

    g_theSimpleRenderer->DisableBlend();
}

//*******************************************************************
void World::RenderGoalScoresForOneAgentDebugText(Agent* currentAgent, int screenWidthMultiplier) const
{
    float heightAboveMap = SCREEN_HEIGHT * TILE_HEIGHT;
    float textWidthLocation = 1.0f + ((float)screenWidthMultiplier * (TILE_WIDTH * 22.0f));
    float textHeightLocation = TILE_HEIGHT * 0.95f;
    float textHeight = TILE_HEIGHT * 0.95f;
    float heightMultiplier = 22.0f;
    float textTab = TILE_WIDTH * 2.0f;
    Rgba goalSelectedColor = Rgba::YELLOW;

    std::string agentNameString = Stringf("Name: %c", currentAgent->m_glyph);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, agentNameString.c_str(), Rgba::WHITE);
    heightMultiplier--;

    std::string goalScoresString = Stringf("Current Goal Scores:");
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, goalScoresString.c_str(), Rgba::WHITE);
    heightMultiplier--;

    int currentGoalsScoreSize = currentAgent->m_currentGoalsScores.size();
    for (int currentGoalScoreIndex = 0; currentGoalScoreIndex < currentGoalsScoreSize; currentGoalScoreIndex++)
    {
        Rgba textColor = Rgba::WHITE;
        if (currentAgent->GetCurrentGoal().GetGoalName().compare(m_worldGoals[currentGoalScoreIndex]->GetGoalName()) == 0)
            textColor = goalSelectedColor;

        std::string goalNameAndScore = Stringf("(%4.2f) %s", currentAgent->m_currentGoalsScores[currentGoalScoreIndex], m_worldGoals[currentGoalScoreIndex]->GetGoalName().c_str());
        g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + (int)textTab, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, goalNameAndScore.c_str(), textColor);
        heightMultiplier--;
    }
}

//*******************************************************************
void World::RenderAgentsGoalDebugText(Agent* agent, int screenHeightMultiplier) const
{
    UNUSED(screenHeightMultiplier);

    Goal currentGoal = agent->GetCurrentGoal();
    std::string currentGoalName = currentGoal.GetGoalName();
    std::string currentGoalAction = currentGoal.GetCurrentGoalAction().GetCurrentGoalActionName();
    std::string currentGoalScore = "";
    if(currentGoal.GetGoalName().compare("none") != 0)
        currentGoalScore = std::to_string(currentGoal.m_goalCalculation(agent, &currentGoal));
    std::string agentGlyph = std::to_string(agent->m_glyph);

    int goalNeededResourcesSize = (int)currentGoal.m_neededResources.size();
    std::string resourcesString = "";
    for (int goalNeededResourcesIndex = 0; goalNeededResourcesIndex < goalNeededResourcesSize; goalNeededResourcesIndex++)
    {
        Resource currentResource = currentGoal.m_neededResources[goalNeededResourcesIndex];
        std::string currentResourceName = currentResource.GetName();
        std::string numberOfMinimumOfCurrentResourceString = std::to_string(currentGoal.m_minimumOfEachResource[goalNeededResourcesIndex]);

        resourcesString += Stringf("%s(%s)", currentResourceName.c_str(), numberOfMinimumOfCurrentResourceString.c_str());
        if (goalNeededResourcesIndex != (goalNeededResourcesIndex - 1))
            resourcesString += ", ";
    }

    std::string agentString = Stringf("%c :: CurrentGoal - %s [%s][%s] :: CurrentGoalAction - %s", agent->m_glyph, currentGoalName.c_str(), resourcesString.c_str(), currentGoalScore.c_str(), currentGoalAction.c_str());

    int textHeightLocation = (int)ORHTO_HEIGHT - ((int)TILE_HEIGHT * 0);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, textHeightLocation, (int)TILE_HEIGHT, agentString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void World::RenderAgentsResourcesDebugText(Agent* agent, int screenHeightMultiplier) const
{
    UNUSED(screenHeightMultiplier);

    std::vector<Resource*> agentsResources = agent->GetResources();
    int agentsResourcesSize = (int)agentsResources.size();
    std::string resourcesString = "";

    for (int agentsResourceIndex = 0; agentsResourceIndex < agentsResourcesSize; agentsResourceIndex++)
    {
        Resource* currentResource = agentsResources[agentsResourceIndex];
        std::string currentResourceName = currentResource->GetName();
        int numberOfUsesOfCurrentResource = currentResource->GetNumberOfUses();
        std::string numberOfUsesOfCurrentResourceString = std::to_string(numberOfUsesOfCurrentResource);

        resourcesString += Stringf("%s(%s)", currentResourceName.c_str(), numberOfUsesOfCurrentResourceString.c_str());
        if (agentsResourceIndex != (agentsResourcesSize - 1))
            resourcesString += ", ";
    }

    int textHeightLocation = (int)ORHTO_HEIGHT - ((int)TILE_HEIGHT * (0 + 1));
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, textHeightLocation, (int)TILE_HEIGHT, resourcesString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void World::RenderAgentsCurrentLocation(Agent* agent, int screenHeightMultiplier) const
{
    UNUSED(screenHeightMultiplier);

    std::string locationString = Stringf("Location : (%i,%i)", agent->m_worldLocation.x, agent->m_worldLocation.y);

    int textHeightLocation = (int)ORHTO_HEIGHT - ((int)TILE_HEIGHT * (0 + 2));
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, textHeightLocation, (int)TILE_HEIGHT, locationString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void World::RenderAgentsNeedsDebugText(Agent* agent, int screenHeightMultiplier) const
{
    UNUSED(screenHeightMultiplier);

    std::string needForWaterString = std::to_string(agent->GetNeedForWater());
    std::string needForShelterString = std::to_string(agent->GetNeedForShelter());
    std::string needForProtectionString = std::to_string(agent->GetNeedForProtection());
    std::string needForFoodString = std::to_string(agent->GetNeedForFood());

    std::string needsString = Stringf("Water(%s),Shelter(%s),Protection(%s),Food(%s)", needForWaterString.c_str(), needForShelterString.c_str(), needForProtectionString.c_str(), needForFoodString.c_str());

    int textHeightLocation = (int)ORHTO_HEIGHT - ((int)TILE_HEIGHT * (0 + 3));
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, textHeightLocation, (int)TILE_HEIGHT, needsString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void World::RenderAgentsHealthAndLifeSpanDebugText(Agent* agent, int screenHeightMultiplier) const
{
    UNUSED(screenHeightMultiplier);

    std::string healthString = std::to_string(agent->m_health);
    std::string lifeSpanString = std::to_string(agent->m_lifeSpan);

    std::string healthAndLifeSpanString = Stringf("Health(%s),LifeSpan(%s)", healthString.c_str(), lifeSpanString.c_str());

    int textHeightLocation = (int)ORHTO_HEIGHT - ((int)TILE_HEIGHT * (0 + 4));
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, textHeightLocation, (int)TILE_HEIGHT, healthAndLifeSpanString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void World::RegisterGoalActionDefinition(ActionKey newActionKey, GoalAction &newGoalAction)
{
    UNUSED(newActionKey);
    GoalAction* goalAction = new GoalAction();
    goalAction->m_actionKey = newGoalAction.m_actionKey;
    goalAction->m_functionCompleteAction = newGoalAction.m_functionCompleteAction;
    m_goalActions.push_back(goalAction);
}

//*******************************************************************
LocationDataFromFile World::GetLocationData(int locationTypeKey)
{
    std::vector<LocationDataFromFile> locationsData = m_map->m_mapDefinition->m_generators[0]->m_locationsData;
    return locationsData[locationTypeKey];
}

//*******************************************************************
void World::ChangeTileToDirtTile(Tile* tile)
{
    tile->ChangeType("dirt");
    tile->m_tileCoords = IntVector2(tile->m_tileCoords.x, tile->m_tileCoords.y);
    tile->m_permanence = 1.0f;
    int tileIndex = m_map->GetTileIndexForTile(*tile);
    m_map->m_tiles[tileIndex] = *tile;
}

//*******************************************************************
void World::ChangeTileToWoodTile(Tile* tile)
{
    tile->ChangeType("wood");
    tile->m_tileCoords = IntVector2(tile->m_tileCoords.x, tile->m_tileCoords.y);
    tile->m_permanence = 1.0f;
    int tileIndex = m_map->GetTileIndexForTile(*tile);
    m_map->m_tiles[tileIndex] = *tile;
}

//*******************************************************************
void World::AddResourcesToAgent(int numberOfResources, std::string typeOfResource, Agent* agent)
{
    for (int resourceIndex = 0; resourceIndex < numberOfResources; resourceIndex++)
    {
        agent->AddResourceWeightToCurrentWeight(new Resource(typeOfResource));
    }
}

//*******************************************************************
int World::GetNumberOfTypeOfTile(std::string tileType)
{
    int numberOfTypeOfTile = 0;
    int numberOfTiles = (int)m_map->m_tiles.size();
    for (int tileIndex = 0; tileIndex < numberOfTiles; tileIndex++)
    {
        Tile* tile = m_map->GetTileForTileIndex(tileIndex);
        if (tile->m_tileDefinition->m_type.compare(tileType) == 0)
            numberOfTypeOfTile++;
    }
    return numberOfTypeOfTile;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//*******************************************************************
void World::SetupGoalActionDefinitions()
{
    GoalAction* goalAction = new GoalAction();
    goalAction->m_actionKey = ACTION_KEY_TEST;
    goalAction->m_functionCompleteAction = std::bind(&World::TestCallbackFunction, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_TEST, *goalAction);

    GoalAction* goalActionGeneralOne = new GoalAction();
    goalActionGeneralOne->m_actionKey = ACTION_KEY_GENERAL_ONE;
    goalActionGeneralOne->m_functionCompleteAction = std::bind(&World::FindClosestTileForGoal, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_GENERAL_ONE, *goalActionGeneralOne);

    GoalAction* goalActionWaterOne = new GoalAction();
    goalActionWaterOne->m_actionKey = ACTION_KEY_WATER_ONE;
    goalActionWaterOne->m_functionCompleteAction = std::bind(&World::GeneratePathToRiver, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_WATER_ONE, *goalActionWaterOne);

    GoalAction* goalActionWaterTwo = new GoalAction();
    goalActionWaterTwo->m_actionKey = ACTION_KEY_WATER_TWO;
    goalActionWaterTwo->m_functionCompleteAction = std::bind(&World::FollowPath, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_WATER_TWO, *goalActionWaterTwo);

    GoalAction* goalActionWaterThree = new GoalAction();
    goalActionWaterThree->m_actionKey = ACTION_KEY_WATER_THREE;
    goalActionWaterThree->m_functionCompleteAction = std::bind(&World::AddWaterResources, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_WATER_THREE, *goalActionWaterThree);

    GoalAction* goalActionFoodOne = new GoalAction();
    goalActionFoodOne->m_actionKey = ACTION_KEY_FOOD_ONE;
    goalActionFoodOne->m_functionCompleteAction = std::bind(&World::GeneratePathToCrops, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_FOOD_ONE, *goalActionFoodOne);

    GoalAction* goalActionFoodTwo = new GoalAction();
    goalActionFoodTwo->m_actionKey = ACTION_KEY_FOOD_TWO;
    goalActionFoodTwo->m_functionCompleteAction = std::bind(&World::FollowPath, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_FOOD_TWO, *goalActionFoodTwo);

    GoalAction* goalActionFoodThree = new GoalAction();
    goalActionFoodThree->m_actionKey = ACTION_KEY_FOOD_THREE;
    goalActionFoodThree->m_functionCompleteAction = std::bind(&World::AddFoodResourcesFromCrops, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_FOOD_THREE, *goalActionFoodThree);

    GoalAction* goalActionShelterOne = new GoalAction();
    goalActionShelterOne->m_actionKey = ACTION_KEY_SHELTER_ONE;
    goalActionShelterOne->m_functionCompleteAction = std::bind(&World::GeneratePathToDesiredLocation, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_FOOD_THREE, *goalActionShelterOne);

    GoalAction* goalActionShelterTwo = new GoalAction();
    goalActionShelterTwo->m_actionKey = ACTION_KEY_SHELTER_TWO;
    goalActionShelterTwo->m_functionCompleteAction = std::bind(&World::FollowPath, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_SHELTER_TWO, *goalActionShelterTwo);

    GoalAction* goalActionShelterThree = new GoalAction();
    goalActionShelterThree->m_actionKey = ACTION_KEY_SHELTER_THREE;
    goalActionShelterThree->m_functionCompleteAction = std::bind(&World::UseNecessaryResources, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_SHELTER_THREE, *goalActionShelterThree);

    GoalAction* goalActionShelterFour = new GoalAction();
    goalActionShelterFour->m_actionKey = ACTION_KEY_SHELTER_FOUR;
    goalActionShelterFour->m_functionCompleteAction = std::bind(&World::GetNecessaryResourcesShelter, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_SHELTER_FOUR, *goalActionShelterFour);

    GoalAction* goalActionShelterFive = new GoalAction();
    goalActionShelterFive->m_actionKey = ACTION_KEY_SHELTER_FIVE;
    goalActionShelterFive->m_functionCompleteAction = std::bind(&World::AddResourceFromWorldLocationShelter, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_SHELTER_FIVE, *goalActionShelterFive);

    GoalAction* goalActionShelterSix = new GoalAction();
    goalActionShelterSix->m_actionKey = ACTION_KEY_SHELTER_SIX;
    goalActionShelterSix->m_functionCompleteAction = std::bind(&World::CompleteHut, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_SHELTER_SIX, *goalActionShelterSix);

    GoalAction* goalActionProtectionOne = new GoalAction();
    goalActionProtectionOne->m_actionKey = ACTION_KEY_PROTECTION_ONE;
    goalActionProtectionOne->m_functionCompleteAction = std::bind(&World::UseNecessaryResources, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_PROTECTION_ONE, *goalActionProtectionOne);

    GoalAction* goalActionProtectionTwo = new GoalAction();
    goalActionProtectionTwo->m_actionKey = ACTION_KEY_PROTECTION_TWO;
    goalActionProtectionTwo->m_functionCompleteAction = std::bind(&World::GetNecessaryResourcesProtection, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_PROTECTION_TWO, *goalActionProtectionTwo);

    GoalAction* goalActionProtectionThree = new GoalAction();
    goalActionProtectionThree->m_actionKey = ACTION_KEY_PROTECTION_THREE;
    goalActionProtectionThree->m_functionCompleteAction = std::bind(&World::AddResourceFromWorldLocationProtection, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_PROTECTION_THREE, *goalActionProtectionThree);

    GoalAction* goalActionProtectionFour = new GoalAction();
    goalActionProtectionFour->m_actionKey = ACTION_KEY_PROTECTION_FOUR;
    goalActionProtectionFour->m_functionCompleteAction = std::bind(&World::MakeRope, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_PROTECTION_FOUR, *goalActionProtectionFour);

    GoalAction* goalActionProtectionFive = new GoalAction();
    goalActionProtectionFive->m_actionKey = ACTION_KEY_PROTECTION_FIVE;
    goalActionProtectionFive->m_functionCompleteAction = std::bind(&World::MakeAxe, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_PROTECTION_FIVE, *goalActionProtectionFive);

    GoalAction* goalActionShelterSeven = new GoalAction();
    goalActionShelterSeven->m_actionKey = ACTION_KEY_SHELTER_SEVEN;
    goalActionShelterSeven->m_functionCompleteAction = std::bind(&World::GoToHome, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_SHELTER_SEVEN, *goalActionShelterSeven);

    GoalAction* goalActionShelterEight = new GoalAction();
    goalActionShelterEight->m_actionKey = ACTION_KEY_SHELTER_EIGHT;
    goalActionShelterEight->m_functionCompleteAction = std::bind(&World::CompleteCommunityHousing, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_SHELTER_EIGHT, *goalActionShelterEight);

    GoalAction* goalActionShelterNine = new GoalAction();
    goalActionShelterNine->m_actionKey = ACTION_KEY_SHELTER_NINE;
    goalActionShelterNine->m_functionCompleteAction = std::bind(&World::CompleteFoodStorage, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_SHELTER_NINE, *goalActionShelterNine);

    GoalAction* goalActionShelterTen = new GoalAction();
    goalActionShelterTen->m_actionKey = ACTION_KEY_SHELTER_TEN;
    goalActionShelterTen->m_functionCompleteAction = std::bind(&World::CompleteWaterPipe, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_SHELTER_TEN, *goalActionShelterTen);

    GoalAction* goalActionWaterFour = new GoalAction();
    goalActionWaterFour->m_actionKey = ACTION_KEY_WATER_FOUR;
    goalActionWaterFour->m_functionCompleteAction = std::bind(&World::AddWaterResourcesFromPipe, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_WATER_FOUR, *goalActionWaterFour);

    GoalAction* goalActionReproduceOne = new GoalAction();
    goalActionReproduceOne->m_actionKey = ACTION_kEY_REPRODUCE_ONE;
    goalActionReproduceOne->m_functionCompleteAction = std::bind(&World::ReproduceANewAgent, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_kEY_REPRODUCE_ONE, *goalActionReproduceOne);

    GoalAction* goalActionFoodFour = new GoalAction();
    goalActionFoodFour->m_actionKey = ACTION_KEY_FOOD_FOUR;
    goalActionFoodFour->m_functionCompleteAction = std::bind(&World::AttackAnimal, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_FOOD_FOUR, *goalActionFoodFour);

    GoalAction* goalActionFoodFive = new GoalAction();
    goalActionFoodFive->m_actionKey = ACTION_KEY_FOOD_FIVE;
    goalActionFoodFive->m_functionCompleteAction = std::bind(&World::AddFoodResourcesFromAnimals, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_FOOD_FIVE, *goalActionFoodFive);

    GoalAction* goalActionFoodSix = new GoalAction();
    goalActionFoodSix->m_actionKey = ACTION_KEY_FOOD_SIX;
    goalActionFoodSix->m_functionCompleteAction = std::bind(&World::FollowPath, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_FOOD_SIX, *goalActionFoodSix);

    GoalAction* goalActionWaterFive = new GoalAction();
    goalActionWaterFive->m_actionKey = ACTION_KEY_WATER_FIVE;
    goalActionWaterFive->m_functionCompleteAction = std::bind(&World::CompleteWell, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_WATER_FIVE, *goalActionWaterFive);

    GoalAction* goalActionProtectionSix = new GoalAction();
    goalActionProtectionSix->m_actionKey = ACTION_KEY_PROTECTION_SIX;
    goalActionProtectionSix->m_functionCompleteAction = std::bind(&World::MakeSword, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_PROTECTION_SIX, *goalActionProtectionSix);

    GoalAction* goalActionProtectionSeven = new GoalAction();
    goalActionProtectionSeven->m_actionKey = ACTION_KEY_PROTECTION_SEVEN;
    goalActionProtectionSeven->m_functionCompleteAction = std::bind(&World::MakeShield, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_PROTECTION_SEVEN, *goalActionProtectionSeven);

    GoalAction* goalActionFoodSeven = new GoalAction();
    goalActionFoodSeven->m_actionKey = ACTION_KEY_FOOD_SEVEN;
    goalActionFoodSeven->m_functionCompleteAction = std::bind(&World::CompleteFarm, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_FOOD_SEVEN, *goalActionFoodSeven);

    GoalAction* goalActionGeneralTwo = new GoalAction();
    goalActionGeneralTwo->m_actionKey = ACTION_KEY_GENERAL_TWO;
    goalActionGeneralTwo->m_functionCompleteAction = std::bind(&World::CompleteRockMine, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_GENERAL_TWO, *goalActionGeneralTwo);

    GoalAction* goalActionGeneralThree = new GoalAction();
    goalActionGeneralThree->m_actionKey = ACTION_KEY_GENERAL_THREE;
    goalActionGeneralThree->m_functionCompleteAction = std::bind(&World::CompleteMetalMine, this, std::placeholders::_1);
    RegisterGoalActionDefinition(ACTION_KEY_GENERAL_THREE, *goalActionGeneralThree);
}

//*******************************************************************
bool World::TestCallbackFunction(Agent* currentAgent)
{
    UNUSED(currentAgent);
    return false;
}

//*******************************************************************
bool World::FindClosestTileForGoal(Agent* currentAgent)
{
    std::vector<Tile>& allTilesInCurrentMap = m_map->m_tiles;
    IntVector2 startTileCoords = currentAgent->m_worldLocation;
    Vector2 startPosition = m_map->GetWorldCenterForTileCoordinates(startTileCoords);

    bool foundACloseByTile = false;
    float shortestDistanceToTile = WINDOW_WIDTH * WINDOW_HEIGHT;
    for (int allTilesInCurrentMapIndex = 0; allTilesInCurrentMapIndex < (int)allTilesInCurrentMap.size(); allTilesInCurrentMapIndex++)
    {
        Tile& tile = allTilesInCurrentMap[allTilesInCurrentMapIndex];

        if (tile.m_tileDefinition->m_type.compare(currentAgent->GetCurrentGoal().m_desiredLocationTileType) == 0)
        {
            foundACloseByTile = true;
            Vector2 endPosition = m_map->GetWorldCenterForTileCoordinates(tile.m_tileCoords);
            RayCastResult2D result = m_map->AmanatidesWooRaycast(startPosition, endPosition);

            float distanceFromStartToEnd = (float)m_map->CalculateManhattanDistance(startTileCoords, tile.m_tileCoords);
            if (distanceFromStartToEnd < shortestDistanceToTile)
            {
                shortestDistanceToTile = distanceFromStartToEnd;
                if (tile.m_tileDefinition->m_isSolid)
                {
                    Tile* tileToTheWest = m_map->GetTileAtTileCoords(tile.m_tileCoords + IntVector2(-1, 0));
                    Tile* tileToTheEast = m_map->GetTileAtTileCoords(tile.m_tileCoords + IntVector2(1, 0));
                    Tile* tileToTheSouth = m_map->GetTileAtTileCoords(tile.m_tileCoords + IntVector2(0, -1));
                    Tile* tileToTheNorth = m_map->GetTileAtTileCoords(tile.m_tileCoords + IntVector2(0, 1));

                    if (tileToTheEast != nullptr && tileToTheEast->m_tileDefinition->m_isSolid == false)
                    {
                        currentAgent->m_currentGoal.m_desiredLocation = tileToTheEast->m_tileCoords;
                        continue;
                    }
                    if (tileToTheWest != nullptr && tileToTheWest->m_tileDefinition->m_isSolid == false)
                    {
                        currentAgent->m_currentGoal.m_desiredLocation = tileToTheWest->m_tileCoords;
                        continue;
                    }
                    if (tileToTheNorth != nullptr && tileToTheNorth->m_tileDefinition->m_isSolid == false)
                    {
                        currentAgent->m_currentGoal.m_desiredLocation = tileToTheNorth->m_tileCoords;
                        continue;
                    }
                    if (tileToTheSouth != nullptr && tileToTheSouth->m_tileDefinition->m_isSolid == false)
                    {
                        currentAgent->m_currentGoal.m_desiredLocation = tileToTheSouth->m_tileCoords;
                        continue;
                    }
                }
                else
                    currentAgent->m_currentGoal.m_desiredLocation = tile.m_tileCoords;
            }
        }        
    }

    IntVector2 tileCoords = currentAgent->GetCurrentGoal().m_desiredLocation;
    currentAgent->m_currentPath = m_map->GeneratePath(currentAgent->m_worldLocation, tileCoords, nullptr);
    return foundACloseByTile;
}

//*******************************************************************
bool World::GeneratePathToRiver(Agent* currentAgent)
{
    bool foundWaterTile = FindClosestTileForGoal(currentAgent);
    if (!foundWaterTile)
        return false;
    IntVector2 riverTile = currentAgent->GetCurrentGoal().m_desiredLocation;
    currentAgent->m_currentPath = m_map->GeneratePath(currentAgent->m_worldLocation, riverTile, nullptr);
    return true;
}

//*******************************************************************
bool World::FollowPath(Agent* currentAgent)
{
    if ((int)currentAgent->m_currentPath.size() == 0)
    {
        return true;
    }
    if ((int)currentAgent->m_currentPath.size() == 1)
    {
        m_map->RemoveAIOccupantAtTileCoords(currentAgent->m_worldLocation);
        m_map->AddAIOccupantAtTileCoords(currentAgent->m_currentPath[0]->m_tileCoords, currentAgent);
        currentAgent->m_currentPath.erase(currentAgent->m_currentPath.begin() + 0);
        return true;
    }
    m_map->RemoveAIOccupantAtTileCoords(currentAgent->m_worldLocation);
    m_map->AddAIOccupantAtTileCoords(currentAgent->m_currentPath[currentAgent->m_currentPath.size() - 1]->m_tileCoords, currentAgent);
    currentAgent->m_currentPath.erase(currentAgent->m_currentPath.begin() + (currentAgent->m_currentPath.size() - 1));
    return false;
}

//*******************************************************************
bool World::AddWaterResources(Agent* currentAgent)
{
    Tile* tileToTheWest = m_map->GetTileAtTileCoords(currentAgent->m_worldLocation + IntVector2(-1, 0));
    Tile* tileToTheEast = m_map->GetTileAtTileCoords(currentAgent->m_worldLocation + IntVector2(1, 0));
    Tile* tileToTheSouth = m_map->GetTileAtTileCoords(currentAgent->m_worldLocation + IntVector2(0, -1));
    Tile* tileToTheNorth = m_map->GetTileAtTileCoords(currentAgent->m_worldLocation + IntVector2(0, 1));
    
    if ((tileToTheEast != nullptr && (tileToTheEast->m_tileDefinition->m_type.compare("water") != 0)) && (tileToTheWest != nullptr && (tileToTheWest->m_tileDefinition->m_type.compare("water") != 0)) && (tileToTheNorth != nullptr && (tileToTheNorth->m_tileDefinition->m_type.compare("water") != 0)) && (tileToTheSouth != nullptr && (tileToTheSouth->m_tileDefinition->m_type.compare("water") != 0)))
    {
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_WATER_ONE]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_WATER_TWO]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_WATER_THREE]);
        return true;
    }

    int numWatersToAdd = 2;
    if (tileToTheEast != nullptr && (tileToTheEast->m_tileDefinition->m_type.compare("water") == 0))
    {
        AddResourcesToAgent(numWatersToAdd, "water", currentAgent);
        ChangeTileToDirtTile(tileToTheEast);
        return true;
    }
    if (tileToTheWest != nullptr && (tileToTheWest->m_tileDefinition->m_type.compare("water") == 0))
    {
        AddResourcesToAgent(numWatersToAdd, "water", currentAgent);
        ChangeTileToDirtTile(tileToTheWest);
        return true;
    }
    if (tileToTheNorth != nullptr && (tileToTheNorth->m_tileDefinition->m_type.compare("water") == 0))
    {
        AddResourcesToAgent(numWatersToAdd, "water", currentAgent);
        ChangeTileToDirtTile(tileToTheNorth);
        return true;
    }
    if (tileToTheSouth != nullptr && (tileToTheSouth->m_tileDefinition->m_type.compare("water") == 0))
    {
        AddResourcesToAgent(numWatersToAdd, "water", currentAgent);
        ChangeTileToDirtTile(tileToTheSouth);
        return true;
    }
   
    return true;
}

//*******************************************************************
bool World::GeneratePathToCrops(Agent* currentAgent)
{
    FindClosestTileForGoal(currentAgent);
    IntVector2 cropTile = currentAgent->GetCurrentGoal().m_desiredLocation;
    currentAgent->m_currentPath = m_map->GeneratePath(currentAgent->m_worldLocation, cropTile, nullptr);
    return true;
}

//*******************************************************************
bool World::AddFoodResourcesFromCrops(Agent* currentAgent)
{
    Tile* currentAgentTile = m_map->GetTileAtTileCoords(currentAgent->m_worldLocation);

    if (currentAgentTile != nullptr && (currentAgentTile->m_tileDefinition->m_type.compare("food") != 0))
    {
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_FOOD_ONE]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_FOOD_TWO]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_FOOD_THREE]);
        return true;
    }

    AddResourcesToAgent(2, "food", currentAgent);
    ChangeTileToDirtTile(currentAgentTile);   
    return true;
}

//*******************************************************************
bool World::GeneratePathToDesiredLocation(Agent* currentAgent)
{
    currentAgent->m_currentPath = m_map->GeneratePath(currentAgent->m_worldLocation, currentAgent->m_currentGoal.m_desiredLocation, nullptr);
    return true;
}

//*******************************************************************
bool World::UseNecessaryResources(Agent* currentAgent)
{
    for (int neededResourcesIndex = 0; neededResourcesIndex < (int)currentAgent->m_currentGoal.m_neededResources.size(); neededResourcesIndex++)
    {
        Resource currentNeededResource = currentAgent->m_currentGoal.m_neededResources[neededResourcesIndex];
        int numberOfCurrentNeededResource = (int)currentAgent->m_currentGoal.m_minimumOfEachResource[neededResourcesIndex];

        if (numberOfCurrentNeededResource == 0)
            continue;

        int resourcesRemoved = 0;
        std::vector<Resource*> currentAgentsResources = currentAgent->GetResources();
        for (Resource* currentResource : currentAgentsResources)
        {
            if (currentResource->GetName().compare(currentNeededResource.GetName()) == 0)
            {
                resourcesRemoved++;
                currentAgent->UseResource(currentResource);
                if (resourcesRemoved == currentAgent->m_currentGoal.m_minimumOfEachResource[neededResourcesIndex])
                    break;
            }
        }

        if (currentAgent->m_currentGoal.m_buildType == GROUP)
        {
            for (int worldGoalIndex = 0; worldGoalIndex < (int)m_worldGoals.size(); worldGoalIndex++)
            {
                if (m_worldGoals[worldGoalIndex]->GetGoalName().compare(currentAgent->m_currentGoal.GetGoalName()) == 0)
                {
                    m_worldGoals[worldGoalIndex]->m_minimumOfEachResource[neededResourcesIndex] = m_worldGoals[worldGoalIndex]->m_minimumOfEachResource[neededResourcesIndex] - resourcesRemoved;
                    currentAgent->m_currentGoal.m_minimumOfEachResource[neededResourcesIndex] = m_worldGoals[worldGoalIndex]->m_minimumOfEachResource[neededResourcesIndex];
                }
            }
        }
        else
        {
            currentAgent->m_currentGoal.m_minimumOfEachResource[neededResourcesIndex] = currentAgent->m_currentGoal.m_minimumOfEachResource[neededResourcesIndex] - resourcesRemoved;
        }

    }
    return true;
}

//*******************************************************************
bool World::GetNecessaryResourcesShelter(Agent* currentAgent)
{
    for (int neededResourcesIndex = 0; neededResourcesIndex < (int)currentAgent->m_currentGoal.m_neededResources.size(); neededResourcesIndex++)
    {
        Resource currentNeededResource = currentAgent->m_currentGoal.m_neededResources[neededResourcesIndex];
        int numberOfCurrentNeededResource = (int)currentAgent->m_currentGoal.m_minimumOfEachResource[neededResourcesIndex];
        if (numberOfCurrentNeededResource != 0)
        {
            currentAgent->m_currentGoal.m_desiredLocationTileType = currentNeededResource.GetName();
            FindClosestTileForGoal(currentAgent);
            currentAgent->m_currentPath = m_map->GeneratePath(currentAgent->m_worldLocation, currentAgent->m_currentGoal.m_desiredLocation, nullptr);
            currentAgent->m_currentPath.erase(currentAgent->m_currentPath.begin() + (currentAgent->m_currentPath.size() - 1));
            bool doesAgentContinuePath = FollowPath(currentAgent);
            if (!doesAgentContinuePath)
                return false;
            currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()) - 1, *m_goalActions[ACTION_KEY_SHELTER_FIVE]);
            return true;
        }
    }

    return true;
}

//*******************************************************************
bool World::GetNecessaryResourcesProtection(Agent* currentAgent)
{
    for (int neededResourcesIndex = 0; neededResourcesIndex < (int)currentAgent->m_currentGoal.m_neededResources.size(); neededResourcesIndex++)
    {
        Resource currentNeededResource = currentAgent->m_currentGoal.m_neededResources[neededResourcesIndex];
        int numberOfCurrentNeededResource = (int)currentAgent->m_currentGoal.m_minimumOfEachResource[neededResourcesIndex];
        if (numberOfCurrentNeededResource != 0)
        {
            currentAgent->m_currentGoal.m_desiredLocationTileType = currentNeededResource.GetName();
            FindClosestTileForGoal(currentAgent);
            currentAgent->m_currentPath = m_map->GeneratePath(currentAgent->m_worldLocation, currentAgent->m_currentGoal.m_desiredLocation, nullptr);
            currentAgent->m_currentPath.erase(currentAgent->m_currentPath.begin() + (currentAgent->m_currentPath.size() - 1));
            bool doesAgentContinuePath = FollowPath(currentAgent);
            if (!doesAgentContinuePath)
                return false;
            currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()) - 1, *m_goalActions[ACTION_KEY_PROTECTION_THREE]);
            return true;
        }
    }

    return true;
}

//*******************************************************************
bool World::AddResourceFromWorldLocationShelter(Agent* currentAgent)
{
    Tile* currentTile = m_map->GetTileAtTileCoords(currentAgent->m_worldLocation);
    std::string desiredTileType = currentAgent->m_currentGoal.m_desiredLocationTileType;

    if (currentTile != nullptr && (currentTile->m_tileDefinition->m_type.compare(desiredTileType) == 0))
    {
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_SHELTER_ONE]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_SHELTER_TWO]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_SHELTER_FIVE]);
        return true;
    }

    AddResourcesToAgent(2, currentTile->m_tileDefinition->m_type, currentAgent);
    ChangeTileToDirtTile(currentTile);

    currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size() - 1), *m_goalActions[ACTION_KEY_SHELTER_ONE]);
    currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size() - 1), *m_goalActions[ACTION_KEY_SHELTER_TWO]);
    currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size() - 1), *m_goalActions[ACTION_KEY_SHELTER_THREE]);
    currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size() - 1), *m_goalActions[ACTION_KEY_SHELTER_FOUR]);
    return true;
}

//*******************************************************************
bool World::CompleteHut(Agent* currentAgent)
{
    UNUSED(currentAgent);

    LocationDataFromFile hutData = GetLocationData((int)LOCATION_HUT);
    Location* newLocation = new Location(LOCATION_HUT, hutData.m_worldLocation, hutData.m_size);
    newLocation->SetHealth(hutData.m_health);

    int locationWidth = newLocation->GetLocationSize().x;
    int locationHeight = newLocation->GetLocationSize().y;

    for (int xIndex = 0; xIndex < locationWidth; xIndex++)
    {
        for (int yIndex = 0; yIndex < locationHeight; yIndex++)
        {
            IntVector2 newLocationsWorldLocation = newLocation->GetWorldLocation();
            Tile* tile = m_map->GetTileAtTileCoords(IntVector2(newLocationsWorldLocation.x + xIndex, newLocationsWorldLocation.y + yIndex));
            tile->ChangeType("house");
            tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
            tile->m_permanence = 1.0f;
            int tileIndex = m_map->GetTileIndexForTile(*tile);
            m_map->m_tiles[tileIndex] = *tile;
        }
    }
    m_locations.push_back(newLocation);

    for (int worldGoalIndex = 0; worldGoalIndex < (int)m_worldGoals.size(); worldGoalIndex++)
    {
        if (m_worldGoals[worldGoalIndex]->GetGoalName().compare("build_hut") == 0)
        {
            m_worldGoals[worldGoalIndex]->m_minimumOfEachResource = m_worldGoals[worldGoalIndex]->m_maximumOfEachResource;
            break;
        }
    }

    for(Agent* agent : m_agents)
        agent->SetHome(newLocation);
    return true;
}

//*******************************************************************
bool World::AddResourceFromWorldLocationProtection(Agent* currentAgent)
{
    Tile* currentTile = m_map->GetTileAtTileCoords(currentAgent->m_worldLocation);
    std::string desiredTileType = currentAgent->m_currentGoal.m_desiredLocationTileType;  

    if (currentTile != nullptr && (currentTile->m_tileDefinition->m_type.compare(desiredTileType) == 0))
    {
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_PROTECTION_ONE]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size()), *m_goalActions[ACTION_KEY_PROTECTION_TWO]);
        return true;
    }

    AddResourcesToAgent(2, currentTile->m_tileDefinition->m_type, currentAgent);
    ChangeTileToDirtTile(currentTile);

    currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size() - 1), *m_goalActions[ACTION_KEY_PROTECTION_ONE]);
    currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + ((int)currentAgent->m_currentGoal.m_goalActions.size() - 1), *m_goalActions[ACTION_KEY_PROTECTION_TWO]);

    return true;
}

//*******************************************************************
bool World::MakeRope(Agent* currentAgent)
{
    currentAgent->AddResourceWeightToCurrentWeight(new Resource("rope"));
    return true;
}

//*******************************************************************
bool World::MakeAxe(Agent* currentAgent)
{
    currentAgent->AddResourceWeightToCurrentWeight(new Resource("axe"));
    return true;
}

//*******************************************************************
bool World::GoToHome(Agent* currentAgent)
{
    FindClosestTileForGoal(currentAgent);
    IntVector2 houseLocation = currentAgent->GetCurrentGoal().m_desiredLocation;
    currentAgent->m_currentPath = m_map->GeneratePath(currentAgent->m_worldLocation, houseLocation, nullptr);
    bool doesAgentContinuePath = FollowPath(currentAgent);
    if (doesAgentContinuePath)
        return true;
    return false;
}

//*******************************************************************
bool World::CompleteCommunityHousing(Agent* currentAgent)
{
    LocationDataFromFile communityHousingData = GetLocationData((int)LOCATION_HOUSE);
    Location* newLocation = new Location(LOCATION_HOUSE, communityHousingData.m_worldLocation, communityHousingData.m_size);
    newLocation->SetHealth(communityHousingData.m_health);
    for (int xIndex = 0; xIndex < newLocation->GetLocationSize().x; xIndex++)
    {
        for (int yIndex = 0; yIndex < newLocation->GetLocationSize().y; yIndex++)
        {
            Tile* tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex));
            tile->ChangeType("house");
            tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
            tile->m_permanence = 1.0f;
            int tileIndex = m_map->GetTileIndexForTile(*tile);
            m_map->m_tiles[tileIndex] = *tile;
        }
    }
    m_locations.push_back(newLocation);
    currentAgent->SetHome(newLocation);

    for (int worldGoalIndex = 0; worldGoalIndex < (int)m_worldGoals.size(); worldGoalIndex++)
    {
        if (m_worldGoals[worldGoalIndex]->GetGoalName().compare("build_community_housing") == 0)
        {
            m_worldGoals[worldGoalIndex]->m_minimumOfEachResource = m_worldGoals[worldGoalIndex]->m_maximumOfEachResource;
            break;
        }
    }

    for (Agent* agent : m_agents)
        agent->SetHome(newLocation);
    return true;
}

//*******************************************************************
bool World::CompleteFoodStorage(Agent* currentAgent)
{
    UNUSED(currentAgent);
    LocationDataFromFile foodStorageData = GetLocationData((int)LOCATION_FOOD_STORAGE);
    Location* newLocation = new Location(LOCATION_FOOD_STORAGE, foodStorageData.m_worldLocation, foodStorageData.m_size);
    newLocation->SetHealth(foodStorageData.m_health);
    for (int xIndex = 0; xIndex < newLocation->GetLocationSize().x; xIndex++)
    {
        for (int yIndex = 0; yIndex < newLocation->GetLocationSize().y; yIndex++)
        {
            Tile* tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex));
            tile->ChangeType("food_storage");
            tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
            tile->m_permanence = 1.0f;
            int tileIndex = m_map->GetTileIndexForTile(*tile);
            m_map->m_tiles[tileIndex] = *tile;

            if (((xIndex > 0) && (xIndex < (newLocation->GetLocationSize().x - 1))) && ((yIndex > 0) && (yIndex < (newLocation->GetLocationSize().y - 1))))
            {
                tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex + 1, newLocation->GetWorldLocation().y + yIndex));
                tile->ChangeType("food");
                tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
                tile->m_permanence = 1.0f;
                tileIndex = m_map->GetTileIndexForTile(*tile);
                m_map->m_tiles[tileIndex] = *tile;
            }
        }
    }
    m_locations.push_back(newLocation);

    for (int worldGoalIndex = 0; worldGoalIndex < (int)m_worldGoals.size(); worldGoalIndex++)
    {
        if (m_worldGoals[worldGoalIndex]->GetGoalName().compare("build_food_storage") == 0)
        {
            m_worldGoals[worldGoalIndex]->m_minimumOfEachResource = m_worldGoals[worldGoalIndex]->m_maximumOfEachResource;
            break;
        }
    }

    return true;
}

//*******************************************************************
bool World::CompleteWaterPipe(Agent* currentAgent)
{
    UNUSED(currentAgent);
    LocationDataFromFile waterPipeData = GetLocationData((int)LOCATION_PIPE);
    Location* newLocation = new Location(LOCATION_PIPE, waterPipeData.m_worldLocation, waterPipeData.m_size);
    newLocation->SetHealth(waterPipeData.m_health);
    for (int xIndex = 0; xIndex < newLocation->GetLocationSize().x; xIndex++)
    {
        int yIndex = 0;
        Tile* tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex));
        tile->ChangeType("water_pipe");
        tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
        tile->m_permanence = 1.0f;
        int tileIndex = m_map->GetTileIndexForTile(*tile);
        m_map->m_tiles[tileIndex] = *tile;

        if (xIndex == (newLocation->GetLocationSize().x - 1))
        {
            tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex + 1, newLocation->GetWorldLocation().y + yIndex));
            tile->ChangeType("water");
            tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex + 1, newLocation->GetWorldLocation().y + yIndex);
            tile->m_permanence = 1.0f;
            tileIndex = m_map->GetTileIndexForTile(*tile);
            m_map->m_tiles[tileIndex] = *tile;
        }
    }
    m_locations.push_back(newLocation);

    for (int worldGoalIndex = 0; worldGoalIndex < (int)m_worldGoals.size(); worldGoalIndex++)
    {
        if (m_worldGoals[worldGoalIndex]->GetGoalName().compare("build_water_pipe") == 0)
        {
            m_worldGoals[worldGoalIndex]->m_minimumOfEachResource = m_worldGoals[worldGoalIndex]->m_maximumOfEachResource;
            break;
        }
    }

    return true;
}

//*******************************************************************
bool World::AddWaterResourcesFromPipe(Agent* currentAgent)
{
    AddResourcesToAgent(2, "water", currentAgent);
    return true;
}

//*******************************************************************
bool World::ReproduceANewAgent(Agent* currentAgent)
{
    UNUSED(currentAgent);
    AddRandomTestAgentToSimulation();
    return true;
}

//*******************************************************************
bool World::AttackAnimal(Agent* currentAgent)
{
    int agentResourcesSize = (int)currentAgent->GetResources().size();
    for (int resourceIndex = 0; resourceIndex < agentResourcesSize; resourceIndex++)
    {
        Resource* resource = currentAgent->GetResources()[resourceIndex];
        std::string resourceType = resource->GetType();
        if (resourceType.compare("protection") == 0)
        {
            currentAgent->UseResource(resource);
            return true;
        }
    }

    float chanceOfLiving = GetRandomFloatInRange(-1.0f, 1.0f);
    if (chanceOfLiving >= 0.0f)
    {
        m_agentsThatAreDead.push_back(currentAgent->m_glyph);
        m_agentsCauseOfDeath.push_back("animal attack");
        RemoveAgentFromWorld(nullptr, currentAgent);
    }
    return true;
}

//*******************************************************************
bool World::AddFoodResourcesFromAnimals(Agent* currentAgent)
{
    Tile* currentAgentTile = m_map->GetTileAtTileCoords(currentAgent->m_worldLocation);

    if (currentAgentTile != nullptr && (currentAgentTile->m_tileDefinition->m_type.compare("animal") != 0))
    {
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + (int)currentAgent->m_currentGoal.m_goalActions.size(), *m_goalActions[ACTION_KEY_GENERAL_ONE]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + (int)currentAgent->m_currentGoal.m_goalActions.size(), *m_goalActions[ACTION_KEY_FOOD_SIX]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + (int)currentAgent->m_currentGoal.m_goalActions.size(), *m_goalActions[ACTION_KEY_FOOD_FOUR]);
        currentAgent->m_currentGoal.m_goalActions.insert(currentAgent->m_currentGoal.m_goalActions.begin() + (int)currentAgent->m_currentGoal.m_goalActions.size(), *m_goalActions[ACTION_KEY_FOOD_FIVE]);
        return true;
    }

    AddResourcesToAgent(2, "food", currentAgent);
    ChangeTileToDirtTile(currentAgentTile);
    return true;
}

//*******************************************************************
bool World::CompleteWell(Agent* currentAgent)
{
    UNUSED(currentAgent);

    LocationDataFromFile wellData = GetLocationData((int)LOCATION_WELL);
    Location* newLocation = new Location(LOCATION_WELL, wellData.m_worldLocation, wellData.m_size);
    newLocation->SetHealth(wellData.m_health);
    for (int xIndex = 0; xIndex < newLocation->GetLocationSize().x; xIndex++)
    {
        for (int yIndex = 0; yIndex < newLocation->GetLocationSize().y; yIndex++)
        {
            Tile* tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex));
            tile->ChangeType("well");
            tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
            tile->m_permanence = 1.0f;
            int tileIndex = m_map->GetTileIndexForTile(*tile);
            m_map->m_tiles[tileIndex] = *tile;

            if (((xIndex > 0) && (xIndex < (newLocation->GetLocationSize().x - 1))) && ((yIndex > 0) && (yIndex < (newLocation->GetLocationSize().y - 1))))
            {
                tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex + 1, newLocation->GetWorldLocation().y + yIndex));
                tile->ChangeType("water");
                tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
                tile->m_permanence = 1.0f;
                tileIndex = m_map->GetTileIndexForTile(*tile);
                m_map->m_tiles[tileIndex] = *tile;
            }
        }
    }
    m_locations.push_back(newLocation);

    for (int worldGoalIndex = 0; worldGoalIndex < (int)m_worldGoals.size(); worldGoalIndex++)
    {
        if (m_worldGoals[worldGoalIndex]->GetGoalName().compare("build_well") == 0)
        {
            m_worldGoals[worldGoalIndex]->m_minimumOfEachResource = m_worldGoals[worldGoalIndex]->m_maximumOfEachResource;
            break;
        }
    }

    return true;
}

//*******************************************************************
bool World::MakeSword(Agent* currentAgent)
{
    currentAgent->AddResourceWeightToCurrentWeight(new Resource("sword"));
    return true;
}

//*******************************************************************
bool World::MakeShield(Agent* currentAgent)
{
    currentAgent->AddResourceWeightToCurrentWeight(new Resource("shield"));
    return true;
}

//*******************************************************************
bool World::CompleteFarm(Agent* currentAgent)
{
    UNUSED(currentAgent);
    LocationDataFromFile farmData = GetLocationData((int)LOCATION_FARM);
    Location* newLocation = new Location(LOCATION_FARM, farmData.m_worldLocation, farmData.m_size);
    newLocation->SetHealth(farmData.m_health);
    for (int xIndex = 0; xIndex < newLocation->GetLocationSize().x; xIndex++)
    {
        for (int yIndex = 0; yIndex < newLocation->GetLocationSize().y; yIndex++)
        {
            Tile* tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex));
            tile->ChangeType("farm");
            tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
            tile->m_permanence = 1.0f;
            int tileIndex = m_map->GetTileIndexForTile(*tile);
            m_map->m_tiles[tileIndex] = *tile;

            if (((xIndex > 0) && (xIndex < (newLocation->GetLocationSize().x - 1))) && ((yIndex > 0) && (yIndex < (newLocation->GetLocationSize().y - 1))))
            {
                tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex + 1, newLocation->GetWorldLocation().y + yIndex));
                tile->ChangeType("animal");
                tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
                tile->m_permanence = 1.0f;
                tileIndex = m_map->GetTileIndexForTile(*tile);
                m_map->m_tiles[tileIndex] = *tile;
            }
        }
    }
    m_locations.push_back(newLocation);

    for (int worldGoalIndex = 0; worldGoalIndex < (int)m_worldGoals.size(); worldGoalIndex++)
    {
        if (m_worldGoals[worldGoalIndex]->GetGoalName().compare("build_farm") == 0)
        {
            m_worldGoals[worldGoalIndex]->m_minimumOfEachResource = m_worldGoals[worldGoalIndex]->m_maximumOfEachResource;
            break;
        }
    }

    return true;
}

//*******************************************************************
bool World::CompleteRockMine(Agent* currentAgent)
{
    UNUSED(currentAgent);
    LocationDataFromFile rockMineData = GetLocationData((int)LOCATION_ROCK_MINE);
    Location* newLocation = new Location(LOCATION_ROCK_MINE, rockMineData.m_worldLocation, rockMineData.m_size);
    newLocation->SetHealth(rockMineData.m_health);
    for (int xIndex = 0; xIndex < newLocation->GetLocationSize().x; xIndex++)
    {
        for (int yIndex = 0; yIndex < newLocation->GetLocationSize().y; yIndex++)
        {
            Tile* tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex));
            tile->ChangeType("rock_mine");
            tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
            tile->m_permanence = 1.0f;
            int tileIndex = m_map->GetTileIndexForTile(*tile);
            m_map->m_tiles[tileIndex] = *tile;

            if (((xIndex > 0) && (xIndex < (newLocation->GetLocationSize().x - 1))) && ((yIndex > 0) && (yIndex < (newLocation->GetLocationSize().y - 1))))
            {
                tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex + 1, newLocation->GetWorldLocation().y + yIndex));
                tile->ChangeType("rock");
                tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
                tile->m_permanence = 1.0f;
                tileIndex = m_map->GetTileIndexForTile(*tile);
                m_map->m_tiles[tileIndex] = *tile;
            }
        }
    }
    m_locations.push_back(newLocation);

    for (int worldGoalIndex = 0; worldGoalIndex < (int)m_worldGoals.size(); worldGoalIndex++)
    {
        if (m_worldGoals[worldGoalIndex]->GetGoalName().compare("build_rock_mine") == 0)
        {
            m_worldGoals[worldGoalIndex]->m_minimumOfEachResource = m_worldGoals[worldGoalIndex]->m_maximumOfEachResource;
            break;
        }
    }

    return true;
}

//*******************************************************************
bool World::CompleteMetalMine(Agent* currentAgent)
{
    UNUSED(currentAgent);
    LocationDataFromFile metalMineData = GetLocationData((int)LOCATION_METAL_MINE);
    Location* newLocation = new Location(LOCATION_METAL_MINE, metalMineData.m_worldLocation, metalMineData.m_size);
    newLocation->SetHealth(metalMineData.m_health);
    for (int xIndex = 0; xIndex < newLocation->GetLocationSize().x; xIndex++)
    {
        for (int yIndex = 0; yIndex < newLocation->GetLocationSize().y; yIndex++)
        {
            Tile* tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex));
            tile->ChangeType("metal_mine");
            tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
            tile->m_permanence = 1.0f;
            int tileIndex = m_map->GetTileIndexForTile(*tile);
            m_map->m_tiles[tileIndex] = *tile;

            if (((xIndex > 0) && (xIndex < (newLocation->GetLocationSize().x - 1))) && ((yIndex > 0) && (yIndex < (newLocation->GetLocationSize().y - 1))))
            {
                tile = m_map->GetTileAtTileCoords(IntVector2(newLocation->GetWorldLocation().x + xIndex + 1, newLocation->GetWorldLocation().y + yIndex));
                tile->ChangeType("metal");
                tile->m_tileCoords = IntVector2(newLocation->GetWorldLocation().x + xIndex, newLocation->GetWorldLocation().y + yIndex);
                tile->m_permanence = 1.0f;
                tileIndex = m_map->GetTileIndexForTile(*tile);
                m_map->m_tiles[tileIndex] = *tile;
            }
        }
    }
    m_locations.push_back(newLocation);

    for (int worldGoalIndex = 0; worldGoalIndex < (int)m_worldGoals.size(); worldGoalIndex++)
    {
        if (m_worldGoals[worldGoalIndex]->GetGoalName().compare("build_metal_mine") == 0)
        {
            m_worldGoals[worldGoalIndex]->m_minimumOfEachResource = m_worldGoals[worldGoalIndex]->m_maximumOfEachResource;
            break;
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//*******************************************************************
void World::SetupGoals()
{
    //#1
    //Get Water From Closest Source - Tier 1, water
        //Generate Path to Source
        //Follow Path to Source
        //Add 2 new water resources upon arrival
    Goal* retrieveWaterFromRiver = new Goal();
    retrieveWaterFromRiver->m_goalActions.push_back(*m_goalActions[ACTION_KEY_WATER_ONE]);
    retrieveWaterFromRiver->m_goalActions.push_back(*m_goalActions[ACTION_KEY_WATER_TWO]);
    retrieveWaterFromRiver->m_goalActions.push_back(*m_goalActions[ACTION_KEY_WATER_THREE]);
    retrieveWaterFromRiver->m_type = "water";
    retrieveWaterFromRiver->m_name = "retrieve_water_from_source";
    retrieveWaterFromRiver->m_tier = TIER_ONE;
    retrieveWaterFromRiver->m_desiredLocationTileType = "water";
    retrieveWaterFromRiver->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);
        if (this->GetNumberOfTypeOfTile("water") == 0)
            return 0.0f;
        return agent->GetNeedForWater();
    };
    retrieveWaterFromRiver->m_buildType = INDIVIDUAL;
    m_worldGoals.push_back(retrieveWaterFromRiver);

    //#2
    //Get Food From Crops - Tier 1, food
        //Got to Crops
        //Follow Path to Crops
        //Add 2 new food resources
    Goal* retrieveFoodFromCrops = new Goal();
    retrieveFoodFromCrops->m_goalActions.push_back(*m_goalActions[ACTION_KEY_FOOD_ONE]);
    retrieveFoodFromCrops->m_goalActions.push_back(*m_goalActions[ACTION_KEY_FOOD_TWO]);
    retrieveFoodFromCrops->m_goalActions.push_back(*m_goalActions[ACTION_KEY_FOOD_THREE]);
    retrieveFoodFromCrops->m_type = "food";
    retrieveFoodFromCrops->m_name = "retrieve_food_from_crops";
    retrieveFoodFromCrops->m_tier = TIER_ONE;
    retrieveFoodFromCrops->m_desiredLocationTileType = "food";
    retrieveFoodFromCrops->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);

        if (m_maxNumberOfFoodTiles == 0)
            return 0.0f;

        float percentageOfCropTiles = (float)this->GetNumberOfTypeOfTile("food") / (float)m_maxNumberOfFoodTiles;
        if (percentageOfCropTiles <= 0.0f)
            return 0.0f;
        float needForGatheringCrops = agent->GetNeedForFood();
        return needForGatheringCrops;
    };
    retrieveFoodFromCrops->m_buildType = INDIVIDUAL;
    m_worldGoals.push_back(retrieveFoodFromCrops);

    //#3
    //Build Hut - Tier 1, shelter
        //5 wood
        //Go to designated location
        //Use current resources
        //If resources used not enough, go get rest
        //Repeat previous 2 steps until built
        //build hut
    LocationDataFromFile hutData = GetLocationData((int)LOCATION_HUT);
    Goal* buildHut = new Goal();
    buildHut->m_neededResources.push_back(Resource("wood"));
    buildHut->m_importanceOfEachResource.push_back(1.0f);
    buildHut->m_minimumOfEachResource.push_back(5.0f);
    buildHut->m_maximumOfEachResource.push_back(8.0f);
    buildHut->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_ONE]);
    buildHut->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_TWO]);
    buildHut->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_THREE]);
    buildHut->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_FOUR]);
    buildHut->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_SIX]);
    buildHut->m_type = "shelter";
    buildHut->m_name = "build_hut";
    buildHut->m_tier = TIER_ONE;
    buildHut->m_desiredLocation = hutData.m_worldLocation;
    buildHut->m_desiredLocationTileType = "none";
    buildHut->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);

        CalculatePopulationsNeedForShelter();
        float goalUtility = this->m_populationsNeedForShelter * 0.8f;
        for (int locationIndex = 0; locationIndex < (int)this->m_locations.size(); locationIndex++)
        {
            Location* currentLocation = this->m_locations[locationIndex];
            if ((currentLocation->GetLocationType() == LOCATION_HUT) || (currentLocation->GetLocationType() == LOCATION_HOUSE))
                goalUtility = 0.0f;
        }
                
        if (this->GetNumberOfTypeOfTile("wood") == 0)
            return 0.0f;

        return goalUtility;
    };
    buildHut->m_buildType = GROUP;
    m_worldGoals.push_back(buildHut);

    //#4
    //Make Axe - Tier 1, protection
        //1 wood, 1 metal
        //Use resources
        //If resources not enough, go get rest
        //Finish making axe, add to resources
    Goal* makeAxe = new Goal();
    makeAxe->m_neededResources.push_back(Resource("wood"));
    makeAxe->m_neededResources.push_back(Resource("metal"));
    makeAxe->m_importanceOfEachResource.push_back(0.5f);
    makeAxe->m_importanceOfEachResource.push_back(0.5f);
    makeAxe->m_minimumOfEachResource.push_back(1.0f);
    makeAxe->m_minimumOfEachResource.push_back(1.0f);
    makeAxe->m_maximumOfEachResource.push_back(2.0f);
    makeAxe->m_maximumOfEachResource.push_back(2.0f);
    makeAxe->m_goalActions.push_back(*m_goalActions[ACTION_KEY_PROTECTION_ONE]);
    makeAxe->m_goalActions.push_back(*m_goalActions[ACTION_KEY_PROTECTION_TWO]);
    makeAxe->m_goalActions.push_back(*m_goalActions[ACTION_KEY_PROTECTION_FIVE]);
    makeAxe->m_type = "protection";
    makeAxe->m_name = "make_axe";
    makeAxe->m_tier = TIER_ONE;
    makeAxe->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(goal);

        if ((this->GetNumberOfTypeOfTile("wood") == 0) || (this->GetNumberOfTypeOfTile("rock") == 0))
            return 0.0f;

        float goalUtility = agent->GetNeedForProtection() * 0.8f;
        return goalUtility;
    };
    makeAxe->m_desiredLocationTileType = "none";
    makeAxe->m_buildType = INDIVIDUAL;
    m_worldGoals.push_back(makeAxe);

    //#5
    //Visit Hut - Tier 1, shelter
        //If shelter does not exist, cannot do this
    Goal* visitHome = new Goal();
    visitHome->m_goalActions.push_back(*m_goalActions[ACTION_KEY_GENERAL_ONE]);
    visitHome->m_goalActions.push_back(*m_goalActions[ACTION_KEY_WATER_TWO]);
    visitHome->m_type = "shelter";
    visitHome->m_name = "visit_home";
    visitHome->m_tier = TIER_ONE;
    visitHome->m_desiredLocationTileType = "house";
    visitHome->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(goal);
        float goalUtility = 0.0f;
        for (int locationIndex = 0; locationIndex < (int)this->m_locations.size(); locationIndex++)
        {
            Location* currentLocation = this->m_locations[locationIndex];
            if ((currentLocation->GetLocationType() == LOCATION_HUT) || (currentLocation->GetLocationType() == LOCATION_HOUSE))
                goalUtility = agent->GetNeedForShelter();
        }

        return goalUtility;
    };
    visitHome->m_buildType = INDIVIDUAL;
    m_worldGoals.push_back(visitHome);

    //#6
    //Build Community Housing - Tier 2
        //20 wood, 10 metal
        //If resources not enough, go get rest
        //Finish making axe, add to resources
    LocationDataFromFile communityHousingData = GetLocationData((int)LOCATION_HOUSE);
    Goal* buildCommunityHousing = new Goal();
    buildCommunityHousing->m_neededResources.push_back(Resource("wood"));
    buildCommunityHousing->m_neededResources.push_back(Resource("metal"));
    buildCommunityHousing->m_importanceOfEachResource.push_back(0.6f);
    buildCommunityHousing->m_importanceOfEachResource.push_back(0.4f);
    buildCommunityHousing->m_minimumOfEachResource.push_back(20.0f);
    buildCommunityHousing->m_minimumOfEachResource.push_back(10.0f);
    buildCommunityHousing->m_maximumOfEachResource.push_back(30.0f);
    buildCommunityHousing->m_maximumOfEachResource.push_back(20.0f);
    buildCommunityHousing->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_ONE]);
    buildCommunityHousing->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_TWO]);
    buildCommunityHousing->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_THREE]);
    buildCommunityHousing->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_FOUR]);
    buildCommunityHousing->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_EIGHT]);
    buildCommunityHousing->m_type = "shelter";
    buildCommunityHousing->m_name = "build_community_housing";
    buildCommunityHousing->m_tier = TIER_TWO;
    buildCommunityHousing->m_desiredLocation = communityHousingData.m_worldLocation;
    buildCommunityHousing->m_desiredLocationTileType = "none";
    buildCommunityHousing->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);
        CalculatePopulationsNeedForShelter();
        float goalUtility = this->m_populationsNeedForShelter * 0.6f;
        for (int locationIndex = 0; locationIndex < (int)this->m_locations.size(); locationIndex++)
        {
            Location* currentLocation = this->m_locations[locationIndex];
            if (currentLocation->GetLocationType() == LOCATION_HOUSE)
                goalUtility = 0.0f;
        }
        
        if ((this->GetNumberOfTypeOfTile("wood") == 0) || (this->GetNumberOfTypeOfTile("rock") == 0))
            return 0.0f;

        return goalUtility;
    };
    buildCommunityHousing->m_buildType = GROUP;
    m_worldGoals.push_back(buildCommunityHousing);

    //#7
    //Build Food Storage - Tier 2
        //40 wood, 20 metal
        //If resources not enough, go get rest
        //Finish making 
    LocationDataFromFile foodStorageData = GetLocationData((int)LOCATION_FOOD_STORAGE);
    Goal* buildfoodStorage = new Goal();
    buildfoodStorage->m_neededResources.push_back(Resource("wood"));
    buildfoodStorage->m_neededResources.push_back(Resource("metal"));
    buildfoodStorage->m_importanceOfEachResource.push_back(0.6f);
    buildfoodStorage->m_importanceOfEachResource.push_back(0.4f);
    buildfoodStorage->m_minimumOfEachResource.push_back(40.0f);
    buildfoodStorage->m_minimumOfEachResource.push_back(20.0f);
    buildfoodStorage->m_maximumOfEachResource.push_back(80.0f);
    buildfoodStorage->m_maximumOfEachResource.push_back(40.0f);
    buildfoodStorage->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_ONE]);
    buildfoodStorage->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_TWO]);
    buildfoodStorage->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_THREE]);
    buildfoodStorage->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_FOUR]);
    buildfoodStorage->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_NINE]);
    buildfoodStorage->m_type = "food";
    buildfoodStorage->m_name = "build_food_storage";
    buildfoodStorage->m_tier = TIER_TWO;
    buildfoodStorage->m_desiredLocation = foodStorageData.m_worldLocation;
    buildfoodStorage->m_desiredLocationTileType = "none";
    buildfoodStorage->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);
        this->CalculatePopulationsNeedForFood();

        float percentageOfCropsLeftInWorld = 0.0f;
        if(m_maxNumberOfFoodTiles > 0)
            percentageOfCropsLeftInWorld = (float)this->GetNumberOfTypeOfTile("food") / (float)m_maxNumberOfFoodTiles;
         
        float goalUtility = this->m_populationsNeedForFood * (1.0f - percentageOfCropsLeftInWorld);
        for (int locationIndex = 0; locationIndex < (int)this->m_locations.size(); locationIndex++)
        {
            Location* currentLocation = this->m_locations[locationIndex];
            if (currentLocation->GetLocationType() == LOCATION_FOOD_STORAGE)
                goalUtility = 0.0f;
        }
        
        if ((this->GetNumberOfTypeOfTile("wood") == 0) || (this->GetNumberOfTypeOfTile("metal") == 0))
            return 0.0f;

        return goalUtility;
    };
    buildfoodStorage->m_buildType = GROUP;
    m_worldGoals.push_back(buildfoodStorage);

    //#8
    //Build Pipe - Tier 2 
        //75 metal
        //If resources not enough, go get rest
        //Finish making 
    LocationDataFromFile waterPipeData = GetLocationData((int)LOCATION_PIPE);
    Goal* buildWaterPipe = new Goal();
    buildWaterPipe->m_neededResources.push_back(Resource("metal"));
    buildWaterPipe->m_importanceOfEachResource.push_back(1.0f);
    buildWaterPipe->m_minimumOfEachResource.push_back(30.0f);
    buildWaterPipe->m_maximumOfEachResource.push_back(30.0f);
    buildWaterPipe->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_ONE]);
    buildWaterPipe->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_TWO]);
    buildWaterPipe->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_THREE]);
    buildWaterPipe->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_FOUR]);
    buildWaterPipe->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_TEN]);
    buildWaterPipe->m_type = "water";
    buildWaterPipe->m_name = "build_water_pipe";
    buildWaterPipe->m_tier = TIER_TWO;
    buildWaterPipe->m_desiredLocation = waterPipeData.m_worldLocation;
    buildWaterPipe->m_desiredLocationTileType = "none";
    buildWaterPipe->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);
        this->CalculatePopulationsNeedForWater();
        float goalUtility = this->m_populationsNeedForWater;
        for (int locationIndex = 0; locationIndex < (int)this->m_locations.size(); locationIndex++)
        {
            Location* currentLocation = m_locations[locationIndex];
            if (currentLocation->GetLocationType() == LOCATION_PIPE)
                goalUtility = 0.0f;
        }

        if (this->GetNumberOfTypeOfTile("metal") == 0)
            return 0.0f;

        float percentageOfExistingWaterTiles = 0.0f;
        if (this->m_maxNumberOfWaterTiles > 0)
            percentageOfExistingWaterTiles = (float)this->GetNumberOfTypeOfTile("water") / (float)this->m_maxNumberOfWaterTiles;
        return goalUtility * percentageOfExistingWaterTiles;
    };
    buildWaterPipe->m_buildType = GROUP;
    m_worldGoals.push_back(buildWaterPipe);

    //#9
    //Reproduce
        //add new agent to world
    Goal* reproduceNewAgent = new Goal();
    reproduceNewAgent->m_goalActions.push_back(*m_goalActions[ACTION_kEY_REPRODUCE_ONE]);
    reproduceNewAgent->m_type = "reproduction";
    reproduceNewAgent->m_name = "reproduce_new_agent";
    reproduceNewAgent->m_tier = TIER_ONE;
    reproduceNewAgent->m_desiredLocationTileType = "none";
    reproduceNewAgent->m_goalCalculation =
        [](Agent* agent, Goal* goal)
    {
        UNUSED(goal);
        return agent->GetNeedToReproduce();
    };
    reproduceNewAgent->m_buildType = INDIVIDUAL;
    m_worldGoals.push_back(reproduceNewAgent);

    //#10
    //Reproduce due to population dying off
        //add new agent to world
    Goal* worldReproduceNewAgentWithDecliningPopulation = new Goal();
    worldReproduceNewAgentWithDecliningPopulation->m_goalActions.push_back(*m_goalActions[ACTION_kEY_REPRODUCE_ONE]);
    worldReproduceNewAgentWithDecliningPopulation->m_type = "reproduction";
    worldReproduceNewAgentWithDecliningPopulation->m_name = "world_reproduce_population_declining";
    worldReproduceNewAgentWithDecliningPopulation->m_tier = TIER_ONE;
    worldReproduceNewAgentWithDecliningPopulation->m_desiredLocationTileType = "none";
    worldReproduceNewAgentWithDecliningPopulation->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(goal);
        UNUSED(agent);
        this->CalculatePopulationsNeedForReproduction();
        return this->m_populationsNeedForReproduction;
    };
    worldReproduceNewAgentWithDecliningPopulation->m_buildType = GROUP;
    m_worldGoals.push_back(worldReproduceNewAgentWithDecliningPopulation);

    //#11
    //Reproduce due to population being stagnant
        //add new agent to world
    Goal* worldReproduceNewAgentStagnantPopulation = new Goal();
    worldReproduceNewAgentStagnantPopulation->m_goalActions.push_back(*m_goalActions[ACTION_kEY_REPRODUCE_ONE]);
    worldReproduceNewAgentStagnantPopulation->m_type = "reproduction";
    worldReproduceNewAgentStagnantPopulation->m_name = "world_reproduce_population_stagnant";
    worldReproduceNewAgentStagnantPopulation->m_tier = TIER_TWO;
    worldReproduceNewAgentStagnantPopulation->m_desiredLocationTileType = "none";
    worldReproduceNewAgentStagnantPopulation->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(goal);
        UNUSED(agent);
        bool doINeedToReproduce = this->IsGrowthRateStagnant();
        bool isANewDay = m_currentTurn % 24 == 0;
        if (doINeedToReproduce && isANewDay)
            return 1.0f;
        return 0.0f;
    };
    worldReproduceNewAgentStagnantPopulation->m_buildType = GROUP;
    m_worldGoals.push_back(worldReproduceNewAgentStagnantPopulation);

    //#12
    //Get Food From Animals - Tier 1, food
        //Get path to animals
        //Follow Path to Animals
        //Dice roll to see if survive
        //If survive, add 4 new food resources
    Goal* retrieveFoodFromAnimals = new Goal();
    retrieveFoodFromAnimals->m_goalActions.push_back(*m_goalActions[ACTION_KEY_GENERAL_ONE]);
    retrieveFoodFromAnimals->m_goalActions.push_back(*m_goalActions[ACTION_KEY_FOOD_SIX]);
    retrieveFoodFromAnimals->m_goalActions.push_back(*m_goalActions[ACTION_KEY_FOOD_FOUR]);
    retrieveFoodFromAnimals->m_goalActions.push_back(*m_goalActions[ACTION_KEY_FOOD_FIVE]);
    retrieveFoodFromAnimals->m_type = "food";
    retrieveFoodFromAnimals->m_name = "retrieve_food_from_animals";
    retrieveFoodFromAnimals->m_tier = TIER_ONE;
    retrieveFoodFromAnimals->m_desiredLocationTileType = "animal";
    retrieveFoodFromAnimals->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(goal);
        if (m_maxNumberOfAnimalTiles == 0)
            return 0.0f;
        float percentageOfAnimalTiles = (float)this->GetNumberOfTypeOfTile("animal") / (float)m_maxNumberOfAnimalTiles;
        if (percentageOfAnimalTiles <= 0.0f)
            return 0.0f;
        float needForHuntingAnimals = agent->GetNeedForFood() * agent->GetNeedForFood();
        return needForHuntingAnimals;
    };
    retrieveFoodFromAnimals->m_buildType = INDIVIDUAL;
    m_worldGoals.push_back(retrieveFoodFromAnimals);

    //#13
    //Build Well - Tier 2 
        //40 rock, 5 wood
        //If resources not enough, go get rest
        //Finish making 
    LocationDataFromFile wellData = GetLocationData((int)LOCATION_WELL);
    Goal* buildWell = new Goal();
    buildWell->m_neededResources.push_back(Resource("rock"));
    buildWell->m_neededResources.push_back(Resource("wood"));
    buildWell->m_importanceOfEachResource.push_back(0.1f);
    buildWell->m_importanceOfEachResource.push_back(0.9f);
    buildWell->m_minimumOfEachResource.push_back(5.0f);
    buildWell->m_minimumOfEachResource.push_back(40.0f);
    buildWell->m_maximumOfEachResource.push_back(5.0f);
    buildWell->m_maximumOfEachResource.push_back(40.0f);
    buildWell->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_ONE]);
    buildWell->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_TWO]);
    buildWell->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_THREE]);
    buildWell->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_FOUR]);
    buildWell->m_goalActions.push_back(*m_goalActions[ACTION_KEY_WATER_FIVE]);
    buildWell->m_type = "water";
    buildWell->m_name = "build_well";
    buildWell->m_tier = TIER_TWO;
    buildWell->m_desiredLocation = wellData.m_worldLocation;
    buildWell->m_desiredLocationTileType = "none";
    buildWell->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);
        this->CalculatePopulationsNeedForWater();
        float goalUtility = this->m_populationsNeedForWater;
        for (int locationIndex = 0; locationIndex < (int)this->m_locations.size(); locationIndex++)
        {
            Location* currentLocation = m_locations[locationIndex];
            if (currentLocation->GetLocationType() == LOCATION_WELL)
                goalUtility = 0.0f;
        }

        if ((this->GetNumberOfTypeOfTile("wood") == 0) || (this->GetNumberOfTypeOfTile("rock") == 0))
            return 0.0f;

        if (m_maxNumberOfWaterTiles > 0)
            return 0.0f;

        if (m_maxNumberOfWaterTiles == 0)
            return goalUtility;

        float percentageOfExistingWaterTiles = (float)this->GetNumberOfTypeOfTile("water") / (float)this->m_maxNumberOfWaterTiles;
        if (percentageOfExistingWaterTiles > 0.3f)
            goalUtility = 0.0f;

        return goalUtility;
    };
    buildWell->m_buildType = GROUP;
    m_worldGoals.push_back(buildWell);

    //#14
    //Build a farm
    LocationDataFromFile farmData = GetLocationData((int)LOCATION_FARM);
    Goal* buildFarm = new Goal();
    buildFarm->m_neededResources.push_back(Resource("wood"));
    buildFarm->m_neededResources.push_back(Resource("metal"));
    buildFarm->m_importanceOfEachResource.push_back(0.6f);
    buildFarm->m_importanceOfEachResource.push_back(0.4f);
    buildFarm->m_minimumOfEachResource.push_back(40.0f);
    buildFarm->m_minimumOfEachResource.push_back(20.0f);
    buildFarm->m_maximumOfEachResource.push_back(80.0f);
    buildFarm->m_maximumOfEachResource.push_back(40.0f);
    buildFarm->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_ONE]);
    buildFarm->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_TWO]);
    buildFarm->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_THREE]);
    buildFarm->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_FOUR]);
    buildFarm->m_goalActions.push_back(*m_goalActions[ACTION_KEY_FOOD_SEVEN]);
    buildFarm->m_type = "food";
    buildFarm->m_name = "build_farm";
    buildFarm->m_tier = TIER_TWO;
    buildFarm->m_desiredLocation = farmData.m_worldLocation;
    buildFarm->m_desiredLocationTileType = "none";
    buildFarm->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);
        this->CalculatePopulationsNeedForFood();

        float percentageOfAnimalsLeftInWorld = 0.0f;
        if (m_maxNumberOfAnimalTiles > 0)
            percentageOfAnimalsLeftInWorld = (float)this->GetNumberOfTypeOfTile("animal") / (float)m_maxNumberOfAnimalTiles;

        float goalUtility = this->m_populationsNeedForFood * (1.0f - percentageOfAnimalsLeftInWorld);
        for (int locationIndex = 0; locationIndex < (int)this->m_locations.size(); locationIndex++)
        {
            Location* currentLocation = this->m_locations[locationIndex];
            if (currentLocation->GetLocationType() == LOCATION_FARM)
                goalUtility = 0.0f;
        }

        if ((this->GetNumberOfTypeOfTile("wood") == 0) || (this->GetNumberOfTypeOfTile("metal") == 0))
            return 0.0f;

        return goalUtility;
    };
    buildFarm->m_buildType = GROUP;
    m_worldGoals.push_back(buildFarm);

    //#15
    //Build a rock mine
    LocationDataFromFile rockMineData = GetLocationData((int)LOCATION_ROCK_MINE);
    Goal* buildRockMine = new Goal();
    buildRockMine->m_neededResources.push_back(Resource("wood"));
    buildRockMine->m_importanceOfEachResource.push_back(1.0f);
    buildRockMine->m_minimumOfEachResource.push_back(40.0f);
    buildRockMine->m_maximumOfEachResource.push_back(40.0f);
    buildRockMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_ONE]);
    buildRockMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_TWO]);
    buildRockMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_THREE]);
    buildRockMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_FOUR]);
    buildRockMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_GENERAL_TWO]);
    buildRockMine->m_type = "generic";
    buildRockMine->m_name = "build_rock_mine";
    buildRockMine->m_tier = TIER_TWO;
    buildRockMine->m_desiredLocation = rockMineData.m_worldLocation;
    buildRockMine->m_desiredLocationTileType = "none";
    buildRockMine->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);
        this->CalculatePopulationsNeedForShelter();

        float percentageOfRocksLeftInWorld = 0.0f;
        if(m_maxNumberOfRockTiles > 0)
            percentageOfRocksLeftInWorld = (float)this->GetNumberOfTypeOfTile("rock") / (float)m_maxNumberOfRockTiles;

        float goalUtility = this->m_populationsNeedForShelter * (1.0f - percentageOfRocksLeftInWorld);
        for (int locationIndex = 0; locationIndex < (int)this->m_locations.size(); locationIndex++)
        {
            Location* currentLocation = this->m_locations[locationIndex];
            if (currentLocation->GetLocationType() == LOCATION_ROCK_MINE)
                goalUtility = 0.0f;
        }
        
        if (this->GetNumberOfTypeOfTile("wood") == 0)
            return 0.0f;

        return goalUtility;
    };
    buildRockMine->m_buildType = GROUP;
    m_worldGoals.push_back(buildRockMine);

    //#16
    //Build a metal mine
    LocationDataFromFile metalMineData = GetLocationData((int)LOCATION_METAL_MINE);
    Goal* buildMetalMine = new Goal();
    buildMetalMine->m_neededResources.push_back(Resource("wood"));
    buildMetalMine->m_importanceOfEachResource.push_back(1.0f);
    buildMetalMine->m_minimumOfEachResource.push_back(40.0f);
    buildMetalMine->m_maximumOfEachResource.push_back(40.0f);
    buildMetalMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_ONE]);
    buildMetalMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_TWO]);
    buildMetalMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_THREE]);
    buildMetalMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_SHELTER_FOUR]);
    buildMetalMine->m_goalActions.push_back(*m_goalActions[ACTION_KEY_GENERAL_THREE]);
    buildMetalMine->m_type = "generic";
    buildMetalMine->m_name = "build_metal_mine";
    buildMetalMine->m_tier = TIER_TWO;
    buildMetalMine->m_desiredLocation = metalMineData.m_worldLocation;
    buildMetalMine->m_desiredLocationTileType = "none";
    buildMetalMine->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(agent);
        UNUSED(goal);
        this->CalculatePopulationsNeedForShelter();

        float percentageOfMetalLeftInWorld = 0.0f;
        if(m_maxNumberOfMetalTiles > 0)
            percentageOfMetalLeftInWorld = (float)this->GetNumberOfTypeOfTile("metal") / (float)m_maxNumberOfMetalTiles;

        float goalUtility = this->m_populationsNeedForShelter * (1.0f - percentageOfMetalLeftInWorld);
        for (int locationIndex = 0; locationIndex < (int)this->m_locations.size(); locationIndex++)
        {
            Location* currentLocation = this->m_locations[locationIndex];
            if (currentLocation->GetLocationType() == LOCATION_METAL_MINE)
                goalUtility = 0.0f;
        }
        
        if (this->GetNumberOfTypeOfTile("wood") == 0) 
            return 0.0f;

        return goalUtility;
    };
    buildMetalMine->m_buildType = GROUP;
    m_worldGoals.push_back(buildMetalMine);

    //#17
    //Make a sword - Tier 1, protection
        //1 wood, 2 metal
        //Use resources
        //If resources not enough, go get rest
        //Finish making sword, add to resources
    Goal* makeSword = new Goal();
    makeSword->m_neededResources.push_back(Resource("wood"));
    makeSword->m_neededResources.push_back(Resource("metal"));
    makeSword->m_importanceOfEachResource.push_back(0.5f);
    makeSword->m_importanceOfEachResource.push_back(0.5f);
    makeSword->m_minimumOfEachResource.push_back(1.0f);
    makeSword->m_minimumOfEachResource.push_back(2.0f);
    makeSword->m_maximumOfEachResource.push_back(1.0f);
    makeSword->m_maximumOfEachResource.push_back(2.0f);
    makeSword->m_goalActions.push_back(*m_goalActions[ACTION_KEY_PROTECTION_ONE]);
    makeSword->m_goalActions.push_back(*m_goalActions[ACTION_KEY_PROTECTION_TWO]);
    makeSword->m_goalActions.push_back(*m_goalActions[ACTION_KEY_PROTECTION_SIX]);
    makeSword->m_type = "protection";
    makeSword->m_name = "make_sword";
    makeSword->m_tier = TIER_ONE;
    makeSword->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(goal);
        float goalUtility = agent->GetNeedForProtection() * 0.6f;
        
        if ((this->GetNumberOfTypeOfTile("wood") == 0) || (this->GetNumberOfTypeOfTile("metal") == 0))
            return 0.0f;

        return goalUtility;
    };
    makeSword->m_desiredLocationTileType = "none";
    makeSword->m_buildType = INDIVIDUAL;
    m_worldGoals.push_back(makeSword);

    //#18 
    //Make a shield - Tier 1, protection
        //3 metal
        //Use resources
        //If resources not enough, go get rest
        //Finish making sword, add to resources
    Goal* makeShield = new Goal();
    makeShield->m_neededResources.push_back(Resource("wood"));
    makeShield->m_neededResources.push_back(Resource("metal"));
    makeShield->m_importanceOfEachResource.push_back(1.0f);
    makeShield->m_minimumOfEachResource.push_back(3.0f);
    makeShield->m_maximumOfEachResource.push_back(3.0f);
    makeShield->m_goalActions.push_back(*m_goalActions[ACTION_KEY_PROTECTION_ONE]);
    makeShield->m_goalActions.push_back(*m_goalActions[ACTION_KEY_PROTECTION_TWO]);
    makeShield->m_goalActions.push_back(*m_goalActions[ACTION_KEY_PROTECTION_SEVEN]);
    makeShield->m_type = "protection";
    makeShield->m_name = "make_shield";
    makeShield->m_tier = TIER_ONE;
    makeShield->m_goalCalculation =
        [this](Agent* agent, Goal* goal)
    {
        UNUSED(goal);
        float goalUtility = agent->GetNeedForProtection() * 0.4f;
        
        if ((this->GetNumberOfTypeOfTile("wood") == 0) || (this->GetNumberOfTypeOfTile("metal") == 0))
            return 0.0f;

        return goalUtility;
    };
    makeShield->m_desiredLocationTileType = "none";
    makeShield->m_buildType = INDIVIDUAL;
    m_worldGoals.push_back(makeShield);
}

//*******************************************************************
float World::GoalsResourcesCalculation(Agent* agent, Goal* goal)
{
    float goalUtility = 0.0f;
    for (int neededResourcesIndex = 0; neededResourcesIndex < (int)goal->m_neededResources.size(); neededResourcesIndex++)
    {
        float importanceOfResource = goal->m_importanceOfEachResource[neededResourcesIndex];
        float minimumNeeded = goal->m_minimumOfEachResource[neededResourcesIndex];
        float maximumNeeded = goal->m_maximumOfEachResource[neededResourcesIndex];
        float currentAmountOfResource = agent->GetAmountofTotalResourceType(goal->m_neededResources[neededResourcesIndex].GetName());

        float numerator = -1.0f * (pow((currentAmountOfResource - minimumNeeded), currentAmountOfResource));
        float denominator = (minimumNeeded / maximumNeeded) * pow((maximumNeeded - minimumNeeded), currentAmountOfResource);

        float utility = pow(fEULERS_NUMBER, (numerator / denominator));
        goalUtility += (utility * importanceOfResource);
    }
    return goalUtility;
}