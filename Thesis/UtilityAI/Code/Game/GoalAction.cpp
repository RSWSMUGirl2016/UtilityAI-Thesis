#include "Game/GoalAction.hpp"

//*******************************************************************
void GoalAction::SetFunctionToCompleteAction(std::function<bool(Agent*)> const &callback)
{
    m_functionCompleteAction = callback;
}

//*******************************************************************
void GoalAction::RunFunctionToCompleteAction()
{
    m_functionCompleteAction;
}

//*******************************************************************
std::string GoalAction::GetCurrentGoalActionName()
{
    switch (m_actionKey)
    {
    case ACTION_KEY_TEST: return "Test Callback Function";
    case ACTION_KEY_GENERAL_ONE: return "Find Closest Tile For Goal";
    case ACTION_KEY_WATER_ONE: return "Generate Path To River";
    case ACTION_KEY_WATER_TWO: return "Follow Path";
    case ACTION_KEY_WATER_THREE: return "Add Water Resource";
    case ACTION_KEY_FOOD_ONE: return "Generate Path To Crops";
    case ACTION_KEY_FOOD_TWO: return "Follow Path";
    case ACTION_KEY_FOOD_THREE: return "Add Food Resources";
    case ACTION_KEY_SHELTER_ONE: return "Generate Path to Desired Location";
    case ACTION_KEY_SHELTER_TWO: return "Follow Path";
    case ACTION_KEY_SHELTER_THREE: return "Use Necessary Resources";
    case ACTION_KEY_SHELTER_FOUR: return "Get Necessary Shelter Resources";
    case ACTION_KEY_SHELTER_FIVE: return "Add Shelter Resources";
    case ACTION_KEY_SHELTER_SIX: return "Complete Hut";
    case ACTION_KEY_PROTECTION_ONE: return "Use Necessary Resources";
    case ACTION_KEY_PROTECTION_TWO: return "Get Necessary Protection Resources";
    case ACTION_KEY_PROTECTION_THREE: return "Add Protection Resources";
    case ACTION_KEY_PROTECTION_FOUR: return "Make Rope";
    case ACTION_KEY_PROTECTION_FIVE: return "Make Axe";
    case ACTION_KEY_SHELTER_SEVEN: return "Go To Home";
    case ACTION_KEY_SHELTER_EIGHT: return "Complete Community Housing";
    case ACTION_KEY_SHELTER_NINE: return "Complete Food Storage";
    case ACTION_KEY_SHELTER_TEN: return "Complete WAter Pipe";
    case ACTION_KEY_WATER_FOUR: return "Add Water From Pipe";
    case ACTION_kEY_REPRODUCE_ONE: return "Reproduce A New Agent";
    case ACTION_KEY_FOOD_FOUR: return "Attacking Animal";
    case ACTION_KEY_FOOD_FIVE: return "Get food from animal";
    case ACTION_KEY_FOOD_SIX: return "Follow Path";
    case ACTION_KEY_WATER_FIVE: return "Complete Well";
    case ACTION_KEY_PROTECTION_SIX: return "Make Sword";
    case ACTION_KEY_PROTECTION_SEVEN: return "Make Shield";
    case ACTION_KEY_FOOD_SEVEN: return "Complete Farm";
    case ACTION_KEY_GENERAL_TWO: return "Complete Rock Mine";
    case ACTION_KEY_GENERAL_THREE: return "Complete Metal Mine";
    }
    return "No action right now.";
}
