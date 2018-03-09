#pragma once
#include "Engine/Core/XMLUtils.hpp"
#include <map>
#include <vector>

class TileDefinition
{
public:
    std::string m_type;
    bool m_isSolid;
    std::vector<Rgba> m_fillColors;
    std::vector<Rgba> m_glyphColors;
    std::vector<char> m_glyphs;

public:
    TileDefinition();
    TileDefinition(const tinyxml2::XMLElement& xmlElement);
	~TileDefinition();

    void ChooseFillColor(const tinyxml2::XMLElement& xmlElement);

    static TileDefinition* GetTileDefinition(const std::string& tileTypeName);
    static std::string GetTileTypeFromGlyph(char glyph);
    static void PushTileDefinition(TileDefinition* tileDefinition);
    static void ParseFromXML(const std::string& fileName);

};

static std::map<std::string, TileDefinition*> s_tileDefinitions;
static std::map<char, std::string> s_tileGlyphsAndTheirRespectiveNames;