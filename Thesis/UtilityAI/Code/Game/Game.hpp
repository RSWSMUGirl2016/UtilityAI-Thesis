#pragma once
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Console.hpp"
#include "Game/Camera3D.hpp"
#include "Game/World.hpp"
#include "Engine/RHI/Texture2D.hpp"

const int NUM_TURNS_FORWARDED = 48;
const int MAX_NUM_GOALS_IN_DEBUG_TEXT = 50;

enum GameState
{
    MAIN_MENU,
    PLAY_MODE,
    STATS_SCREEN,
    GAME_OVER,
    VICTORY,
    NUM_STATES
};

class Game
{
public:
	Vector2 m_screenShake;
    World* m_theWorld;
    std::string m_mapName;

public:
    int m_currentTexture;
    int m_currentSahder;
    Camera3D m_camera;
    float m_currentModelDegreeYRotation;
    GameState m_currentGameState;

private:
    float m_currentTurnTime;
    bool m_amIDebugStepping;
    float m_secondsPerTurn;

    bool m_doIRenderCharts;
    bool m_doIRenderGrowthRateByTurnChart;
    bool m_doIRenderGrowthRateByDayChart;
    bool m_doIRenderNumberOfTypesGoalsByTurnChart;
    bool m_doIRenderNumberOfInvidiualVSGroupGoalsByDayChart;
    bool m_doIRenderGrowthRateAndIndividualVSGroupByDay;
    bool m_doIRenderWaterNeedsByDayChart;
    bool m_doIRenderShelterNeedsByDayChart;
    bool m_doIRenderGrowthRateAndTypesOfGoalsByTurnChart;
    bool m_doIRenderNumberOfInvidiualVSGroupGoalsByTurnChart;
    bool m_doIRenderGrowthRateAndInvidiualVSGroupGoalsByTurnChart;
    bool m_doIRenderNumberOfResourcesByTurnChart;
    bool m_doIRenderAgentsPastGoals;
    bool m_doIRenderGrowthRateAndResourcesUsedByTurnChart;
    bool m_doIRenderAgentsDeathInfo;

public:
    bool m_doIRenderGoalScoresOfAgent;
    int m_agentToDebugIndex;
    int m_indexOfChartToRender;
    int m_goalsToDebugIndex;

public:
	Game();
	~Game();
public:
    void SetupSimulationSpace();
    void SetWhichChartToRender();
    bool IsDeveloperConsoleOpen();
    void Update(float deltaSeconds);
    void UpdateMainMenuChoices(float deltaSeconds);
    void UpdatePlaySpace(float deltaSeconds);
    void UpdateStatsScreenSpace(float deltaSeconds);
    void UpdateGameOverChoices(float deltaSeconds);
    void UpdateVictoryChoices(float deltaSeconds);
    Vector3 UpdatePlayerMouseLook(float deltaSeconds);
    void UpdatePlayerKeyboardMovement(float deltaSeconds);
    void UpdateDeveloperConsole();
    void UpdateGameConsole();
	void TestingButtons(float deltaSeconds);

public:
	void Render() const;
    void RenderSetup() const;
    void RenderSimulationState() const;
    void RenderDayAndTurn() const;
    void RenderCurrentPopulation() const;

    void RenderChartBackground() const;
    void RenderXAxisOverDaysInSimulation() const;
    void RenderXAxisOverTurnsInSimulation() const;
    void RenderYAxisOverPopulationSizeByDayInSimulation() const;
    void RenderYAxisOverPopulationSizeByTurnInSimulation() const;
    void RenderYAxisOverNumberOfGoalsByDayInSimulation() const;
    void RenderYAxisOverNumberOfGoalsByTurnInSimulation() const;
    void RenderYAxisOverNumberOfResourcesByTurnInSimulation() const;
    void RenderYAxisOverMaxNeedsByDayInSimulation() const;
    void RenderYAxisOverNumberOfTypesGoalsByTurnInSimulation() const;
    void RenderGrowthRateOverDays() const;
    void RenderGrowthRateOverTurns() const;
    void RenderIndividualAndGroupGoalsBarOverDaysGraph() const;
    void RenderIndividualAndGroupGoalsBarOverTurnsGraph() const;
    void RenderTypesOfGoalsOverTurnsBarGraph() const;
    void RenderWaterNeedsOverTime() const;
    void RenderShelterNeedsOverTime() const;
    void RenderResourcesUsedRateOverTurns() const;
    void RenderGrowthRateOverDaysChart() const;
    void RenderGrowthRateOverTurnsChart() const;
    void RenderResourcesUsedOverTurnsChart() const;
    void RenderNumberOfIndividualVSGroupGoalsByDayChart() const;
    void RenderIndividualVSGroupLegend() const;
    void RenderNumberOfIndividualVSGroupGoalsByTurnChart() const;
    void RenderTypesOfGoalsLegend() const;
    void RenderNumberOfTypesGoalsByTurnChart() const;
    void RenderGrowthRateWithIndividualVSGroupGoalsOverDaysCharts() const;
    void RenderGrowthRateWithTypesOfGoalsOverTurnsCharts() const;
    void RenderGrowthRateWithIndividualVSGroupGoalsOverTurnsCharts() const;
    void RenderGrowthRateWithResourcesUsedOverTurnsCharts() const;
    void RenderWaterNeedsChart() const;
    void RenderShelterNeedsChart() const;
    void RenderAgentsPastGoalsDebugText() const;
    void RenderAgentsDeathsDebugText() const;
    void RenderAgentsPastGoals() const;
    void Game::RenderAnAgentsPastGoals(Agent* agent, int screenWidthMultiplier) const;
    void RenderAgentsDeaths() const;

public:
    void RenderMainMenuSpace() const;
    void RenderGameSpace() const;
    void RenderHUD() const;
    void RenderStatsScreenSpace() const;
    void RenderGameOverSpace() const;
    void RenderVictorySpace() const;
    void RenderDeveloperSpace() const;


public:
    void SetCamera(Vector3 moveThisFrame, Vector3 rotateThisFrame);
    void PrintTextInDeveloperConsole() const;
    void SetGameDeveloperConsoleOrGameShader() const;


public:
    void SetRandomScreenShake();
	void SetScreenShakeToZero();
	void PlaySound(const std::string& soundFileName);
};


