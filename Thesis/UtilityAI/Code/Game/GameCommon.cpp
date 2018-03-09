#include "Game/GameCommon.hpp"

float ORHTO_WIDTH = 0.0f;
float ORHTO_HEIGHT = 0.0f;

float WINDOW_WIDTH = 0;
float WINDOW_HEIGHT = 0;
char const* WINDOW_TITLE = "";

float SCREEN_WIDTH = 0.0f;
float SCREEN_HEIGHT = 0.0f;
int MAX_NUM_TILES = 0;
int MAX_NUM_ROOMS = 0;

InputSystem* g_inputSystem = nullptr;
Renderer* g_theRenderer = nullptr;
Camera2D* g_camera2D = nullptr;
Game* g_theGame = nullptr;
AudioSystem* g_theAudioSystem = nullptr;
SimpleRenderer* g_theSimpleRenderer = nullptr;
Console* g_theConsole = nullptr;