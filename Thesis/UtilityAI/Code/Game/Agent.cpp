#include "Game/Agent.hpp"
#include "Engine/Math/MathUtilities.hpp"

#include <math.h> 
#include <algorithm>

const float NUM_MAX_OF_TYPE_OF_RESOURCE = 5.0f;
const float MAX_WEIGHT_OF_TYPE_OF_RESOURCE = 25.0f;
const int NUM_TURNS_PER_DAY = 24;
const int NUM_MAX_TURNS_SHOULD_BE_FROM_HOME = NUM_TURNS_PER_DAY * 5;

//*******************************************************************
Agent::Agent()
    : m_worldLocation(IntVector2())
    , m_glyph(' ')
    , m_glyphColor(Rgba::MAGENTA)
    , m_currentCarryingWeight(0.0f)
    , m_currentPercentageOfWater(100.0f)
    , m_currentExposure(100.0f)
    , m_currentDanger(100.0f)
    , m_currentPercentageOfFood(100.0f)
    , m_currentResources(100.0f)
    , m_turnsSinceInShelter(0)
    , m_currentNumberOfTurns(0)
    , m_weightCanCarry(100.0f)
    , m_needForWater(0.0f)
    , m_needForShelter(0.0f)
    , m_needForProtection(0.0f)
    , m_needForFood(0.0f)
    , m_home(nullptr)
    , m_maxHealth(100.0f)
    , m_currentGoalsScores{}
    , m_allGoalsChosen{}
    , m_causeOfDeath("none")
{}

//*******************************************************************
Agent::~Agent()
{
    for (Resource* currentResource : m_resources)
    {
        if (currentResource != nullptr)
        {
            delete currentResource;
            currentResource = nullptr;
        }
    }
    m_resources.clear();
}

//*******************************************************************
void Agent::CalculateNeedForWater()
{
    float maximumNeedForWater = 100.0f;
    float currentNeedForWater = maximumNeedForWater - m_currentPercentageOfWater; //Example m_currentPercentageOfWater = 20, currentNeedForWater = 80

    float percentageOfWaterNeed = currentNeedForWater / maximumNeedForWater; // percentageOfWaterNeed = 0.8f

    float utilityScoreForNeedForWater = percentageOfWaterNeed; // utilityScoreForNeedForWater = 0.8f
    utilityScoreForNeedForWater = SmoothStep(utilityScoreForNeedForWater);
    m_needForWater = utilityScoreForNeedForWater;
}

//*******************************************************************
void Agent::CalculateNeedForShelter()
{
    float maximumNeedForShelter = 100.0f;
    float currentNeedForShelter = m_currentExposure;

    float shelterFraction = currentNeedForShelter / maximumNeedForShelter;

    float utilityScoreForNeedForShelter = shelterFraction;
    utilityScoreForNeedForShelter = SmoothStartN(utilityScoreForNeedForShelter, 2);
    m_needForShelter = utilityScoreForNeedForShelter;
}

//*******************************************************************
void Agent::CalculateNeedForProtection()
{
    float maximumNeedForProtection = 100.0f;
    float currentNeedForProtection = m_currentDanger;

    float protectionFraction = currentNeedForProtection / maximumNeedForProtection;

    float utilityScoreForNeedForProtection = protectionFraction;
    utilityScoreForNeedForProtection = SmoothStep5(utilityScoreForNeedForProtection);
    m_needForProtection = utilityScoreForNeedForProtection;
}

//*******************************************************************
void Agent::CalculateNeedForFood()
{
    float maximumNeedForFood = 100.0f;
    float currentNeedForFood = maximumNeedForFood - m_currentPercentageOfFood;

    float foodFraction = currentNeedForFood / maximumNeedForFood;

    float utilityScoreForNeedForFood = foodFraction;
    utilityScoreForNeedForFood = SmoothStartN(utilityScoreForNeedForFood, 4);
    m_needForFood = utilityScoreForNeedForFood;
}

//*******************************************************************
void Agent::SetNeedForReproduction()
{
    float maximumNeedForReproduction = 100.0f;
    float currentNeedForReproduction = m_currentWantToReproduce;
 
    float reproductionFraction = currentNeedForReproduction / maximumNeedForReproduction;
        
    float utilityScoreForNeedForReproduction = reproductionFraction;
    m_needToReproduce = utilityScoreForNeedForReproduction;
}

