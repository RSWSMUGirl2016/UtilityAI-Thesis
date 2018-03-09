#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Math2D.hpp"
#include "Engine/Math/MathUtilities.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include <stdio.h>
#include <cstring>
#include <string>
#include <sstream>

#include "Game/ActorDefinition.hpp"
#include "Game/FeatureDefinition.hpp"
#include "Game/ItemDefinition.hpp"
#include "Game/LootDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/ResourceDefinition.hpp"
#include "ThirdParty/tinyXML/tinyxml2-master/tinyxml2.h"

using namespace std;

//*******************************************************************
Game::Game()
    :m_screenShake(Vector2())
    , m_currentTexture(0)
    , m_currentSahder(0)
    , m_camera(Camera3D(Vector3(-5.f, 0.5f, -5000.f)))
    , m_currentModelDegreeYRotation(2)
    , m_currentGameState(MAIN_MENU)
    , m_theWorld(nullptr)
    , m_currentTurnTime(0.0f)
    , m_amIDebugStepping(true)
    , m_agentToDebugIndex(0)
    , m_secondsPerTurn(MIN_SECONDS_PER_TURN)
    , m_doIRenderCharts(false)
    , m_doIRenderGrowthRateByTurnChart(false)
    , m_doIRenderGrowthRateByDayChart(false)
    , m_doIRenderNumberOfTypesGoalsByTurnChart(false)
    , m_doIRenderNumberOfInvidiualVSGroupGoalsByDayChart(false)
    , m_doIRenderGrowthRateAndIndividualVSGroupByDay(false)
    , m_doIRenderWaterNeedsByDayChart(false)
    , m_doIRenderShelterNeedsByDayChart(false)
    , m_doIRenderGoalScoresOfAgent(false)
    , m_doIRenderGrowthRateAndTypesOfGoalsByTurnChart(false)
    , m_doIRenderNumberOfInvidiualVSGroupGoalsByTurnChart(false)
    , m_doIRenderGrowthRateAndInvidiualVSGroupGoalsByTurnChart(false)
    , m_doIRenderNumberOfResourcesByTurnChart(false)
    , m_doIRenderGrowthRateAndResourcesUsedByTurnChart(false)
    , m_doIRenderAgentsDeathInfo(false)
    , m_indexOfChartToRender(0)
    , m_goalsToDebugIndex(0)
    , m_mapName("perlin map - no resources - one")
{
    g_inputSystem->SetMouseCursorHiddenWhenFocus(false);
    g_theConsole = new Console();
}

//*******************************************************************
Game::~Game()
{
    if (g_theConsole != nullptr)
    {
        delete g_theConsole;
        g_theConsole = nullptr;
    }

    if (m_theWorld != nullptr)
    {
        delete m_theWorld;
        m_theWorld = nullptr;
    }
}

//*******************************************************************
void Game::SetupSimulationSpace()
{
    ResourceDefinition::ParseFromXML("Data/AI/Resources.xml");
    TileDefinition::ParseFromXML("Data/Gameplay/Tiles.xml");
    MapDefinition::ParseFromXML("Data/Gameplay/Maps.xml");

    m_theWorld = new World(m_mapName);
}

//*******************************************************************
void Game::SetWhichChartToRender()
{
    int maxNumCharts = 8;

    if (g_inputSystem->WasKeyJustPressed(KEY_UPARROW) || g_inputSystem->WasKeyJustPressed(KEY_DOWNARROW))
    {
        if (g_inputSystem->WasKeyJustPressed(KEY_UPARROW))
            m_indexOfChartToRender--;
        if (g_inputSystem->WasKeyJustPressed(KEY_DOWNARROW))
            m_indexOfChartToRender++;
        if (m_indexOfChartToRender > maxNumCharts)
            m_indexOfChartToRender = 0;
        if (m_indexOfChartToRender < 0)
            m_indexOfChartToRender = maxNumCharts;
    }

    m_doIRenderGrowthRateByTurnChart = false;
    m_doIRenderGrowthRateByDayChart = false;
    m_doIRenderNumberOfTypesGoalsByTurnChart = false;
    m_doIRenderNumberOfInvidiualVSGroupGoalsByDayChart = false;
    m_doIRenderGrowthRateAndIndividualVSGroupByDay = false;
    m_doIRenderWaterNeedsByDayChart = false;
    m_doIRenderShelterNeedsByDayChart = false;
    m_doIRenderGrowthRateAndTypesOfGoalsByTurnChart = false;
    m_doIRenderNumberOfInvidiualVSGroupGoalsByTurnChart = false;
    m_doIRenderGrowthRateAndInvidiualVSGroupGoalsByTurnChart = false;
    m_doIRenderNumberOfResourcesByTurnChart = false;
    m_doIRenderAgentsPastGoals = false;
    m_doIRenderGrowthRateAndResourcesUsedByTurnChart = false;
    m_doIRenderAgentsDeathInfo = false;

    switch (m_indexOfChartToRender)
    {
    case 0:  m_doIRenderGrowthRateByTurnChart = true; break;
    case 1:  m_doIRenderNumberOfTypesGoalsByTurnChart = true; break;
    case 2:  m_doIRenderNumberOfInvidiualVSGroupGoalsByTurnChart = true; break;
    case 3:  m_doIRenderGrowthRateAndTypesOfGoalsByTurnChart = true; break;
    case 4:  m_doIRenderGrowthRateAndInvidiualVSGroupGoalsByTurnChart = true; break;
    case 5:  m_doIRenderNumberOfResourcesByTurnChart = true; break;
    case 6:  m_doIRenderGrowthRateAndResourcesUsedByTurnChart = true; break;
    //case 7:  m_doIRenderGrowthRateByDayChart = true; break; 
    //case 8:  m_doIRenderNumberOfInvidiualVSGroupGoalsByDayChart = true; break;
    //case 9:  m_doIRenderGrowthRateAndIndividualVSGroupByDay = true; break;
    case 7:  m_doIRenderAgentsPastGoals = true; break;
    case 8:  m_doIRenderAgentsDeathInfo = true; break;
    //case 9:  m_doIRenderWaterNeedsByDayChart = true; break;
    //case 10: m_doIRenderShelterNeedsByDayChart = true; break;
    }
}

//*******************************************************************
bool Game::IsDeveloperConsoleOpen()
{
    if (g_inputSystem->WasKeyJustPressed(KEY_OEM3))
    {
        g_theApp->m_isKeyDown = false;
        g_theApp->m_isDeveloperConsoleOpen = !g_theApp->m_isDeveloperConsoleOpen;
        g_theApp->m_didDeveloperWindowJustOpen = !g_theApp->m_didDeveloperWindowJustOpen;
        return true;
    }

    if (g_theApp->m_isDeveloperConsoleOpen)
    {
        UpdateDeveloperConsole();
        return true;
    }

    return false;
}

//*******************************************************************
void Game::Update(float deltaSeconds)
{
    UNUSED(deltaSeconds);

    //Closes applications
    if (g_inputSystem->WasKeyJustPressed(KEY_ESCAPE))
        g_theApp->OnExitRequested();

    //Restarts sim
    if (g_inputSystem->WasKeyJustPressed(KEY_BACK))
        m_theWorld = new World(m_mapName);

    if (g_inputSystem->WasKeyJustPressed('1'))
    {
        m_mapName = "utility ai map";
        m_theWorld = new World(m_mapName);
    }
    if (g_inputSystem->WasKeyJustPressed('2'))
    {
        m_mapName = "perlin map - little food - one";
        m_theWorld = new World(m_mapName);
    }
    if (g_inputSystem->WasKeyJustPressed('3'))
    {
        m_mapName = "perlin map - little water - one";
        m_theWorld = new World(m_mapName);
    }
    if (g_inputSystem->WasKeyJustPressed('4'))
    {
        m_mapName = "perlin map - no resources - one";
        m_theWorld = new World(m_mapName);
    }

    //Takes input only into developer console
    if (IsDeveloperConsoleOpen())
        return;

    //If no more agents in sim, stop taking all other input
    if ((int)m_theWorld->m_agents.size() <= 0)
        return;    

    if (!m_amIDebugStepping)
    {
        //Sim self steps, spacebar controls whether it goes at a slow pace, or a fast pace
        if (g_inputSystem->WasKeyJustPressed(KEY_SPACEBAR))
        {
            if (m_secondsPerTurn == MIN_SECONDS_PER_TURN)
                m_secondsPerTurn = MAX_SECONDS_PER_TURN;
            else
                m_secondsPerTurn = MIN_SECONDS_PER_TURN;
        }

        if (m_currentTurnTime < m_secondsPerTurn)
            m_currentTurnTime += deltaSeconds;
        else
        {
            m_theWorld->UpdateWorld();
            m_currentTurnTime = 0.0f;
        }
    }
    else
    {
        //User controls the pace to see individual turns
        if (g_inputSystem->WasKeyJustPressed(KEY_SPACEBAR))
            m_theWorld->UpdateWorld();

        //Fast Forward Key (2 sim days)
        if (g_inputSystem->WasKeyJustPressed(KEY_TAB))
        {
            for (int turnIndex = 0; turnIndex < NUM_TURNS_FORWARDED; turnIndex++)
                m_theWorld->UpdateWorld();
        }
    }

    //Let's sim run on own or let's user jump between turns
    if (g_inputSystem->WasKeyJustPressed(KEY_ENTER))
        m_amIDebugStepping = !m_amIDebugStepping;
    
    //Render info charts
    if (g_inputSystem->WasKeyJustPressed(KEY_F1))
        m_doIRenderCharts = !m_doIRenderCharts;
    
    //Scroll through all the agents to view their info
    if (g_inputSystem->WasKeyJustPressed(KEY_RIGHTARROW))
    {
        m_agentToDebugIndex += MAX_NUM_AGENTS_IN_DEBUG_TEXT;
        if (m_agentToDebugIndex > ((int)m_theWorld->m_agents.size() - 1))
            m_agentToDebugIndex = 0;
    }

    //Scroll through all the agents to view their info
    if (g_inputSystem->WasKeyJustPressed(KEY_LEFTARROW))
    {
        m_agentToDebugIndex -= MAX_NUM_AGENTS_IN_DEBUG_TEXT;
        if (m_agentToDebugIndex < 0)
        {
            int numAgents = (int)m_theWorld->m_agents.size();
            int multiplier = numAgents / MAX_NUM_AGENTS_IN_DEBUG_TEXT;
            int subtractionFromEndOfAgentsVecotr = numAgents - (multiplier * MAX_NUM_AGENTS_IN_DEBUG_TEXT);
            if (subtractionFromEndOfAgentsVecotr == 0)
                m_agentToDebugIndex = numAgents - MAX_NUM_AGENTS_IN_DEBUG_TEXT;
            else
                m_agentToDebugIndex = numAgents - subtractionFromEndOfAgentsVecotr;
        }
    }

    //Scroll through all the goals in the debug text
    if (g_inputSystem->WasKeyJustPressed(KEY_PERIOD))
    {
        m_goalsToDebugIndex += MAX_NUM_GOALS_IN_DEBUG_TEXT;
        if (m_goalsToDebugIndex > m_theWorld->m_currentTurn)
            m_goalsToDebugIndex = 0;
    }

    if (g_inputSystem->WasKeyJustPressed(KEY_COMMA))
    {
        m_goalsToDebugIndex -= MAX_NUM_GOALS_IN_DEBUG_TEXT;
        if (m_goalsToDebugIndex < 0)
        {
            int numGoals = m_theWorld->m_currentTurn;
            int multiplier = numGoals / MAX_NUM_GOALS_IN_DEBUG_TEXT;
            int subtractionFromEndOfGoalsVecotr = numGoals - (multiplier * MAX_NUM_GOALS_IN_DEBUG_TEXT);
            if (subtractionFromEndOfGoalsVecotr == 0)
                m_goalsToDebugIndex = numGoals - MAX_NUM_GOALS_IN_DEBUG_TEXT;
            else
                m_goalsToDebugIndex = numGoals - subtractionFromEndOfGoalsVecotr;
        }
    }

    //Render info charts
    if (m_doIRenderCharts)
    {
        SetWhichChartToRender();
        return;
    }

    //Switches between agent's debug texts (needs, health, current goal scores)
    if (g_inputSystem->WasKeyJustPressed(KEY_UPARROW) || g_inputSystem->WasKeyJustPressed(KEY_DOWNARROW))
        m_doIRenderGoalScoresOfAgent = !m_doIRenderGoalScoresOfAgent;
}

