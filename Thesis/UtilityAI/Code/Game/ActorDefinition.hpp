#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include "Game/Behavior.hpp"
#include <map>
#include <vector>

class ActorDefinition
{
public:
    ActorDefinition();
    ActorDefinition(const tinyxml2::XMLElement& xmlElement);
    ~ActorDefinition();

    static ActorDefinition* GetActorDefinition(const std::string& actorName);
    static std::string GetActorTypeFromGlyph(char glyph);
    static void PushActorDefinition(ActorDefinition* actorDefinition);
    void PushBehaviors(const tinyxml2::XMLElement& xmlElement);
    static void ParseFromXML(const std::string& fileName);

    std::string m_name;
    int m_currentHealth;
    int m_totalHealth;
    int m_damage;
    int m_defense;
    std::vector<Rgba> m_fillColors;
    std::vector<Rgba> m_glyphColors;
    std::vector<char> m_glyphs;
    std::vector<Behavior*> m_behaviors;
    std::string m_faction;
    int m_totalWeight;
};

static std::map<std::string, ActorDefinition*> s_ActorDefinitions;
static std::map<char, std::string> s_actorGlyphsAndTheirRespectiveNames;