//*******************************************************************
void Agent::CalculateCurrentPercentageOfWater()
{
    std::string resourceType = "water";
    float weightOfAllWaterResources = GetAmountofWeightOfATypeOfResource(resourceType);
    float weightOfMaximumWaterResources = MAX_WEIGHT_OF_TYPE_OF_RESOURCE;
    m_currentPercentageOfWater = (weightOfAllWaterResources / weightOfMaximumWaterResources) * 100.0f;
}

//*******************************************************************
void Agent::CalculateCurrentExposure()
{
    bool amIHome = IsInShelter();
    if (amIHome)
    {
        m_turnsSinceInShelter = 0;
        m_currentExposure = 0.0f;
        return;
    }

    m_turnsSinceInShelter++;
    if (m_turnsSinceInShelter >= NUM_MAX_TURNS_SHOULD_BE_FROM_HOME)
        m_turnsSinceInShelter = NUM_MAX_TURNS_SHOULD_BE_FROM_HOME;
    m_currentExposure = ((float)m_turnsSinceInShelter / (float)NUM_MAX_TURNS_SHOULD_BE_FROM_HOME) * 100.0f;
}

//*******************************************************************
void Agent::CalculateCurrentDanger()
{
    std::string resourceType = "protection";
    float weightOfAllProtectionResources = GetAmountofWeightOfATypeOfResource(resourceType);
    float percentageOfProtectionResources = (weightOfAllProtectionResources / MAX_WEIGHT_OF_TYPE_OF_RESOURCE) * 100.0f;

    float healthPercentage = 100.0f - ((m_health / 100.0f) * 100.0f);
    float needForProtectionPercentage = 100.0f - percentageOfProtectionResources;

    m_currentDanger = (0.25f * healthPercentage) + (0.75f * needForProtectionPercentage);
}

//*******************************************************************
void Agent::CalculatePercentageOfFood()
{
    std::string resourceType = "food";
    float weightOfAllFoodResources = GetAmountofWeightOfATypeOfResource(resourceType);
    float weightOfMaximumFoodResources = MAX_WEIGHT_OF_TYPE_OF_RESOURCE;
    m_currentPercentageOfFood = (weightOfAllFoodResources / weightOfMaximumFoodResources) * 100.0f;
}

//*******************************************************************
void Agent::SetPercentageOfWantingToReproduce()
{
    m_currentWantToReproduce = 0.0f;
}

//*******************************************************************
float Agent::CalculateGoalUtility(Goal* currentGoal)
{
    float goalUtility = currentGoal->m_goalCalculation(this, currentGoal);
    return goalUtility;
}

//*******************************************************************
bool Agent::AmIDeadFromLowHealth()
{
    if (m_health > 0)
        return false;
    return true;
}

//*******************************************************************
float Agent::GetAmountofTotalResource(std::string resourceName)
{
    int currentNumberOfNewResource = 0;

    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetName().compare(resourceName) == 0)
            currentNumberOfNewResource = currentResource->GetNumberOfUses();
    }

    return (float)currentNumberOfNewResource;
}

//*******************************************************************
float Agent::GetAmountofTotalResourceType(std::string resourceType)
{
    int currentNumberOfNewResource = 0;

    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetType().compare(resourceType) == 0)
            currentNumberOfNewResource++;
    }

    return (float)currentNumberOfNewResource;
}

//*******************************************************************
float Agent::GetAmountofWeightOfATypeOfResource(std::string resourceType)
{
    float currentWeightOfResource = 0;

    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetType().compare(resourceType) == 0)
            currentWeightOfResource += currentResource->GetWeight();
    }

    return currentWeightOfResource;
}

//*******************************************************************
bool Agent::IsInShelter()
{
    if (m_home == nullptr)
        return false;

    IntVector2 bottomLeftCornerOfHome = m_home->GetWorldLocation();
    IntVector2 topRightCornerOfHome = bottomLeftCornerOfHome + m_home->GetLocationSize();

    if ((m_worldLocation.x >= bottomLeftCornerOfHome.x) && (m_worldLocation.x <= topRightCornerOfHome.x) && (m_worldLocation.y >= bottomLeftCornerOfHome.y) && (m_worldLocation.y <= topRightCornerOfHome.y))
        return true;

    return false;
}