//*******************************************************************
void Game::UpdateMainMenuChoices(float deltaSeconds)
{
    UNUSED(deltaSeconds);

    SetupSimulationSpace();
    m_theWorld->m_map->Update();
}

//*******************************************************************
void Game::UpdatePlaySpace(float deltaSeconds)
{
    if (g_inputSystem->WasKeyJustPressed(KEY_ESCAPE))
    {
        m_currentGameState = MAIN_MENU;
    }

    if (g_inputSystem->WasKeyJustPressed('1'))
    {
        m_currentGameState = STATS_SCREEN;
    }

    UNUSED(deltaSeconds);
    UpdatePlayerKeyboardMovement(deltaSeconds);
}

//*******************************************************************
void Game::UpdateStatsScreenSpace(float deltaSeconds)
{
    if (g_inputSystem->WasKeyJustPressed(KEY_ESCAPE))
    {
        m_currentGameState = PLAY_MODE;
    }

    UNUSED(deltaSeconds);
}

//*******************************************************************
void Game::UpdateGameOverChoices(float deltaSeconds)
{
    UNUSED(deltaSeconds);
}

//*******************************************************************
void Game::UpdateVictoryChoices(float deltaSeconds)
{
    UNUSED(deltaSeconds);
}

//*******************************************************************
Vector3 Game::UpdatePlayerMouseLook(float deltaSeconds)
{
    UNUSED(deltaSeconds);

    if (!g_inputSystem->DoesAppHaveFocus())
        return Vector3(); //Don't update player look angle from mouse when in not in focus!

    const float YAW_DEGREES_PER_MOUSE_MOVE = -0.02f;
    const float PITCH_DEGREES_PER_MOUSE_MOVE = 0.02f;

    Vector2 screenSize = g_inputSystem->GetScreenSize();
    const Vector2 mouseRecenterPos = screenSize * 0.5f;
    Vector2 mouseScreenPos = g_inputSystem->GetMouseScreenCoords();
    Vector2 mouseMoveSinceLastFrame = mouseScreenPos - mouseRecenterPos;

    g_inputSystem->SetMouseCursorPosition(mouseRecenterPos);
    m_camera.m_yawDegreesAboutZ += YAW_DEGREES_PER_MOUSE_MOVE * mouseMoveSinceLastFrame.x;
    m_camera.m_pitchDegreesAboutY += PITCH_DEGREES_PER_MOUSE_MOVE * mouseMoveSinceLastFrame.y;
    m_camera.m_pitchDegreesAboutY = ClampWithin(m_camera.m_pitchDegreesAboutY, -89.9f, 89.9f);

    return Vector3(ClampWithin(PITCH_DEGREES_PER_MOUSE_MOVE * mouseMoveSinceLastFrame.y, -89.9f, 89.9f), (-1)*(YAW_DEGREES_PER_MOUSE_MOVE * mouseMoveSinceLastFrame.x), 0.f);
}

//*******************************************************************
void Game::UpdatePlayerKeyboardMovement(float deltaSeconds)
{
    UNUSED(deltaSeconds);
}

//*******************************************************************
void Game::UpdateDeveloperConsole()
{
    g_theConsole->UpdateConsole((unsigned char)g_theApp->m_keyCode);
}

//*******************************************************************
void Game::UpdateGameConsole()
{
}

//*******************************************************************
void Game::TestingButtons(float deltaSeconds)
{
    if (g_inputSystem->IsKeyDown('T'))
        deltaSeconds *= 0.1f;

    if (g_inputSystem->IsKeyDown('A'))
        DebuggerPrintf("\nThe 'A' key is down.\n");

    if (g_inputSystem->WasKeyJustPressed('B'))
        DebuggerPrintf("The 'B' key was just pressed.\n");

    XboxController xboxController = g_inputSystem->GetControllerState(0);
    if (!xboxController.IsControlledConnected())
        DebuggerPrintf("The controller is not connected.\n");

    if (xboxController.IsButtonPressed(BUTTON_A))
        DebuggerPrintf("The controller 'A' button is being pressed.\n");

    if (xboxController.WasButtonPressed(BUTTON_B))
        DebuggerPrintf("The controller 'B' button was pressed.\n");

    Vector2 pos = xboxController.m_leftJoyStick.m_position;
    float magnitude = xboxController.m_leftJoyStick.m_magnitude;
    float angle = xboxController.m_leftJoyStick.m_angleDegrees;
    DebuggerPrintf("LeftJoyStick position.x:%f, position.y:%f, magnitude:%f, angle:%f.\n", pos.x, pos.y, magnitude, angle);
}

//*******************************************************************
void Game::Render() const
{
    if (!g_theApp->m_isDeveloperConsoleOpen)
    {
        RenderSetup();
        RenderSimulationState();
        RenderDayAndTurn();
        RenderCurrentPopulation();
        m_theWorld->Render();

        if (m_doIRenderCharts && m_doIRenderGrowthRateByTurnChart)
            RenderGrowthRateOverTurnsChart();

        if (m_doIRenderCharts && m_doIRenderGrowthRateByDayChart)
            RenderGrowthRateOverDaysChart();

        if (m_doIRenderCharts && m_doIRenderNumberOfTypesGoalsByTurnChart)
            RenderNumberOfTypesGoalsByTurnChart();

        if (m_doIRenderCharts && m_doIRenderNumberOfInvidiualVSGroupGoalsByDayChart)
            RenderNumberOfIndividualVSGroupGoalsByDayChart();

        if (m_doIRenderCharts && m_doIRenderGrowthRateAndIndividualVSGroupByDay)
            RenderGrowthRateWithIndividualVSGroupGoalsOverDaysCharts();

        if (m_doIRenderCharts && m_doIRenderWaterNeedsByDayChart)
            RenderWaterNeedsChart();

        if (m_doIRenderCharts && m_doIRenderShelterNeedsByDayChart)
            RenderShelterNeedsChart();

        if (m_doIRenderCharts && m_doIRenderGrowthRateAndTypesOfGoalsByTurnChart)
            RenderGrowthRateWithTypesOfGoalsOverTurnsCharts();

        if (m_doIRenderCharts && m_doIRenderNumberOfInvidiualVSGroupGoalsByTurnChart)
            RenderNumberOfIndividualVSGroupGoalsByTurnChart();

        if (m_doIRenderCharts && m_doIRenderGrowthRateAndInvidiualVSGroupGoalsByTurnChart)
            RenderGrowthRateWithIndividualVSGroupGoalsOverTurnsCharts();

        if (m_doIRenderCharts && m_doIRenderNumberOfResourcesByTurnChart)
            RenderResourcesUsedOverTurnsChart();

        if (m_doIRenderCharts && m_doIRenderAgentsPastGoals)
            RenderAgentsPastGoalsDebugText();

        if (m_doIRenderCharts && m_doIRenderGrowthRateAndResourcesUsedByTurnChart)
            RenderGrowthRateWithResourcesUsedOverTurnsCharts();

        if (m_doIRenderCharts && m_doIRenderAgentsDeathInfo)
            RenderAgentsDeathsDebugText();

        if ((int)m_theWorld->m_agents.size() <= 0)
            RenderGameOverSpace();
    }
    else
    {
        RenderDeveloperSpace();
    }

    g_theSimpleRenderer->Present();
}

//*******************************************************************
void Game::RenderSetup() const
{
    g_theSimpleRenderer->matrix_data.view = Matrix4();

    g_theSimpleRenderer->SetRenderTarget(nullptr);
    g_theSimpleRenderer->ClearColor(Rgba(0, 0, 0, 255));
    g_theSimpleRenderer->ClearDepth();
    g_theSimpleRenderer->EnableDepthTest(false);

    g_theSimpleRenderer->SetViewport(0, 0, (unsigned int)g_theSimpleRenderer->m_output->GetWidth(), (unsigned int)g_theSimpleRenderer->m_output->GetHeight());
    g_theSimpleRenderer->SetSampler(g_theSimpleRenderer->m_pointSampler);
    g_theSimpleRenderer->SetOrthoProjection(Vector2(0.0f, 0.0f), Vector2(ORHTO_WIDTH, ORHTO_HEIGHT));

    g_theSimpleRenderer->SetShader(g_theSimpleRenderer->m_unlitShader);
    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteSample);
}

//*******************************************************************
void Game::RenderSimulationState() const
{
    std::string simulationStateString = "";
    if (m_amIDebugStepping)
        simulationStateString = "DEBUG - STEPPING";
    else
        simulationStateString = "SELF - STEPPING";

    int textWidthLocation = 0;
    int textHeightLocation = (int)ORHTO_HEIGHT;// -(int)TILE_HEIGHT;
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, textWidthLocation, textHeightLocation, (int)TILE_HEIGHT, simulationStateString.c_str(), Rgba::YELLOW);
}

//*******************************************************************
void Game::RenderDayAndTurn() const
{
    std::string currentDayString = std::to_string(m_theWorld->GetCurrentDay());
    std::string currentTurnString = std::to_string(m_theWorld->GetCurrentTurn());

    std::string currentDayAndTurnString = Stringf("Day(%s),Turn(%s)", currentDayString.c_str(), currentTurnString.c_str());

    int textWidthLocation = 0;
    int textHeightLocation = (int)ORHTO_HEIGHT - (int)TILE_HEIGHT;
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, textWidthLocation, textHeightLocation, (int)TILE_HEIGHT, currentDayAndTurnString.c_str(), Rgba::YELLOW);

}

//*******************************************************************
void Game::RenderCurrentPopulation() const
{
    std::string currentDayPopulationString = std::to_string((int)m_theWorld->m_agents.size());

    std::string currentPopulationString = Stringf("Population(%s)", currentDayPopulationString.c_str());

    int textWidthLocation = 0;
    int textHeightLocation = (int)ORHTO_HEIGHT - ((int)TILE_HEIGHT * 2);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, textWidthLocation, textHeightLocation, (int)TILE_HEIGHT, currentPopulationString.c_str(), Rgba::YELLOW);

}

