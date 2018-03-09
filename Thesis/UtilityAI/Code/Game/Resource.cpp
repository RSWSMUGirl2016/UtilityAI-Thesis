#include "Game/Resource.hpp"

//*******************************************************************
Resource::Resource()
    : m_name("")
    , m_weight(0.0f)
    , m_resourceDefinition(nullptr)
{}

//*******************************************************************
Resource::Resource(std::string resourceTypeName)
{
    ChangeType(resourceTypeName);
    m_name = m_resourceDefinition->m_name;
    m_weight = m_resourceDefinition->m_weight;
    m_type = m_resourceDefinition->m_type;
    m_numberOfUses = m_resourceDefinition->m_numberOfUses;
    m_maxNumberOfUses = m_numberOfUses;
}

//*******************************************************************
Resource::~Resource()
{}

//*******************************************************************
std::string Resource::GetName()
{
    return m_name;
}

//*******************************************************************
float Resource::GetWeight()
{
    return m_weight;
}

//*******************************************************************
std::string Resource::GetType()
{
    return m_type;
}

//*******************************************************************
int Resource::GetNumberOfUses()
{
    return m_numberOfUses;
}

//*******************************************************************
void Resource::SetNumberOfUses(int newNumberOfUses)
{
    m_numberOfUses = newNumberOfUses;
}

//*******************************************************************
void Resource::ChangeType(const std::string& resourceTypeName)
{
    m_resourceDefinition = ResourceDefinition::GetResourceDefinition(resourceTypeName);
}

//*******************************************************************
void Resource::SetWeight(float newWeight)
{
    m_weight = newWeight;
}

//*******************************************************************
int Resource::GetMaxNumberOfUses()
{
    return m_maxNumberOfUses;
}

//*******************************************************************
void Resource::SetMaxNumberOfUses(int newMaxNumberOfUses)
{
    m_maxNumberOfUses = newMaxNumberOfUses;
}

//*******************************************************************
ResourceDefinition* Resource::GetResourceDefinition()
{
    return m_resourceDefinition;
}
