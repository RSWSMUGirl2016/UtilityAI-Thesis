#include "Game/TileDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtilities.hpp"

#include <sstream>

//*******************************************************************
TileDefinition::TileDefinition()
    : m_type("none")
    , m_isSolid(false)
{
}

//*******************************************************************
TileDefinition::TileDefinition(const tinyxml2::XMLElement& xmlElement)
    : m_type("none")
    , m_isSolid(false)
{
    
    m_type = ParseXmlAttribute(xmlElement, "tiletype", m_type);
    m_isSolid = ParseXmlAttribute(xmlElement, "solid", m_isSolid);

    ChooseFillColor(xmlElement);

    Rgba glyphColor;
    glyphColor = ParseXmlAttribute(xmlElement, "glyphcolor", glyphColor);
    m_glyphColors.push_back(glyphColor);

    char glyph = ' ';
    glyph = ParseXmlAttribute(xmlElement, "glyph", glyph);
    m_glyphs.push_back(glyph);
    
    s_tileGlyphsAndTheirRespectiveNames[glyph] = m_type;
}

//*******************************************************************
TileDefinition::~TileDefinition()
{}

//*******************************************************************
void TileDefinition::ChooseFillColor(const tinyxml2::XMLElement& xmlElement)
{
    std::string fillColors;
    fillColors = ParseXmlAttribute(xmlElement, "fillcolor", fillColors);

    std::istringstream iStringStream(fillColors);
    std::string stringToken;

    std::vector<Rgba> tempFillColors;

    while (std::getline(iStringStream, stringToken, '_'))
    {
        std::istringstream iStringStreamColor(stringToken);
        std::string stringTokenColor;
        Rgba returnColorValue = Rgba::FUSCHIA;
        int currentIndex = 0;

        while (std::getline(iStringStreamColor, stringTokenColor, ','))
        {
            std::string::size_type size;
            int color = std::stoi(stringTokenColor, &size);

            if (currentIndex == 0)
                returnColorValue.r = (unsigned char)color;
            if (currentIndex == 1)
                returnColorValue.g = (unsigned char)color;
            if (currentIndex == 2)
                returnColorValue.b = (unsigned char)color;
            if (currentIndex == 3)
                returnColorValue.a = (unsigned char)color;

            currentIndex++;
        }
        m_fillColors.push_back(returnColorValue);
    }
}

//*******************************************************************
TileDefinition* TileDefinition::GetTileDefinition(const std::string& tileTypeName)
{
    return s_tileDefinitions[tileTypeName];
}

//*******************************************************************
std::string TileDefinition::GetTileTypeFromGlyph(char glyph)
{
    return s_tileGlyphsAndTheirRespectiveNames[glyph];
}

//*******************************************************************
void TileDefinition::PushTileDefinition(TileDefinition* tileDefinition)
{
    if (s_tileDefinitions.find(tileDefinition->m_type) == s_tileDefinitions.end())
    {
        s_tileDefinitions[tileDefinition->m_type] = tileDefinition;
    }
    else
    {
        ASSERT_OR_DIE(true, "Multiple tile definitions!");
    }
}

void TileDefinition::ParseFromXML(const std::string& fileName)
{
    tinyxml2::XMLDocument xmlDoc;
    //tinyxml2::XMLError eResult = xmlDoc.LoadFile("Data/Gameplay/Tiles.xml");
    tinyxml2::XMLError eResult = xmlDoc.LoadFile(fileName.c_str());
    ASSERT_OR_DIE(eResult == tinyxml2::XML_SUCCESS, "Error in loading file from file, could not find Tiles.xml file\n");

    auto root = xmlDoc.RootElement();
    ASSERT_OR_DIE(root != nullptr, "Error in loading file from file, could not find root\n");

    for (auto xml_tileDefintion = root->FirstChildElement("TileDefinition"); xml_tileDefintion != nullptr; xml_tileDefintion = xml_tileDefintion->NextSiblingElement())
    {
        TileDefinition* tileDefinition = new TileDefinition(*xml_tileDefintion);
        TileDefinition::PushTileDefinition(tileDefinition);
    }
}