//*******************************************************************
void Game::RenderChartBackground() const
{
    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteSample);
    g_theSimpleRenderer->DrawQuad(Vector2(), Vector2(ORHTO_WIDTH, ORHTO_HEIGHT), Rgba(25, 25, 25, 230));
}

//*******************************************************************
void Game::RenderXAxisOverDaysInSimulation() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    g_theSimpleRenderer->DrawLine2D(4.0f, Vector3(bottomLeftOfGraph.x, bottomLeftOfGraph.y, 0.0f), Vector3(topRightOfGraph.x, bottomLeftOfGraph.y, 0.0f), Rgba::WHITE, Rgba::WHITE);

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numDays = m_theWorld->m_currentDay + 1;
    int numTicksOnGraphX = numDays;
    int numIntervalsOnX = numTicksOnGraphX - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;
    float intervalOnXTickDisplacementH = (float)TILE_HEIGHT;
    float heightOfTick = chartDisplacementH / 2.0f;

    for (int tickOnXIndex = 0; tickOnXIndex < numTicksOnGraphX; tickOnXIndex++)
    {
        float tickLocationX = bottomLeftOfGraph.x + (tickOnXIndex * intervalOnXSize);
        float tickLocationY = intervalOnXTickDisplacementH;
        g_theSimpleRenderer->DrawLine2D(2.0f, Vector3(tickLocationX, tickLocationY, 0.0f), Vector3(tickLocationX, tickLocationY + heightOfTick, 0.0f), Rgba::WHITE, Rgba::WHITE);
    }
}

//*******************************************************************
void Game::RenderXAxisOverTurnsInSimulation() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    g_theSimpleRenderer->DrawLine2D(4.0f, Vector3(bottomLeftOfGraph.x, bottomLeftOfGraph.y, 0.0f), Vector3(topRightOfGraph.x, bottomLeftOfGraph.y, 0.0f), Rgba::WHITE, Rgba::WHITE);

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numTurns = m_theWorld->m_currentTurn + 1;
    int numTicksOnGraphX = numTurns;
    int numIntervalsOnX = numTicksOnGraphX - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;
    float intervalOnXTickDisplacementH = (float)TILE_HEIGHT;
    float heightOfTick = chartDisplacementH / 2.0f;

    for (int tickOnXIndex = 0; tickOnXIndex < numTicksOnGraphX; tickOnXIndex++)
    {
        float tickLocationX = bottomLeftOfGraph.x + (tickOnXIndex * intervalOnXSize);
        float tickLocationY = intervalOnXTickDisplacementH;
        g_theSimpleRenderer->DrawLine2D(2.0f, Vector3(tickLocationX, tickLocationY, 0.0f), Vector3(tickLocationX, tickLocationY + heightOfTick, 0.0f), Rgba::WHITE, Rgba::WHITE);
    }
}

//*******************************************************************
void Game::RenderYAxisOverPopulationSizeByDayInSimulation() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numDays = m_theWorld->m_currentDay + 1;

    int largestPopulation = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_theWorld->m_dayAndPopulationData[dayIndex] > largestPopulation)
            largestPopulation = m_theWorld->m_dayAndPopulationData[dayIndex];
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numAgents = largestPopulation;
    int numTicksOnGraphY = numAgents + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;
    float IntervalOnYTickDisplacementW = (float)TILE_WIDTH;
    float lengthOfTick = chartDisplacementW / 2.0f;

    g_theSimpleRenderer->DrawLine2D(4.0f, Vector3(bottomLeftOfGraph.x, bottomLeftOfGraph.y, 0.0f), Vector3(bottomLeftOfGraph.x, topRightOfGraph.y, 0.0f), Rgba::WHITE, Rgba::WHITE);

    for (int tickOnYIndex = 0; tickOnYIndex < numTicksOnGraphY; tickOnYIndex++)
    {
        float tickLocationX = IntervalOnYTickDisplacementW;
        float tickLocationY = bottomLeftOfGraph.y + (tickOnYIndex * intervalOnYSize);
        g_theSimpleRenderer->DrawLine2D(2.0f, Vector3(tickLocationX, tickLocationY, 0.0f), Vector3(tickLocationX + lengthOfTick, tickLocationY, 0.0f), Rgba::WHITE, Rgba::WHITE);
    }
}

//*******************************************************************
void Game::RenderYAxisOverPopulationSizeByTurnInSimulation() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numTurns = m_theWorld->m_currentTurn + 1;

    int largestPopulation = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        if (m_theWorld->m_turnAndPopulationData[turnIndex] > largestPopulation)
            largestPopulation = m_theWorld->m_turnAndPopulationData[turnIndex];
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numAgents = largestPopulation;
    int numTicksOnGraphY = numAgents + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;
    float IntervalOnYTickDisplacementW = (float)TILE_WIDTH;
    float lengthOfTick = chartDisplacementW / 2.0f;

    g_theSimpleRenderer->DrawLine2D(4.0f, Vector3(bottomLeftOfGraph.x, bottomLeftOfGraph.y, 0.0f), Vector3(bottomLeftOfGraph.x, topRightOfGraph.y, 0.0f), Rgba::WHITE, Rgba::WHITE);

    for (int tickOnYIndex = 0; tickOnYIndex < numTicksOnGraphY; tickOnYIndex++)
    {
        float tickLocationX = IntervalOnYTickDisplacementW;
        float tickLocationY = bottomLeftOfGraph.y + (tickOnYIndex * intervalOnYSize);
        g_theSimpleRenderer->DrawLine2D(2.0f, Vector3(tickLocationX, tickLocationY, 0.0f), Vector3(tickLocationX + lengthOfTick, tickLocationY, 0.0f), Rgba::WHITE, Rgba::WHITE);
    }
}

//*******************************************************************
void Game::RenderYAxisOverNumberOfGoalsByDayInSimulation() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numDays = m_theWorld->m_currentDay + 1;

    g_theSimpleRenderer->DrawLine2D(4.0f, Vector3(bottomLeftOfGraph.x, bottomLeftOfGraph.y, 0.0f), Vector3(bottomLeftOfGraph.x, topRightOfGraph.y, 0.0f), Rgba::WHITE, Rgba::WHITE);

    int largestNumberOfGoals = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_theWorld->m_dayAndIndividualGoalsChosen[dayIndex] > largestNumberOfGoals)
            largestNumberOfGoals = m_theWorld->m_dayAndIndividualGoalsChosen[dayIndex];

        if (m_theWorld->m_dayAndGroupGoalsChosen[dayIndex] > largestNumberOfGoals)
            largestNumberOfGoals = m_theWorld->m_dayAndGroupGoalsChosen[dayIndex];
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numGoals = largestNumberOfGoals;
    int numTicksOnGraphY = numGoals + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;
    float IntervalOnYTickDisplacementW = (float)TILE_WIDTH;
    float lengthOfTick = chartDisplacementW / 2.0f;

    for (int tickOnYIndex = 0; tickOnYIndex < numTicksOnGraphY; tickOnYIndex++)
    {
        float tickLocationX = IntervalOnYTickDisplacementW;
        float tickLocationY = bottomLeftOfGraph.y + (tickOnYIndex * intervalOnYSize);
        g_theSimpleRenderer->DrawLine2D(2.0f, Vector3(tickLocationX, tickLocationY, 0.0f), Vector3(tickLocationX + lengthOfTick, tickLocationY, 0.0f), Rgba::WHITE, Rgba::WHITE);
    }
}

//*******************************************************************
void Game::RenderYAxisOverNumberOfGoalsByTurnInSimulation() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numTurns = m_theWorld->m_currentTurn + 1;

    g_theSimpleRenderer->DrawLine2D(4.0f, Vector3(bottomLeftOfGraph.x, bottomLeftOfGraph.y, 0.0f), Vector3(bottomLeftOfGraph.x, topRightOfGraph.y, 0.0f), Rgba::WHITE, Rgba::WHITE);

    int largestNumberOfGoals = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        int currentNumberOfGoals = m_theWorld->m_turnAndIndividualGoalsChosen[turnIndex] + m_theWorld->m_turnAndGroupGoalsChosen[turnIndex];
        if (currentNumberOfGoals > largestNumberOfGoals)
            largestNumberOfGoals = currentNumberOfGoals;
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numGoals = largestNumberOfGoals;
    int numTicksOnGraphY = numGoals + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;
    float IntervalOnYTickDisplacementW = (float)TILE_WIDTH;
    float lengthOfTick = chartDisplacementW / 2.0f;

    for (int tickOnYIndex = 0; tickOnYIndex < numTicksOnGraphY; tickOnYIndex++)
    {
        float tickLocationX = IntervalOnYTickDisplacementW;
        float tickLocationY = bottomLeftOfGraph.y + (tickOnYIndex * intervalOnYSize);
        g_theSimpleRenderer->DrawLine2D(2.0f, Vector3(tickLocationX, tickLocationY, 0.0f), Vector3(tickLocationX + lengthOfTick, tickLocationY, 0.0f), Rgba::WHITE, Rgba::WHITE);
    }
}

//*******************************************************************
void Game::RenderYAxisOverNumberOfResourcesByTurnInSimulation() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numTurns = m_theWorld->m_currentTurn + 1;

    g_theSimpleRenderer->DrawLine2D(4.0f, Vector3(bottomLeftOfGraph.x, bottomLeftOfGraph.y, 0.0f), Vector3(bottomLeftOfGraph.x, topRightOfGraph.y, 0.0f), Rgba::WHITE, Rgba::WHITE);

    int largestAmountOfResources = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        if (m_theWorld->m_currentNumberOfResourcesPerTurn[turnIndex] > largestAmountOfResources)
            largestAmountOfResources = m_theWorld->m_currentNumberOfResourcesPerTurn[turnIndex];
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numResources = largestAmountOfResources;
    int numTicksOnGraphY = numResources + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;
    float IntervalOnYTickDisplacementW = (float)TILE_WIDTH;
    float lengthOfTick = chartDisplacementW / 2.0f;

    for (int tickOnYIndex = 0; tickOnYIndex < numTicksOnGraphY; tickOnYIndex++)
    {
        float tickLocationX = IntervalOnYTickDisplacementW;
        float tickLocationY = bottomLeftOfGraph.y + (tickOnYIndex * intervalOnYSize);
        g_theSimpleRenderer->DrawLine2D(2.0f, Vector3(tickLocationX, tickLocationY, 0.0f), Vector3(tickLocationX + lengthOfTick, tickLocationY, 0.0f), Rgba::WHITE, Rgba::WHITE);
    }
}