//*******************************************************************
void Agent::DrinkWater()
{
    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetType().compare("water") == 0)
        {
            int currentNumberOfUses = currentResource->GetNumberOfUses();
            currentNumberOfUses--;
            currentResource->SetNumberOfUses(currentNumberOfUses);

            float definedWeightOfResource = currentResource->GetResourceDefinition()->m_weight;
            float definedNumberOfUsesOfResource = (float)currentResource->GetResourceDefinition()->m_numberOfUses;
            float weightPerUseOfResource = definedWeightOfResource / definedNumberOfUsesOfResource;
            RemoveResourceWeightFromCurrentWeight(currentResource->GetName(), weightPerUseOfResource);

            if (m_health < 100.0f)
                m_health+=0.25f;

            if (currentNumberOfUses == 0)
            {
                RemoveResourceWeightFromCurrentWeight(currentResource->GetName(), weightPerUseOfResource);
                return;
            }
            return;
        }
    }

    m_health--;
    bool isDead = AmIDeadFromLowHealth();
    if (isDead && (m_causeOfDeath.compare("none") == 0))
        m_causeOfDeath = "dehydration";
}

//*******************************************************************
void Agent::EatFood()
{
    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetType().compare("food") == 0)
        {
            int currentNumberOfUses = currentResource->GetNumberOfUses();
            currentNumberOfUses--;
            currentResource->SetNumberOfUses(currentNumberOfUses);

            float definedWeightOfResource = currentResource->GetResourceDefinition()->m_weight;
            float definedNumberOfUsesOfResource = (float)currentResource->GetResourceDefinition()->m_numberOfUses;
            float weightPerUseOfResource = definedWeightOfResource / definedNumberOfUsesOfResource;
            RemoveResourceWeightFromCurrentWeight(currentResource->GetName(), weightPerUseOfResource);

            if (m_health < 100.0f)
                m_health+=0.25f;

            if (currentNumberOfUses == 0)
            {
                RemoveResourceWeightFromCurrentWeight(currentResource->GetName(), weightPerUseOfResource);
                return;
            }
            return;
        }
    }

    m_health--;
    bool isDead = AmIDeadFromLowHealth();
    if (isDead && (m_causeOfDeath.compare("none") == 0))
        m_causeOfDeath = "starvation";
}

//*******************************************************************
void Agent::AdjustHealthBasedOnExposure()
{
    if (m_currentExposure >= 90.0f)
    {
        m_health--;
        bool isDead = AmIDeadFromLowHealth();
        if (isDead && (m_causeOfDeath.compare("none") == 0))
            m_causeOfDeath = "exposure";
    }

    if (m_currentExposure == 0.0f)
    {
        m_health += 10.0f;
        if (m_health > 100.0f)
            m_health = 100.0f;
    }
}

//*******************************************************************
void Agent::UseResource(Resource* resourceUsed)
{
    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetName().compare(resourceUsed->GetName()) == 0)
        {
            int currentNumberOfUses = currentResource->GetNumberOfUses();
            currentNumberOfUses--;
            currentResource->SetNumberOfUses(currentNumberOfUses);

            float definedWeightOfResource = currentResource->GetResourceDefinition()->m_weight;
            float definedNumberOfUsesOfResource = (float)currentResource->GetResourceDefinition()->m_numberOfUses;
            float weightPerUseOfResource = definedWeightOfResource / definedNumberOfUsesOfResource;
            RemoveResourceWeightFromCurrentWeight(currentResource->GetName(), weightPerUseOfResource);

            if (currentNumberOfUses == 0)
            {
                RemoveResourceWeightFromCurrentWeight(currentResource->GetName(), weightPerUseOfResource);
                return;
            }

            return;
        }
    }
}

//*******************************************************************
float Agent::GetNeedForWater()
{
    return m_needForWater;
}

//*******************************************************************
float Agent::GetNeedForShelter()
{
    return m_needForShelter;
}

//*******************************************************************
float Agent::GetNeedForProtection()
{
    return m_needForProtection;
}

//*******************************************************************
float Agent::GetNeedForFood()
{
    return m_needForFood;
}

//*******************************************************************
float Agent::GetNeedToReproduce()
{
    return m_needToReproduce;
}

//*******************************************************************
Goal Agent::GetCurrentGoal()
{
    return m_currentGoal;
}

//*******************************************************************
std::vector<Resource*> Agent::GetResources()
{
    return m_resources;
}

