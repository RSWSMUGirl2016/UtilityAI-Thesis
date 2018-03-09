#pragma once
#include "Game/Entity.hpp"
#include "Game/FeatureDefinition.hpp"

class Feature : public Entity
{
public:
    FeatureDefinition* m_featureDefinition;
    IntVector2 m_tileCoords;

    std::string m_type;
    bool m_isSolid;    
    std::string m_subType;
    

    Feature();
	~Feature();
    
    void ChangeType(const std::string& featureName);

    virtual void Update();
    virtual void Render() const;
    virtual void Activate();
};