//*******************************************************************
void Game::RenderYAxisOverMaxNeedsByDayInSimulation() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    g_theSimpleRenderer->DrawLine2D(4.0f, Vector3(bottomLeftOfGraph.x, bottomLeftOfGraph.y, 0.0f), Vector3(bottomLeftOfGraph.x, topRightOfGraph.y, 0.0f), Rgba::WHITE, Rgba::WHITE);

    int numIntervalsFromZeroToOne = 10 / 2;

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numTicksOnGraphY = numIntervalsFromZeroToOne + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;
    float IntervalOnYTickDisplacementW = (float)TILE_WIDTH;
    float lengthOfTick = chartDisplacementW / 2.0f;

    for (int tickOnYIndex = 0; tickOnYIndex < numTicksOnGraphY; tickOnYIndex++)
    {
        float tickLocationX = IntervalOnYTickDisplacementW;
        float tickLocationY = bottomLeftOfGraph.y + (tickOnYIndex * intervalOnYSize);
        g_theSimpleRenderer->DrawLine2D(2.0f, Vector3(tickLocationX, tickLocationY, 0.0f), Vector3(tickLocationX + lengthOfTick, tickLocationY, 0.0f), Rgba::WHITE, Rgba::WHITE);
    }
}

//*******************************************************************
void Game::RenderYAxisOverNumberOfTypesGoalsByTurnInSimulation() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numTurns = m_theWorld->m_currentTurn + 1;


    int largestNumberOfGoals = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        int currentNumberOfGoalsThisTurn = m_theWorld->m_turnAndWaterGoalsData[turnIndex] + m_theWorld->m_turnAndFoodGoalsData[turnIndex] + m_theWorld->m_turnAndShelterGoalsData[turnIndex] + m_theWorld->m_turnAndProtectionGoalsData[turnIndex] + m_theWorld->m_turnAndReproductionGoalsData[turnIndex] + m_theWorld->m_turnAndGenericGoalsData[turnIndex];

        if (currentNumberOfGoalsThisTurn > largestNumberOfGoals)
            largestNumberOfGoals = currentNumberOfGoalsThisTurn;
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numGoals = largestNumberOfGoals;
    int numTicksOnGraphY = numGoals + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;
    float IntervalOnYTickDisplacementW = (float)TILE_WIDTH;
    float lengthOfTick = chartDisplacementW / 2.0f;

    g_theSimpleRenderer->DrawLine2D(4.0f, Vector3(bottomLeftOfGraph.x, bottomLeftOfGraph.y, 0.0f), Vector3(bottomLeftOfGraph.x, topRightOfGraph.y, 0.0f), Rgba::WHITE, Rgba::WHITE);

    for (int tickOnYIndex = 0; tickOnYIndex < numTicksOnGraphY; tickOnYIndex++)
    {
        float tickLocationX = IntervalOnYTickDisplacementW;
        float tickLocationY = bottomLeftOfGraph.y + (tickOnYIndex * intervalOnYSize);
        g_theSimpleRenderer->DrawLine2D(2.0f, Vector3(tickLocationX, tickLocationY, 0.0f), Vector3(tickLocationX + lengthOfTick, tickLocationY, 0.0f), Rgba::WHITE, Rgba::WHITE);
    }
}

//*******************************************************************
void Game::RenderGrowthRateOverDays() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    std::vector<Vector3> graphPointLocations;
    int numDays = m_theWorld->m_currentDay + 1;
    int dataSize = numDays;

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numIntervalsOnX = numDays - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;

    int largestPopulation = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_theWorld->m_dayAndPopulationData[dayIndex] > largestPopulation)
            largestPopulation = m_theWorld->m_dayAndPopulationData[dayIndex];
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numAgents = largestPopulation;
    int numTicksOnGraphY = numAgents + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;

    for (int dataIndex = 0; dataIndex < dataSize; dataIndex++)
    {
        float positionX = bottomLeftOfGraph.x + (dataIndex * intervalOnXSize);
        float positionY = bottomLeftOfGraph.y + (m_theWorld->m_dayAndPopulationData[dataIndex] * intervalOnYSize);
        Vector3 graphPoint = Vector3(positionX, positionY, 0.0f);
        graphPointLocations.push_back(graphPoint);
    }

    for (int graphPointIndex = 0; graphPointIndex < (((int)graphPointLocations.size()) - 1); graphPointIndex++)
    {
        Vector3 currentGraphPoint = graphPointLocations[graphPointIndex];
        Vector3 nextGraphPoint = graphPointLocations[graphPointIndex + 1];

        Vector3 directionOfLine = nextGraphPoint - currentGraphPoint;

        Rgba growthColor = Rgba::DARK_GREEN;
        if (directionOfLine.x < 0.0f || directionOfLine.y < 0.0f)
            growthColor = Rgba::DARK_RED;

        g_theSimpleRenderer->DrawLine2D(5.0f, currentGraphPoint, nextGraphPoint, growthColor, growthColor);
    }
}

//*******************************************************************
void Game::RenderGrowthRateOverTurns() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    std::vector<Vector3> graphPointLocations;
    int numTurns = m_theWorld->m_currentTurn + 1;
    int dataSize = numTurns;

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numIntervalsOnX = numTurns - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;

    int largestPopulation = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        if (m_theWorld->m_turnAndPopulationData[turnIndex] > largestPopulation)
            largestPopulation = m_theWorld->m_turnAndPopulationData[turnIndex];
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numAgents = largestPopulation;
    int numTicksOnGraphY = numAgents + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;

    for (int dataIndex = 0; dataIndex < dataSize; dataIndex++)
    {
        float positionX = bottomLeftOfGraph.x + (dataIndex * intervalOnXSize);
        float positionY = bottomLeftOfGraph.y + (m_theWorld->m_turnAndPopulationData[dataIndex] * intervalOnYSize);
        Vector3 graphPoint = Vector3(positionX, positionY, 0.0f);
        graphPointLocations.push_back(graphPoint);
    }

    for (int graphPointIndex = 0; graphPointIndex < (((int)graphPointLocations.size()) - 1); graphPointIndex++)
    {
        Vector3 currentGraphPoint = graphPointLocations[graphPointIndex];
        Vector3 nextGraphPoint = graphPointLocations[graphPointIndex + 1];

        Vector3 directionOfLine = nextGraphPoint - currentGraphPoint;

        Rgba growthColor = Rgba::GREEN;
        if (directionOfLine.x < 0.0f || directionOfLine.y < 0.0f)
            growthColor = Rgba::RED;

        g_theSimpleRenderer->DrawLine2D(3.5f, currentGraphPoint, nextGraphPoint, growthColor, growthColor);
    }
}

//*******************************************************************
void Game::RenderIndividualAndGroupGoalsBarOverDaysGraph() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numDays = m_theWorld->m_currentDay + 1;
    int numTicksOnGraphX = numDays;
    int numIntervalsOnX = numTicksOnGraphX - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;

    int largestNumberOfGoals = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_theWorld->m_dayAndIndividualGoalsChosen[dayIndex] > largestNumberOfGoals)
            largestNumberOfGoals = m_theWorld->m_dayAndIndividualGoalsChosen[dayIndex];

        if (m_theWorld->m_dayAndGroupGoalsChosen[dayIndex] > largestNumberOfGoals)
            largestNumberOfGoals = m_theWorld->m_dayAndGroupGoalsChosen[dayIndex];
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numGoals = largestNumberOfGoals;
    int numTicksOnGraphY = numGoals + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;

    float barWidth = intervalOnXSize * 0.5f;
    for (int tickOnXIndex = 0; tickOnXIndex < numTicksOnGraphX; tickOnXIndex++)
    {
        float individualBottomLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize) - (barWidth * 0.25f);
        float individualBottomLocationY = bottomLeftOfGraph.y;

        float individualBarHeightDisplacement = (float)m_theWorld->m_dayAndIndividualGoalsChosen[tickOnXIndex];
        float individualTopLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize) - (barWidth * 0.25f);
        float individualTopLocationY = bottomLeftOfGraph.y + (individualBarHeightDisplacement * intervalOnYSize);


        float groupBottomLocationX = bottomLeftOfGraph.x + (tickOnXIndex * intervalOnXSize);
        float groupBottomLocationY = bottomLeftOfGraph.y;

        float groupBarHeightDisplacement = (float)m_theWorld->m_dayAndGroupGoalsChosen[tickOnXIndex];
        float groupTopLocationX = bottomLeftOfGraph.x + (tickOnXIndex * intervalOnXSize);
        float groupTopLocationY = bottomLeftOfGraph.y + (groupBarHeightDisplacement * intervalOnYSize);


        g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(individualBottomLocationX, individualBottomLocationY, 0.0f), Vector3(individualTopLocationX, individualTopLocationY, 0.0f), Rgba::LIGHT_BLUE, Rgba::LIGHT_BLUE);
        g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(groupBottomLocationX, groupBottomLocationY, 0.0f), Vector3(groupTopLocationX, groupTopLocationY, 0.0f), Rgba::PURPLE, Rgba::PURPLE);
    }
}

//*******************************************************************
void Game::RenderIndividualAndGroupGoalsBarOverTurnsGraph() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numTurns = m_theWorld->m_currentTurn + 1;
    int numTicksOnGraphX = numTurns;
    int numIntervalsOnX = numTicksOnGraphX - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;
    
    int largestNumberOfGoals = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        int currentNumberOfGoals = m_theWorld->m_turnAndIndividualGoalsChosen[turnIndex] + m_theWorld->m_turnAndGroupGoalsChosen[turnIndex];
        if (currentNumberOfGoals > largestNumberOfGoals)
            largestNumberOfGoals = currentNumberOfGoals;
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numGoals = largestNumberOfGoals;
    int numTicksOnGraphY = numGoals + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;

    float barWidth = intervalOnXSize * 0.5f;
    for (int tickOnXIndex = 0; tickOnXIndex < numTicksOnGraphX; tickOnXIndex++)
    {
        float individualBottomLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float individualBottomLocationY = bottomLeftOfGraph.y;

        float individualBarHeightDisplacement = (float)m_theWorld->m_turnAndIndividualGoalsChosen[tickOnXIndex];
        float individualTopLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float individualTopLocationY = bottomLeftOfGraph.y + (individualBarHeightDisplacement * intervalOnYSize);


        float groupBottomLocationX = bottomLeftOfGraph.x + (tickOnXIndex * intervalOnXSize);
        float groupBottomLocationY = bottomLeftOfGraph.y;

        float groupBarHeightDisplacement = (float)m_theWorld->m_turnAndGroupGoalsChosen[tickOnXIndex];
        float groupTopLocationX = bottomLeftOfGraph.x + (tickOnXIndex * intervalOnXSize);
        float groupTopLocationY = bottomLeftOfGraph.y + (groupBarHeightDisplacement * intervalOnYSize);

        g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(individualBottomLocationX, individualBottomLocationY, 0.0f), Vector3(individualTopLocationX, individualTopLocationY, 0.0f), Rgba::LIGHT_BLUE, Rgba::LIGHT_BLUE);
        if(individualTopLocationY > 36.0f)
            g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(groupBottomLocationX, individualTopLocationY + groupBottomLocationY - bottomLeftOfGraph.y, 0.0f), Vector3(groupTopLocationX, individualTopLocationY + groupTopLocationY - bottomLeftOfGraph.y, 0.0f), Rgba::PURPLE, Rgba::PURPLE);
        else
            g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(groupBottomLocationX, groupBottomLocationY, 0.0f), Vector3(groupTopLocationX, groupTopLocationY, 0.0f), Rgba::PURPLE, Rgba::PURPLE);
    }
}