//*******************************************************************
void Agent::AddResourceWeightToCurrentWeight(Resource* newResource)
{
    if ((newResource->GetWeight() + m_currentCarryingWeight) > m_weightCanCarry)
        return;

    float currentWeightOfResource = 0.0f;
    int currentNumUsesOfResource = 0;
    int currentMaxNumUsesOfResource = 0;

    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetName() == newResource->GetName())
        {
            currentWeightOfResource += currentResource->GetWeight();
            currentNumUsesOfResource += currentResource->GetNumberOfUses();
            currentMaxNumUsesOfResource += currentResource->GetMaxNumberOfUses();
        }
    }

    if ((currentWeightOfResource + newResource->GetWeight()) > MAX_WEIGHT_OF_TYPE_OF_RESOURCE)
        return;

    m_currentCarryingWeight += newResource->GetWeight();

    if (currentWeightOfResource == 0.0f)
    {
        m_resources.push_back(newResource);
        return;
    }

    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetName() == newResource->GetName())
        {
            currentResource->SetWeight(currentWeightOfResource + newResource->GetWeight());
            currentResource->SetNumberOfUses(currentNumUsesOfResource + newResource->GetNumberOfUses());
            currentResource->SetMaxNumberOfUses(currentMaxNumUsesOfResource + newResource->GetMaxNumberOfUses());
        }
    }
}

//*******************************************************************
void Agent::RemoveResourceWeightFromCurrentWeight(Resource* usedResource)
{
    for (Resource* currentResource : m_resources)
    {
        if (currentResource == usedResource)
        {
            m_currentCarryingWeight -= usedResource->GetWeight();
            m_resources.erase(std::remove(m_resources.begin(), m_resources.end(), currentResource), m_resources.end());
            return;
        }
    }
}

//*******************************************************************
void Agent::RemoveResourceWeightFromCurrentWeight(std::string usedResourceName)
{
    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetName().compare(usedResourceName) == 0)
        {
            m_currentCarryingWeight -= ResourceDefinition::GetResourceDefinition(usedResourceName)->m_weight;
            m_resources.erase(std::remove(m_resources.begin(), m_resources.end(), currentResource), m_resources.end());
            return;
        }
    }
}

//*******************************************************************
void Agent::RemoveResourceWeightFromCurrentWeight(std::string usedResourceName, float amountOfWeightRemoved)
{
    for (Resource* currentResource : m_resources)
    {
        if (currentResource->GetName().compare(usedResourceName) == 0)
        {
            m_currentCarryingWeight -= amountOfWeightRemoved;
            currentResource->SetWeight(currentResource->GetWeight() - amountOfWeightRemoved);
            if (currentResource->GetWeight() <= 0.0f)
                m_resources.erase(std::remove(m_resources.begin(), m_resources.end(), currentResource), m_resources.end());
            return;
        }
    }
}

//*******************************************************************
void Agent::SetHome(Location* newHome)
{
    m_home = newHome;
}

//*******************************************************************
bool Agent::SetNewGoal(const std::vector<Goal*> currentWorldGoals)
{
    m_currentGoalsScores.clear();
    for (Goal* currentGoal : currentWorldGoals)
        m_currentGoalsScores.push_back(CalculateGoalUtility(currentGoal));

    int indexOfHighestScoringGoal = 0;
    float currentHighestGoalScore = 0.0f;
    for (int currentGoalScoreIndex = 0; currentGoalScoreIndex < (int)m_currentGoalsScores.size(); currentGoalScoreIndex++)
    {
        if (currentHighestGoalScore < m_currentGoalsScores[currentGoalScoreIndex])
        {
            indexOfHighestScoringGoal = currentGoalScoreIndex;
            currentHighestGoalScore = m_currentGoalsScores[currentGoalScoreIndex];
        }
    }

    if (m_currentGoal.GetGoalName().compare(currentWorldGoals[indexOfHighestScoringGoal]->GetGoalName()) != 0)
    {
        m_currentGoal = *currentWorldGoals[indexOfHighestScoringGoal];
        return true;
    }

    return false;
}

//*******************************************************************
void Agent::CalculateNeeds()
{
    CalculateCurrentPercentageOfWater();
    CalculateCurrentExposure();
    CalculateCurrentDanger();
    CalculatePercentageOfFood();
    SetPercentageOfWantingToReproduce();

    CalculateNeedForWater();
    CalculateNeedForShelter();
    CalculateNeedForProtection();
    CalculateNeedForFood();
    SetNeedForReproduction();
}

//*******************************************************************
void Agent::Update()
{
    m_allGoalsChosen.push_back(m_currentGoal);

    if (m_currentGoal.GetGoalName().compare("none") != 0)
        m_currentGoal.RunCurrentGoalAction(this);

    DrinkWater();
    EatFood();
    AdjustHealthBasedOnExposure();
    m_currentNumberOfTurns++;
    
    m_lifeSpan--;
    if (m_lifeSpan <= 0)
    {
        if (m_causeOfDeath.compare("none") == 0)
            m_causeOfDeath = "old age";
    }
}
