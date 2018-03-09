#include "Engine/Math/MathUtilities.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tile.hpp"

#include <sstream>

//*******************************************************************
Tile::Tile()
    : m_occupant(nullptr)
    , m_aiOccupant(nullptr)
    , m_feature(nullptr)
    , m_tileDefinition(nullptr)
    , m_tileCoords(IntVector2())
    , m_inventory(Inventory())
    , m_permanence(0.f)
    , m_closedInPathID(0)
    , m_isOpenInPathID(0)
    , m_fillColor(Rgba::FUSCHIA)
{
}

//*******************************************************************
Tile::~Tile()
{
   /* if (m_occupant != nullptr)
    {
        delete m_occupant;
        m_occupant = nullptr;
    }

    if (m_feature != nullptr)
    {
        delete m_feature;
        m_feature = nullptr;
    }

    if (m_tileDefinition != nullptr)
    {
        delete m_tileDefinition;
        m_tileDefinition = nullptr;
    }*/
}

//*******************************************************************
void Tile::ChangeType(const std::string& tileTypeName)
{
    m_tileDefinition = TileDefinition::GetTileDefinition(tileTypeName);
    int randomColorIndex = GetRandomIntInRange(0, (int)m_tileDefinition->m_fillColors.size() - 1);
    m_fillColor = m_tileDefinition->m_fillColors[randomColorIndex];
    m_permanence = 0.f;
}

//*******************************************************************
void Tile::Update()
{}

//*******************************************************************
void Tile::Render() const
{
    //Only one glyph is rendered per tile; in order of priority:
    //  Actor
    //  Feature
    //  Item on ground (in Tile's inventory)
    //  Tile's own glyph

    //RenderGlyph();
    //RenderFill();
}

//*******************************************************************
void Tile::Render(std::vector<Vertex3>& fillVertexes, std::vector<Vertex3>& glyphVertexes, std::vector<Vertex3>& tintVertexes) const
{
    UNUSED(tintVertexes);

    RenderFill(fillVertexes);
    RenderGlyph(glyphVertexes);
}

//*******************************************************************
void Tile::RenderFill(std::vector<Vertex3>& fillVertexes) const
{
    float centerX = (m_tileCoords.x * TILE_WIDTH) + (TILE_WIDTH / 2.f);
    float centerY = (m_tileCoords.y * TILE_HEIGHT) + (TILE_HEIGHT / 2.f);
    
    g_theSimpleRenderer->DrawSprite2DCentered(g_theSimpleRenderer->m_whiteSample, Vector2(0, 0), Vector2(1, 1), Vector2(centerX, centerY), TILE_WIDTH, TILE_HEIGHT, m_fillColor, fillVertexes);
}

//*******************************************************************
void Tile::RenderGlyph(std::vector<Vertex3>& glyphVertexes) const
{
    char glyph = ' ';
    char glyphCstring[2];
    glyphCstring[1] = '\0';
    Rgba glyphColor;

    if (m_occupant != nullptr)
    {
        glyph = m_occupant->m_glyph;        
        glyphCstring[0] = glyph;        
        glyphColor = m_occupant->m_gylphColor;
    }
    else if (m_aiOccupant != nullptr)
    {
        glyph = m_aiOccupant->m_glyph;
        glyphCstring[0] = glyph;
        glyphColor = m_aiOccupant->m_glyphColor;
    }
    else
    {
        glyph = m_tileDefinition->m_glyphs[0];
        glyphCstring[0] = glyph;
        glyphColor = m_tileDefinition->m_glyphColors[0];
    }

    float centerX = (m_tileCoords.x * TILE_WIDTH) + (TILE_WIDTH / 2.f);
    float centerY = (m_tileCoords.y * TILE_HEIGHT) + (TILE_HEIGHT * 1.5f);

    //std::string tileCoordY = std::to_string(m_tileCoords.y);
    //g_theSimpleRenderer->DrawTextCenteredAndScaledToFitAABB2D(g_theSimpleRenderer->m_font, AABB2D(Vector2(centerX, centerY), TILE_WIDTH, TILE_HEIGHT), tileCoordY.c_str(), Rgba::WHITE, glyphVertexes);

    if ((int)glyph > 90 || (int)glyph < 65)
        return;
        
    g_theSimpleRenderer->DrawTextCenteredAndScaledToFitAABB2D(g_theSimpleRenderer->m_font, AABB2D(Vector2(centerX, centerY), TILE_WIDTH, TILE_HEIGHT), glyphCstring, Rgba::BLACK, glyphVertexes);
    g_theSimpleRenderer->DrawTextCenteredAndScaledToFitAABB2D(g_theSimpleRenderer->m_font, AABB2D(Vector2(centerX, centerY), 0.9f * TILE_WIDTH, 0.9f * TILE_HEIGHT), glyphCstring, glyphColor, glyphVertexes);
}

//*******************************************************************
void Tile::RenderTint(std::vector<Vertex3>& tintVertexes) const
{
    float centerX = (m_tileCoords.x * TILE_WIDTH) + (TILE_WIDTH / 2.f);
    float centerY = (m_tileCoords.y * TILE_HEIGHT) + (TILE_HEIGHT / 2.f);

    g_theSimpleRenderer->DrawSprite2DCentered(g_theSimpleRenderer->m_whiteSample, Vector2(0, 0), Vector2(1, 1), Vector2(centerX, centerY), TILE_WIDTH, TILE_HEIGHT, Rgba(125, 125, 125, 125), tintVertexes);
}

//*******************************************************************
Vector2 Tile::GetCenterOfTile()
{
    float centerX = (m_tileCoords.x * TILE_WIDTH) + (TILE_WIDTH / 2.f);
    float centerY = (m_tileCoords.y * TILE_HEIGHT) + (TILE_HEIGHT / 2.f);
    return Vector2(centerX, centerY);
}