//*******************************************************************
void Game::RenderTypesOfGoalsOverTurnsBarGraph() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numTurns = m_theWorld->m_currentTurn + 1;
    int numTicksOnGraphX = numTurns;
    int numIntervalsOnX = numTicksOnGraphX - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;

    int largestNumberOfGoals = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        int currentNumberOfGoalsThisTurn = m_theWorld->m_turnAndWaterGoalsData[turnIndex] + m_theWorld->m_turnAndFoodGoalsData[turnIndex] + m_theWorld->m_turnAndShelterGoalsData[turnIndex] + m_theWorld->m_turnAndProtectionGoalsData[turnIndex] + m_theWorld->m_turnAndReproductionGoalsData[turnIndex] + m_theWorld->m_turnAndGenericGoalsData[turnIndex];
        
        if (currentNumberOfGoalsThisTurn > largestNumberOfGoals)
            largestNumberOfGoals = currentNumberOfGoalsThisTurn;
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numGoals = largestNumberOfGoals;
    int numTicksOnGraphY = numGoals + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;

    float barWidth = intervalOnXSize * 0.5f;
    for (int tickOnXIndex = 0; tickOnXIndex < numTicksOnGraphX; tickOnXIndex++)
    {
        float waterBottomLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float waterBottomLocationY = bottomLeftOfGraph.y;
        float waterBarHeightDisplacement = (float)m_theWorld->m_turnAndWaterGoalsData[tickOnXIndex];
        float waterTopLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float waterTopLocationY = bottomLeftOfGraph.y + (waterBarHeightDisplacement * intervalOnYSize);

        float foodBottomLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float foodBottomLocationY = bottomLeftOfGraph.y + (waterBarHeightDisplacement * intervalOnYSize);
        float foodBarHeightDisplacement = (float)m_theWorld->m_turnAndFoodGoalsData[tickOnXIndex];
        float foodTopLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float foodTopLocationY = bottomLeftOfGraph.y + ((foodBarHeightDisplacement + waterBarHeightDisplacement) * intervalOnYSize);

        float shelterBottomLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float shelterBottomLocationY = bottomLeftOfGraph.y + ((waterBarHeightDisplacement + foodBarHeightDisplacement) * intervalOnYSize);
        float shelterBarHeightDisplacement = (float)m_theWorld->m_turnAndShelterGoalsData[tickOnXIndex];
        float shelterTopLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float shelterTopLocationY = bottomLeftOfGraph.y + ((shelterBarHeightDisplacement + waterBarHeightDisplacement + foodBarHeightDisplacement) * intervalOnYSize);

        float protectionBottomLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float protectionBottomLocationY = bottomLeftOfGraph.y + ((waterBarHeightDisplacement + foodBarHeightDisplacement + shelterBarHeightDisplacement) * intervalOnYSize);
        float protectionBarHeightDisplacement = (float)m_theWorld->m_turnAndProtectionGoalsData[tickOnXIndex];
        float protectionTopLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float protectionTopLocationY = bottomLeftOfGraph.y + ((protectionBarHeightDisplacement + waterBarHeightDisplacement + foodBarHeightDisplacement + shelterBarHeightDisplacement) * intervalOnYSize);

        float reproductionBottomLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float reproductionBottomLocationY = bottomLeftOfGraph.y + ((waterBarHeightDisplacement + foodBarHeightDisplacement + shelterBarHeightDisplacement + protectionBarHeightDisplacement) * intervalOnYSize);
        float reproductionBarHeightDisplacement = (float)m_theWorld->m_turnAndReproductionGoalsData[tickOnXIndex];
        float reproductionTopLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float reproductionTopLocationY = bottomLeftOfGraph.y + ((reproductionBarHeightDisplacement + waterBarHeightDisplacement + foodBarHeightDisplacement + shelterBarHeightDisplacement + protectionBarHeightDisplacement) * intervalOnYSize);

        float genericBottomLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float genericBottomLocationY = bottomLeftOfGraph.y + ((waterBarHeightDisplacement + foodBarHeightDisplacement + shelterBarHeightDisplacement + protectionBarHeightDisplacement + reproductionBarHeightDisplacement) * intervalOnYSize);
        float genericBarHeightDisplacement = (float)m_theWorld->m_turnAndGenericGoalsData[tickOnXIndex];
        float genericTopLocationX = bottomLeftOfGraph.x + ((float)tickOnXIndex * intervalOnXSize);
        float genericTopLocationY = bottomLeftOfGraph.y + ((genericBarHeightDisplacement + waterBarHeightDisplacement + foodBarHeightDisplacement + shelterBarHeightDisplacement + protectionBarHeightDisplacement + reproductionBarHeightDisplacement) * intervalOnYSize);

        g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(genericBottomLocationX, genericBottomLocationY, 0.0f), Vector3(genericTopLocationX, genericTopLocationY, 0.0f), Rgba::WHITE, Rgba::WHITE);
        g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(waterBottomLocationX, waterBottomLocationY, 0.0f), Vector3(waterTopLocationX, waterTopLocationY, 0.0f), Rgba::NAVY, Rgba::NAVY);
        g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(foodBottomLocationX, foodBottomLocationY, 0.0f), Vector3(foodTopLocationX, foodTopLocationY, 0.0f), Rgba::DARK_RED, Rgba::DARK_RED);
        g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(shelterBottomLocationX, shelterBottomLocationY, 0.0f), Vector3(shelterTopLocationX, shelterTopLocationY, 0.0f), Rgba::PLUM, Rgba::PLUM);
        g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(protectionBottomLocationX, protectionBottomLocationY, 0.0f), Vector3(protectionTopLocationX, protectionTopLocationY, 0.0f), Rgba::DARK_GREEN, Rgba::DARK_GREEN);
        g_theSimpleRenderer->DrawLine2D(barWidth, Vector3(reproductionBottomLocationX, reproductionBottomLocationY, 0.0f), Vector3(reproductionTopLocationX, reproductionTopLocationY, 0.0f), Rgba::ORANGE_RED, Rgba::ORANGE_RED);
    }
}

//*******************************************************************
void Game::RenderWaterNeedsOverTime() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    std::vector<Vector3> worldGraphPointLocations;
    int numDays = m_theWorld->m_currentDay + 1;
    int dataSize = numDays;

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numIntervalsOnX = numDays - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;

    int largestPopulation = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_theWorld->m_dayAndPopulationData[dayIndex] > largestPopulation)
            largestPopulation = m_theWorld->m_dayAndPopulationData[dayIndex];
    }

    int numIntervalsFromZeroToOne = 10 / 2;

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numTicksOnGraphY = numIntervalsFromZeroToOne + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;

    int agentsSize = (int)m_theWorld->m_agents.size();
    for (int agentIndex = 0; agentIndex < agentsSize; agentIndex++)
    {
        std::vector<Vector3> individualGraphPointLocations;

        for (int dataIndex = 0; dataIndex < dataSize; dataIndex++)
        {
            float positionX = bottomLeftOfGraph.x + (dataIndex * intervalOnXSize);
            float positionY = bottomLeftOfGraph.y + (m_theWorld->m_agents[agentIndex]->m_dailyWaterNeed[dataIndex] * intervalOnYSize);
            Vector3 graphPoint = Vector3(positionX, positionY, 0.0f);
            individualGraphPointLocations.push_back(graphPoint);
        }

        for (int graphPointIndex = 0; graphPointIndex < (((int)individualGraphPointLocations.size()) - 1); graphPointIndex++)
        {
            Vector3 currentGraphPoint = individualGraphPointLocations[graphPointIndex];
            Vector3 nextGraphPoint = individualGraphPointLocations[graphPointIndex + 1];

            float currentGraphPointData = m_theWorld->m_agents[agentIndex]->m_dailyWaterNeed[graphPointIndex];
            float nexttGraphPointData = m_theWorld->m_agents[agentIndex]->m_dailyWaterNeed[graphPointIndex + 1];

            if ((currentGraphPointData == 0.0f) && (nexttGraphPointData == 0.0f))
                continue;

            Rgba lineColor = Rgba::BLUE;
            g_theSimpleRenderer->DrawLine2D(5.0f, currentGraphPoint, nextGraphPoint, lineColor, lineColor);
        }

        individualGraphPointLocations.clear();
    }

    for (int dataIndex = 0; dataIndex < dataSize; dataIndex++)
    {
        float positionX = bottomLeftOfGraph.x + (dataIndex * intervalOnXSize);
        float positionY = bottomLeftOfGraph.y + (m_theWorld->m_worldsDailyWaterNeed[dataIndex] * intervalOnYSize);
        Vector3 graphPoint = Vector3(positionX, positionY, 0.0f);
        worldGraphPointLocations.push_back(graphPoint);
    }

    for (int graphPointIndex = 0; graphPointIndex < (((int)worldGraphPointLocations.size()) - 1); graphPointIndex++)
    {
        Vector3 currentGraphPoint = worldGraphPointLocations[graphPointIndex];
        Vector3 nextGraphPoint = worldGraphPointLocations[graphPointIndex + 1];

        Rgba lineColor = Rgba::CYAN;
        g_theSimpleRenderer->DrawLine2D(5.0f, currentGraphPoint, nextGraphPoint, lineColor, lineColor);
    }
}

//*******************************************************************
void Game::RenderShelterNeedsOverTime() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    std::vector<Vector3> worldGraphPointLocations;
    int numDays = m_theWorld->m_currentDay + 1;
    int dataSize = numDays;

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numIntervalsOnX = numDays - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;

    int largestPopulation = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_theWorld->m_dayAndPopulationData[dayIndex] > largestPopulation)
            largestPopulation = m_theWorld->m_dayAndPopulationData[dayIndex];
    }

    int numIntervalsFromZeroToOne = 10 / 2;

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numTicksOnGraphY = numIntervalsFromZeroToOne + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;

    int agentsSize = (int)m_theWorld->m_agents.size();
    for (int agentIndex = 0; agentIndex < agentsSize; agentIndex++)
    {
        std::vector<Vector3> individualGraphPointLocations;

        for (int dataIndex = 0; dataIndex < dataSize; dataIndex++)
        {
            float positionX = bottomLeftOfGraph.x + (dataIndex * intervalOnXSize);
            float positionY = bottomLeftOfGraph.y + (m_theWorld->m_agents[agentIndex]->m_dailyWaterNeed[dataIndex] * intervalOnYSize);
            Vector3 graphPoint = Vector3(positionX, positionY, 0.0f);
            individualGraphPointLocations.push_back(graphPoint);
        }

        for (int graphPointIndex = 0; graphPointIndex < (((int)individualGraphPointLocations.size()) - 1); graphPointIndex++)
        {
            Vector3 currentGraphPoint = individualGraphPointLocations[graphPointIndex];
            Vector3 nextGraphPoint = individualGraphPointLocations[graphPointIndex + 1];

            float currentGraphPointData = m_theWorld->m_agents[agentIndex]->m_dailyShelterNeed[graphPointIndex];
            float nexttGraphPointData = m_theWorld->m_agents[agentIndex]->m_dailyShelterNeed[graphPointIndex + 1];

            if ((currentGraphPointData == 0.0f) && (nexttGraphPointData == 0.0f))
                continue;

            Rgba lineColor = Rgba::PURPLE;
            g_theSimpleRenderer->DrawLine2D(5.0f, currentGraphPoint, nextGraphPoint, lineColor, lineColor);
        }

        individualGraphPointLocations.clear();
    }

    for (int dataIndex = 0; dataIndex < dataSize; dataIndex++)
    {
        float positionX = bottomLeftOfGraph.x + (dataIndex * intervalOnXSize);
        float positionY = bottomLeftOfGraph.y + (m_theWorld->m_worldsDailyShelterNeed[dataIndex] * intervalOnYSize);
        Vector3 graphPoint = Vector3(positionX, positionY, 0.0f);
        worldGraphPointLocations.push_back(graphPoint);
    }

    for (int graphPointIndex = 0; graphPointIndex < (((int)worldGraphPointLocations.size()) - 1); graphPointIndex++)
    {
        Vector3 currentGraphPoint = worldGraphPointLocations[graphPointIndex];
        Vector3 nextGraphPoint = worldGraphPointLocations[graphPointIndex + 1];

        Rgba lineColor = Rgba::MAGENTA;
        g_theSimpleRenderer->DrawLine2D(5.0f, currentGraphPoint, nextGraphPoint, lineColor, lineColor);
    }
}

