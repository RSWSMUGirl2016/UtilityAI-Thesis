#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Camera2D.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Game.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Window.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"


const float MIN_FRAMES_PER_SECOND = 10.f; // Game time will slow down beneath this frame rate
const float TARGET_FRAMES_PER_SECOND = 60.f;
const float TARGET_SECONDS_PER_FRAME = (1.f / TARGET_FRAMES_PER_SECOND);
const float MAX_FRAMES_PER_SECOND = 60.f;
const float MIN_SECONDS_PER_FRAME = (1.f / MAX_FRAMES_PER_SECOND);
const float MAX_SECONDS_PER_FRAME = (1.f / MIN_FRAMES_PER_SECOND);


const float MAX_SECONDS_PER_TURN = 0.05f;
const float MIN_SECONDS_PER_TURN = 0.2f;

extern float ORHTO_WIDTH;
extern float ORHTO_HEIGHT;

extern float WINDOW_WIDTH;
extern float WINDOW_HEIGHT;
extern char const* WINDOW_TITLE;

const float TILE_WIDTH = 18.0f;
const float TILE_HEIGHT = TILE_WIDTH;

extern float SCREEN_WIDTH; 
extern float SCREEN_HEIGHT;
extern int MAX_NUM_TILES;
extern int MAX_NUM_ROOMS;

extern InputSystem* g_inputSystem;
extern Renderer* g_theRenderer;
extern Camera2D* g_camera2D;
extern Game* g_theGame;
extern AudioSystem* g_theAudioSystem;
extern SimpleRenderer* g_theSimpleRenderer;
extern Console* g_theConsole;

//*******************************************************************
// Credit to Squirrel Eiserloh
#define UNUSED(x) (void)(x);