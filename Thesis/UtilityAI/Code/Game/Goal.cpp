#include "Game/Goal.hpp"

//*******************************************************************
Goal::Goal()
    : m_name("none")
{}

//*******************************************************************
Goal::~Goal()
{}

//*******************************************************************
GoalAction Goal::GetCurrentGoalAction()
{
    if(!m_goalActions.empty())
        return m_goalActions[0];
    return GoalAction();
}

//*******************************************************************
void Goal::RunCurrentGoalAction(Agent* agent)
{
    bool isGoalActionComplete = m_goalActions[0].m_functionCompleteAction(agent);
    if(isGoalActionComplete)
        m_goalActions.erase(m_goalActions.begin() + 0);
    if ((int)m_goalActions.size() == 0)
        m_name = "none";
}

//*******************************************************************
std::string Goal::GetType()
{
    return m_type;
}

//*******************************************************************
std::string Goal::GetGoalName()
{
    return m_name;
}

//*******************************************************************
Tier Goal::GetGoalTier()
{
    return m_tier;
}

//*******************************************************************
BuildType Goal::GetGoalBuildType()
{
    return m_buildType;
}