//*******************************************************************
void Game::RenderResourcesUsedRateOverTurns() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    std::vector<Vector3> graphPointLocations;
    int numTurns = m_theWorld->m_currentTurn + 1;
    int dataSize = numTurns;

    Vector2 displacementOfXAxis = Vector2(topRightOfGraph.x, bottomLeftOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfXAxis = displacementOfXAxis.CalcLength();
    int numIntervalsOnX = numTurns - 1;
    float intervalOnXSize = lengthOfXAxis / (float)numIntervalsOnX;

    int largestAmountOfResources = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        if (m_theWorld->m_currentNumberOfResourcesPerTurn[turnIndex] > largestAmountOfResources)
            largestAmountOfResources = m_theWorld->m_currentNumberOfResourcesPerTurn[turnIndex];
    }

    Vector2 displacementOfYAxis = Vector2(bottomLeftOfGraph.x, topRightOfGraph.y) - Vector2(bottomLeftOfGraph.x, bottomLeftOfGraph.y);
    float lengthOfYAxis = displacementOfYAxis.CalcLength();
    int numResources = largestAmountOfResources;
    int numTicksOnGraphY = numResources + 1;
    int numIntervalsOnY = numTicksOnGraphY - 1;
    float intervalOnYSize = lengthOfYAxis / (float)numIntervalsOnY;

    for (int dataIndex = 0; dataIndex < dataSize; dataIndex++)
    {
        float positionX = bottomLeftOfGraph.x + (dataIndex * intervalOnXSize);
        float positionY = bottomLeftOfGraph.y + (m_theWorld->m_currentNumberOfResourcesPerTurn[dataIndex] * intervalOnYSize);
        Vector3 graphPoint = Vector3(positionX, positionY, 0.0f);
        graphPointLocations.push_back(graphPoint);
    }

    for (int graphPointIndex = 0; graphPointIndex < (((int)graphPointLocations.size()) - 1); graphPointIndex++)
    {
        Vector3 currentGraphPoint = graphPointLocations[graphPointIndex];
        Vector3 nextGraphPoint = graphPointLocations[graphPointIndex + 1];

        Vector3 directionOfLine = nextGraphPoint - currentGraphPoint;

        Rgba growthColor = Rgba::DARK_BLUE;
        if (directionOfLine.x < 0.0f || directionOfLine.y < 0.0f)
            growthColor = Rgba::ORANGE_RED;

        g_theSimpleRenderer->DrawLine2D(5.0f, currentGraphPoint, nextGraphPoint, growthColor, growthColor);
    }
}

//*******************************************************************
void Game::RenderGrowthRateOverDaysChart() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numDays = m_theWorld->m_currentDay;

    int largestPopulation = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_theWorld->m_dayAndPopulationData[dayIndex] > largestPopulation)
            largestPopulation = m_theWorld->m_dayAndPopulationData[dayIndex];
    }

    int numAgents = largestPopulation;

    RenderChartBackground();
    RenderXAxisOverDaysInSimulation();
    RenderYAxisOverPopulationSizeByDayInSimulation();
    RenderGrowthRateOverDays();

    std::string numAgentsString = Stringf("Number of Agents: %i", numAgents);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x - (int)(TILE_HEIGHT / 1.5f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 1.5f), numAgentsString.c_str(), Rgba::WHITE);
    std::string numDaysString = Stringf("Number of Days: %i", numDays);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)topRightOfGraph.x - (int)(TILE_HEIGHT * 12.0f), (int)bottomLeftOfGraph.y + (int)(TILE_HEIGHT * 3.0f), (int)(TILE_HEIGHT * 1.5f), numDaysString.c_str(), Rgba::WHITE);
    std::string titleOfGraphString = "Population Growth Over Time";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderGrowthRateOverTurnsChart() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numTurns = m_theWorld->m_currentTurn;

    int largestPopulation = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        if (m_theWorld->m_turnAndPopulationData[turnIndex] > largestPopulation)
            largestPopulation = m_theWorld->m_turnAndPopulationData[turnIndex];
    }

    int numAgents = largestPopulation;

    RenderChartBackground();
    RenderXAxisOverTurnsInSimulation();
    RenderYAxisOverPopulationSizeByTurnInSimulation();
    RenderGrowthRateOverTurns();

    std::string numAgentsString = Stringf("Number of Agents: %i", numAgents);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x - (int)(TILE_HEIGHT / 1.5f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 1.5f), numAgentsString.c_str(), Rgba::WHITE);
    std::string numTurnsString = Stringf("Number of Turns: %i", numTurns);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)topRightOfGraph.x - (int)(TILE_HEIGHT * 12.0f), (int)bottomLeftOfGraph.y + (int)(TILE_HEIGHT * 3.0f), (int)(TILE_HEIGHT * 1.5f), numTurnsString.c_str(), Rgba::WHITE);
    std::string titleOfGraphString = "Population Growth Over Time";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderResourcesUsedOverTurnsChart() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numTurns = m_theWorld->m_currentTurn;

    int largestAmountOfResources = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        if (m_theWorld->m_currentNumberOfResourcesPerTurn[turnIndex] > largestAmountOfResources)
            largestAmountOfResources = m_theWorld->m_currentNumberOfResourcesPerTurn[turnIndex];
    }

    int numResources = largestAmountOfResources;

    RenderChartBackground();
    RenderXAxisOverTurnsInSimulation();
    RenderYAxisOverNumberOfResourcesByTurnInSimulation();
    RenderResourcesUsedRateOverTurns();

    std::string numResourcesString = Stringf("Number of Resources: %i", numResources);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x - (int)(TILE_HEIGHT / 1.5f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 1.5f), numResourcesString.c_str(), Rgba::WHITE);
    std::string numTurnsString = Stringf("Number of Turns: %i", numTurns);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)topRightOfGraph.x - (int)(TILE_HEIGHT * 12.0f), (int)bottomLeftOfGraph.y + (int)(TILE_HEIGHT * 3.0f), (int)(TILE_HEIGHT * 1.5f), numTurnsString.c_str(), Rgba::WHITE);
    std::string titleOfGraphString = "Resources Used Over Time";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderNumberOfIndividualVSGroupGoalsByDayChart() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numDays = m_theWorld->m_currentDay;

    int largestNumberOfGoals = 0;
    for (int dayIndex = 0; dayIndex < numDays; dayIndex++)
    {
        if (m_theWorld->m_dayAndIndividualGoalsChosen[dayIndex] > largestNumberOfGoals)
            largestNumberOfGoals = m_theWorld->m_dayAndIndividualGoalsChosen[dayIndex];

        if (m_theWorld->m_dayAndGroupGoalsChosen[dayIndex] > largestNumberOfGoals)
            largestNumberOfGoals = m_theWorld->m_dayAndGroupGoalsChosen[dayIndex];
    }

    int numGoals = largestNumberOfGoals;

    RenderChartBackground();
    RenderXAxisOverDaysInSimulation();
    RenderYAxisOverNumberOfGoalsByDayInSimulation();
    RenderIndividualAndGroupGoalsBarOverDaysGraph();

    std::string numGoalsString = Stringf("Number of Goals: %i", numGoals);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x - (int)(TILE_HEIGHT / 1.5f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 1.5f), numGoalsString.c_str(), Rgba::WHITE);
    std::string numDaysString = Stringf("Number of Days: %i", numDays);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)topRightOfGraph.x - (int)(TILE_HEIGHT * 12.0f), (int)bottomLeftOfGraph.y + (int)(TILE_HEIGHT * 3.0f), (int)(TILE_HEIGHT * 1.5f), numDaysString.c_str(), Rgba::WHITE);
    std::string titleOfGraphString = "Individual Goals vs Group Goals";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderIndividualVSGroupLegend() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);
    
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.5f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT), (int)(TILE_HEIGHT * 1.3f), "Individual", Rgba::BLACK);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.55f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 1.05f), (int)(TILE_HEIGHT * 1.3f), "Individual", Rgba::LIGHT_BLUE);
    
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.5f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 2.0f), (int)(TILE_HEIGHT * 1.3f), "Group", Rgba::BLACK);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.55f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 2.05f), (int)(TILE_HEIGHT * 1.3f), "Group", Rgba::PURPLE);
}

//*******************************************************************
void Game::RenderNumberOfIndividualVSGroupGoalsByTurnChart() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numTurns = m_theWorld->m_currentTurn + 1;

    int largestNumberOfGoals = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        int currentNumberOfGoals = m_theWorld->m_turnAndIndividualGoalsChosen[turnIndex] + m_theWorld->m_turnAndGroupGoalsChosen[turnIndex];
        if (currentNumberOfGoals > largestNumberOfGoals)
            largestNumberOfGoals = currentNumberOfGoals;
    }

    int numGoals = largestNumberOfGoals;

    RenderChartBackground();
    RenderXAxisOverTurnsInSimulation();
    RenderYAxisOverNumberOfGoalsByTurnInSimulation();
    RenderIndividualAndGroupGoalsBarOverTurnsGraph();
    RenderIndividualVSGroupLegend();

    std::string numGoalsString = Stringf("Number of Goals: %i", numGoals);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x - (int)(TILE_HEIGHT / 1.5f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 1.5f), numGoalsString.c_str(), Rgba::WHITE);
    std::string numDaysString = Stringf("Number of Turns: %i", numTurns - 1);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)topRightOfGraph.x - (int)(TILE_HEIGHT * 12.0f), (int)bottomLeftOfGraph.y + (int)(TILE_HEIGHT * 3.0f), (int)(TILE_HEIGHT * 1.5f), numDaysString.c_str(), Rgba::WHITE);
    std::string titleOfGraphString = "Individual Goals vs Group Goals";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderTypesOfGoalsLegend() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.5f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT), (int)(TILE_HEIGHT * 1.3f), "Water", Rgba::BLACK);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.55f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 1.05f), (int)(TILE_HEIGHT * 1.3f), "Water", Rgba::DARK_BLUE);

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.5f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 2.0f), (int)(TILE_HEIGHT * 1.3f), "Food", Rgba::BLACK);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.55f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 2.05f), (int)(TILE_HEIGHT * 1.3f), "Food", Rgba::DARK_RED);

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.5f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 3.0f), (int)(TILE_HEIGHT * 1.3f), "Shelter", Rgba::BLACK);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.55f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 3.05f), (int)(TILE_HEIGHT * 1.3f), "Shelter", Rgba::PLUM);

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.5f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 4.0f), (int)(TILE_HEIGHT * 1.3f), "Protection", Rgba::BLACK);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.55f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 4.05f), (int)(TILE_HEIGHT * 1.3f), "Protection", Rgba::DARK_GREEN);

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.5f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 5.0f), (int)(TILE_HEIGHT * 1.3f), "Reproduction", Rgba::BLACK);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.55f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 5.05f), (int)(TILE_HEIGHT * 1.3f), "Reproduction", Rgba::ORANGE_RED);

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.5f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 6.0f), (int)(TILE_HEIGHT * 1.3f), "Generic", Rgba::BLACK);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(chartDisplacementW * 1.55f), (int)topRightOfGraph.y - (int)(TILE_HEIGHT * 6.05f), (int)(TILE_HEIGHT * 1.3f), "Generic", Rgba::WHITE);
}

