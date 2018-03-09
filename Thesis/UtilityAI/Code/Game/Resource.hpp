#pragma once

#include "Game/ResourceDefinition.hpp"

#include <string>

class Resource
{
private:
    std::string m_name;
    float m_weight;
    std::string m_type;
    int m_numberOfUses;
    int m_maxNumberOfUses;
    ResourceDefinition* m_resourceDefinition;

public:
    Resource();
    Resource(std::string resourceTypeName);
    ~Resource();

public:
    std::string GetName();
    float GetWeight();
    std::string GetType();
    int GetNumberOfUses();
    void SetNumberOfUses(int newNumberOfUses);
    void ChangeType(const std::string& resourceTypeName);
    void SetWeight(float newWeight);
    int GetMaxNumberOfUses();
    void SetMaxNumberOfUses(int newMaxNumberOfUses);
    ResourceDefinition* GetResourceDefinition();
};