//*******************************************************************
void Game::RenderNumberOfTypesGoalsByTurnChart() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numTurns = m_theWorld->m_currentTurn;
    
    int largestNumberOfGoals = 0;
    for (int turnIndex = 0; turnIndex < numTurns; turnIndex++)
    {
        int currentNumberOfGoalsThisTurn = m_theWorld->m_turnAndWaterGoalsData[turnIndex] + m_theWorld->m_turnAndFoodGoalsData[turnIndex] + m_theWorld->m_turnAndShelterGoalsData[turnIndex] + m_theWorld->m_turnAndProtectionGoalsData[turnIndex] + m_theWorld->m_turnAndReproductionGoalsData[turnIndex] + m_theWorld->m_turnAndGenericGoalsData[turnIndex];

        if (currentNumberOfGoalsThisTurn > largestNumberOfGoals)
            largestNumberOfGoals = currentNumberOfGoalsThisTurn;
    }

    int numGoals = largestNumberOfGoals;

    RenderChartBackground();
    RenderXAxisOverTurnsInSimulation();
    RenderYAxisOverNumberOfTypesGoalsByTurnInSimulation();
    RenderTypesOfGoalsOverTurnsBarGraph();
    RenderTypesOfGoalsLegend();

    std::string numGoalsString = Stringf("Number of Goals: %i", numGoals);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x - (int)(TILE_HEIGHT / 1.5f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 1.5f), numGoalsString.c_str(), Rgba::WHITE);
    std::string numDaysString = Stringf("Number of Turns: %i", numTurns);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)topRightOfGraph.x - (int)(TILE_HEIGHT * 12.0f), (int)bottomLeftOfGraph.y + (int)(TILE_HEIGHT * 3.0f), (int)(TILE_HEIGHT * 1.5f), numDaysString.c_str(), Rgba::WHITE);
    std::string titleOfGraphString = "Types of Goals Chosen Each Turn";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderGrowthRateWithIndividualVSGroupGoalsOverDaysCharts() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    RenderChartBackground();
    RenderXAxisOverDaysInSimulation();
    RenderIndividualAndGroupGoalsBarOverDaysGraph();
    RenderGrowthRateOverDays();
    RenderIndividualVSGroupLegend();

    std::string titleOfGraphString = "Population Growth and Goals Chosen";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderGrowthRateWithTypesOfGoalsOverTurnsCharts() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    RenderChartBackground();
    RenderXAxisOverTurnsInSimulation();
    RenderYAxisOverPopulationSizeByTurnInSimulation();
    RenderTypesOfGoalsOverTurnsBarGraph();
    RenderGrowthRateOverTurns();
    RenderTypesOfGoalsLegend();

    std::string titleOfGraphString = "Population Growth and Types of Goals Chosen";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderGrowthRateWithIndividualVSGroupGoalsOverTurnsCharts() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    RenderChartBackground();
    RenderXAxisOverTurnsInSimulation();
    RenderYAxisOverPopulationSizeByTurnInSimulation();
    RenderIndividualAndGroupGoalsBarOverTurnsGraph();
    RenderGrowthRateOverTurns();
    RenderIndividualVSGroupLegend();

    std::string titleOfGraphString = "Population Growth and Individual VS Groups Goals";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderGrowthRateWithResourcesUsedOverTurnsCharts() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    RenderChartBackground();
    RenderXAxisOverTurnsInSimulation();
    RenderResourcesUsedRateOverTurns();
    RenderGrowthRateOverTurns();

    std::string titleOfGraphString = "Population Growth and Resources Used";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderWaterNeedsChart() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numDays = m_theWorld->m_currentDay;

    RenderChartBackground();
    RenderXAxisOverDaysInSimulation();
    RenderYAxisOverMaxNeedsByDayInSimulation();
    RenderWaterNeedsOverTime();

    std::string numDaysString = Stringf("Number of Days: %i", numDays);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)topRightOfGraph.x - (int)(TILE_HEIGHT * 12.0f), (int)bottomLeftOfGraph.y + (int)(TILE_HEIGHT * 3.0f), (int)(TILE_HEIGHT * 1.5f), numDaysString.c_str(), Rgba::WHITE);
    std::string titleOfGraphString = "Water Needs Over Time";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderShelterNeedsChart() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    int numDays = m_theWorld->m_currentDay;

    RenderChartBackground();
    RenderXAxisOverDaysInSimulation();
    RenderYAxisOverMaxNeedsByDayInSimulation();
    RenderShelterNeedsOverTime();

    std::string numDaysString = Stringf("Number of Days: %i", numDays);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)topRightOfGraph.x - (int)(TILE_HEIGHT * 12.0f), (int)bottomLeftOfGraph.y + (int)(TILE_HEIGHT * 3.0f), (int)(TILE_HEIGHT * 1.5f), numDaysString.c_str(), Rgba::WHITE);
    std::string titleOfGraphString = "Shelter Needs Over Time";
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)bottomLeftOfGraph.x + (int)(TILE_HEIGHT * 20.0f), (int)topRightOfGraph.y + (int)(TILE_HEIGHT * 1.5f), (int)(TILE_HEIGHT * 3.0f), titleOfGraphString.c_str(), Rgba::WHITE);
}

//*******************************************************************
void Game::RenderAgentsPastGoalsDebugText() const
{
    RenderChartBackground();
    RenderAgentsPastGoals();
}

//*******************************************************************
void Game::RenderAgentsDeathsDebugText() const
{
    RenderChartBackground();
    RenderAgentsDeaths();
}

//*******************************************************************
void Game::RenderAgentsPastGoals() const
{
    for (int agentIndex = 0; agentIndex < MAX_NUM_AGENTS_IN_DEBUG_TEXT; agentIndex++)
    {
        if ((agentIndex + m_agentToDebugIndex) > ((int)m_theWorld->m_agents.size() - 1))
            return;
        Agent* currentAgent = m_theWorld->m_agents[agentIndex + g_theGame->m_agentToDebugIndex];
        RenderAnAgentsPastGoals(currentAgent, agentIndex);
    }
}

//*******************************************************************
void Game::RenderAnAgentsPastGoals(Agent* agent, int screenWidthMultiplier) const
{
    float textWidthLocation = 1.0f + ((float)screenWidthMultiplier * (TILE_WIDTH * 22.0f));
    float heightAboveMap = SCREEN_HEIGHT * TILE_HEIGHT;
    float textHeightLocation = TILE_HEIGHT * 0.95f;
    float textHeight = TILE_HEIGHT * 0.95f;
    float textTab = TILE_WIDTH * 2.0f;
    float heightMultiplier = 26.0f;
    Rgba needBeingFulfilledColor = Rgba::YELLOW;

    std::string agentNameString = Stringf("Name: %c", agent->m_glyph);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, agentNameString.c_str(), Rgba::WHITE);
    heightMultiplier--;

    int goalsSize = MAX_NUM_GOALS_IN_DEBUG_TEXT;
    for (int goalIndex = 0; goalIndex < goalsSize; goalIndex++)
    {
        if ((goalIndex + m_goalsToDebugIndex) > ((int)agent->m_allGoalsChosen.size() - 1))
            return;

        Goal currentGoal = agent->m_allGoalsChosen[goalIndex + m_goalsToDebugIndex];

        Rgba goalColor = Rgba::WHITE;
        if (currentGoal.GetType().compare("water") == 0)
            goalColor = Rgba::LIGHT_BLUE;
        if (currentGoal.GetType().compare("food") == 0)
            goalColor = Rgba::RED;
        if (currentGoal.GetType().compare("shelter") == 0)
            goalColor = Rgba::PLUM;
        if (currentGoal.GetType().compare("protection") == 0)
            goalColor = Rgba::GREEN;
        if (currentGoal.GetType().compare("reproduction") == 0)
            goalColor = Rgba::ORANGE_RED;

        std::string currentGoalName = currentGoal.GetGoalName();
        std::string goalNameString = Stringf("[%i] %s", goalIndex + m_goalsToDebugIndex, currentGoalName.c_str());
        g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + (int)textTab, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, goalNameString.c_str(), goalColor);
        heightMultiplier--;
    }
}

//*******************************************************************
void Game::RenderAgentsDeaths() const
{
    float textWidthLocation = 1.0f; // + ((float)screenWidthMultiplier * (TILE_WIDTH * 22.0f));
    float heightAboveMap = SCREEN_HEIGHT * TILE_HEIGHT;
    float textHeightLocation = TILE_HEIGHT * 0.95f;
    float textHeight = TILE_HEIGHT * 0.95f;
    float heightMultiplier = 26.0f;

    for (int deadAgentIndex = 0; deadAgentIndex < (int)m_theWorld->m_agentsThatAreDead.size(); deadAgentIndex++)
    {
        char agentGlyph = m_theWorld->m_agentsThatAreDead[deadAgentIndex];
        std::string causeOfDeath = m_theWorld->m_agentsCauseOfDeath[deadAgentIndex];

        int screenWidthMultiplier = 0;
        if (deadAgentIndex >= MAX_NUM_GOALS_IN_DEBUG_TEXT)
            screenWidthMultiplier = (deadAgentIndex / MAX_NUM_GOALS_IN_DEBUG_TEXT) * (int)(TILE_WIDTH * 8.0f);
        if ((deadAgentIndex >= MAX_NUM_GOALS_IN_DEBUG_TEXT) && ((deadAgentIndex % MAX_NUM_GOALS_IN_DEBUG_TEXT) == 0))
            heightMultiplier = 26.0f;

        std::string agentDeathSTring = Stringf("[%i] %c: %s", deadAgentIndex, agentGlyph, causeOfDeath.c_str());
        g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, (int)textWidthLocation + screenWidthMultiplier, (int)heightAboveMap + ((int)(textHeightLocation * heightMultiplier)), (int)textHeight, agentDeathSTring.c_str(), Rgba::WHITE);
        heightMultiplier--;
    }
}

//*******************************************************************
void Game::RenderMainMenuSpace() const
{
    RenderSetup();

    g_theSimpleRenderer->DrawQuad(Vector3(ORHTO_WIDTH / 2.f, ORHTO_HEIGHT / 2.f, 0.0f), Vector3(ORHTO_WIDTH / 2.f, ORHTO_HEIGHT / 2.f, 0.0f), Rgba(125, 0, 50, 255));

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_fontTexture);
    g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
    g_theSimpleRenderer->DrawTextCenteredAndScaledToFitAABB2D(g_theSimpleRenderer->m_font, AABB2D(Vector2((ORHTO_WIDTH / 2) - 8, (ORHTO_HEIGHT / 2) + 5), 4, 4), "THESIS", Rgba(255, 255, 0, 255));
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, (int)ORHTO_HEIGHT - 1, 1, 1, "Press ESC to Exit Game", Rgba::WHITE);

    Rgba resumePlayingColor = Rgba::WHITE;
    if (m_theWorld == nullptr)
        resumePlayingColor = Rgba(75, 75, 75, 255);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, (int)ORHTO_HEIGHT - 2, 1, 1, "Press 1 to Resume Game", resumePlayingColor);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, (int)ORHTO_HEIGHT - 3, 1, 1, "Press 2 to Start New Game", Rgba::WHITE);
    g_theSimpleRenderer->DisableBlend();
}

//*******************************************************************
void Game::RenderGameSpace() const
{
    RenderSetup();
    //m_theWorld->Render();
    RenderHUD();
}

//*******************************************************************
void Game::RenderHUD() const
{
    /*int numberOfTurnsLeftToSolvePuzzle = m_theWorld->m_currentMap->m_totalNumTurnsToSolve - m_theWorld->m_currentMap->m_currentNumTurnsTowardsSolvingPuzzle;
    std::string numberOfTurnsLeftString = Stringf("#TURNS: %i", numberOfTurnsLeftToSolvePuzzle);

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_fontTexture);
    g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, (int)ORHTO_HEIGHT - 1, 1, 1, "Press ESC to Return to Main Menu", Rgba::WHITE);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, (int)ORHTO_HEIGHT - 2, 1, 1, "Activate All Pressure Plates Within Turns Remaining", Rgba::WHITE);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, 2, 1, 1, numberOfTurnsLeftString.c_str(), Rgba::WHITE);
    g_theSimpleRenderer->DisableBlend();*/
}

//*******************************************************************
void Game::RenderStatsScreenSpace() const
{
    /*RenderGameSpace();

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteSample);
    g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
    g_theSimpleRenderer->DrawQuad(Vector3(ORHTO_WIDTH / 2.f, ORHTO_HEIGHT / 2.f, 0.0f), Vector3(ORHTO_WIDTH / 2.f, ORHTO_HEIGHT / 2.f, 0.0f), Rgba(0, 0, 0, 200));
    g_theSimpleRenderer->DisableBlend();

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_fontTexture);
    g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);

    int textX = 5;
    int textY = (int)ORHTO_HEIGHT - 20;

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, textX, textY, "Stats Screen", Rgba::WHITE);

    textY -= 50;
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, textX, textY, "Player's Stats", Rgba::WHITE);
    textX += 25;
    textY -= 30;
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, textX, textY, Stringf("Current Health: %i", m_theWorld->m_currentMap->m_player->m_currentHealth).c_str(), Rgba::WHITE);
    textY -= 30;
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, textX, textY, Stringf("Damage: %i", m_theWorld->m_currentMap->m_player->m_damage).c_str(), Rgba::WHITE);
    textY -= 30;
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, textX, textY, Stringf("Defense: %i", m_theWorld->m_currentMap->m_player->m_defense).c_str(), Rgba::WHITE);

    g_theSimpleRenderer->DisableBlend();*/
}

//*******************************************************************
void Game::RenderGameOverSpace() const
{
    float chartDisplacementW = 2.0f * (float)TILE_WIDTH;
    float chartDisplacementH = 2.0f * (float)TILE_HEIGHT;
    Vector2 bottomLeftOfGraph = Vector2(chartDisplacementW, chartDisplacementH);
    Vector2 topRightOfGraph = Vector2(ORHTO_WIDTH - chartDisplacementW, ORHTO_HEIGHT - chartDisplacementH);

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteSample);
    g_theSimpleRenderer->DrawQuad(Vector2(), Vector2(ORHTO_WIDTH, ORHTO_HEIGHT), Rgba(0, 0, 0, 200));

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_fontTexture);
    g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
    g_theSimpleRenderer->DrawTextCenteredAndScaledToFitAABB2D(g_theSimpleRenderer->m_font, AABB2D(Vector2(ORHTO_WIDTH / 2, ORHTO_HEIGHT / 2), (int)(TILE_WIDTH * 2.0f), (int)(TILE_HEIGHT * 2.0f)), "SIM OVER", Rgba::RED);
    g_theSimpleRenderer->DisableBlend();

    /*g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, (int)ORHTO_HEIGHT - 1, 1, 1, "Press ESC to Exit Game", Rgba::WHITE);
    Rgba resumePlayingColor = Rgba::WHITE;
    if (m_theWorld == nullptr)
        resumePlayingColor = Rgba(75, 75, 75, 255);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, (int)ORHTO_HEIGHT - 2, 1, 1, "Press 1 to Return to Main Menu", resumePlayingColor);
    g_theSimpleRenderer->DisableBlend();*/
}

//*******************************************************************
void Game::RenderVictorySpace() const
{
    g_theSimpleRenderer->matrix_data.view = Matrix4();

    g_theSimpleRenderer->SetRenderTarget(nullptr);
    g_theSimpleRenderer->ClearColor(Rgba(0, 0, 0, 255));
    g_theSimpleRenderer->ClearDepth();
    g_theSimpleRenderer->EnableDepthTest(false);
    g_theSimpleRenderer->SetViewport(0, 0, (unsigned int)g_theSimpleRenderer->m_output->GetWidth(), (unsigned int)g_theSimpleRenderer->m_output->GetHeight());

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteSample);
    SetGameDeveloperConsoleOrGameShader();

    g_theSimpleRenderer->SetSampler(g_theSimpleRenderer->m_pointSampler);
    g_theSimpleRenderer->SetOrthoProjection(Vector2(0.0f, 0.0f), Vector2(ORHTO_WIDTH, ORHTO_HEIGHT));
    g_theSimpleRenderer->DrawQuad(Vector3(ORHTO_WIDTH / 2.f, ORHTO_HEIGHT / 2.f, 0.0f), Vector3(ORHTO_WIDTH / 2.f, ORHTO_HEIGHT / 2.f, 0.0f), Rgba(255, 255, 255, 255));

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_fontTexture);
    g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
    g_theSimpleRenderer->DrawTextCenteredAndScaledToFitAABB2D(g_theSimpleRenderer->m_font, AABB2D(Vector2((ORHTO_WIDTH / 2) - 8, (ORHTO_HEIGHT / 2) + 5), 4, 4), "VICTORY", Rgba(0, 0, 255, 255));

    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, (int)ORHTO_HEIGHT - 1, 1, 1, "Press ESC to Exit Game", Rgba(0, 0, 0, 255));
    Rgba resumePlayingColor = Rgba::WHITE;
    if (m_theWorld == nullptr)
        resumePlayingColor = Rgba(75, 75, 75, 255);
    g_theSimpleRenderer->DrawText(g_theSimpleRenderer->m_font, 1, (int)ORHTO_HEIGHT - 2, 1, 1, "Press 1 to Return to Main Menu", Rgba(0, 0, 0, 255));
    g_theSimpleRenderer->DisableBlend();
}

//*******************************************************************
void Game::RenderDeveloperSpace() const
{
    g_theSimpleRenderer->matrix_data.view = Matrix4();

    g_theSimpleRenderer->SetRenderTarget(nullptr);
    g_theSimpleRenderer->ClearColor(Rgba(66, 134, 244, 255));
    g_theSimpleRenderer->ClearDepth();
    g_theSimpleRenderer->EnableDepthTest(false);
    g_theSimpleRenderer->SetViewport(0, 0, (unsigned int)g_theSimpleRenderer->m_output->GetWidth(), (unsigned int)g_theSimpleRenderer->m_output->GetHeight());

    g_theSimpleRenderer->SetTexture(g_theSimpleRenderer->m_whiteSample);
    g_theSimpleRenderer->SetSampler(g_theSimpleRenderer->m_pointSampler);

    g_theSimpleRenderer->SetOrthoProjection(Vector2(0.0f, 0.0f), Vector2(ORHTO_WIDTH, ORHTO_HEIGHT));


    g_theSimpleRenderer->DrawQuad(Vector3(ORHTO_WIDTH / 2.f, ORHTO_HEIGHT / 2.f, 0.0f), Vector3(ORHTO_WIDTH / 2.f, ORHTO_HEIGHT / 2.f, 0.0f), Rgba(255, 255, 255, 50));

    PrintTextInDeveloperConsole();
}

//*******************************************************************
void Game::SetCamera(Vector3 moveThisFrame, Vector3 rotateThisFrame)
{
    g_theSimpleRenderer->matrix_data.view.RotateDegreesAboutZ(rotateThisFrame.z);
    g_theSimpleRenderer->matrix_data.view.RotateDegreesAboutX(rotateThisFrame.x);
    g_theSimpleRenderer->matrix_data.view.RotateDegreesAboutY(rotateThisFrame.y);

    g_theSimpleRenderer->matrix_data.view.TranslateAndTranspose(moveThisFrame);

    g_theSimpleRenderer->matrix_cb->Update(g_theSimpleRenderer->m_context, &g_theSimpleRenderer->matrix_data);
}

//*******************************************************************
void Game::PrintTextInDeveloperConsole() const
{
    if (g_theApp->m_isDeveloperConsoleOpen)
    {
        g_theSimpleRenderer->EnableBlend(BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA);
        g_theSimpleRenderer->SetShader(g_theSimpleRenderer->shaderPrograms[0]);
        g_theConsole->RenderConsole();
        g_theSimpleRenderer->DisableBlend();
    }
}

//*******************************************************************
void Game::SetGameDeveloperConsoleOrGameShader() const
{
    if (g_theApp->m_isDeveloperConsoleOpen)
    {
        g_theSimpleRenderer->SetShader(g_theSimpleRenderer->shaderPrograms[11]);
    }
    else
    {
        g_theSimpleRenderer->SetShader(g_theSimpleRenderer->shaderProgram);
    }
}

//*******************************************************************
void Game::SetRandomScreenShake()
{
    float shakeX = GetRandomFloatInRange(-3.f, 3.f);
    float shakeY = GetRandomFloatInRange(-3.f, 3.f);
    m_screenShake = Vector2(shakeX, shakeY);

    XboxController xboxController = g_inputSystem->GetControllerState(0);
    xboxController.RumbleController(0.5f, 0.5f);
}

//*******************************************************************
void Game::SetScreenShakeToZero()
{
    g_theRenderer->Translate(-1.f * m_screenShake.x, -1.f * m_screenShake.y, 0.f);
    m_screenShake = Vector2();

    XboxController xboxController = g_inputSystem->GetControllerState(0);
    xboxController.RumbleController();
}

//*******************************************************************
void Game::PlaySound(const std::string& soundFileName)
{
    SoundID soundId = g_theAudioSystem->CreateOrGetSound(soundFileName);
    g_theAudioSystem->PlaySoundID(soundId);
}