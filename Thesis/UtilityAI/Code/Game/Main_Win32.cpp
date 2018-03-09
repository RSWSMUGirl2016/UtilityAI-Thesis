#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>


#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include <string>
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vertex3.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/Console.hpp"

//*******************************************************************
const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const int WINDOW_PHYSICAL_WIDTH = 1600;
const int WINDOW_PHYSICAL_HEIGHT = 900;

//*******************************************************************
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;
const char* APP_NAME = "THESIS";

//*******************************************************************
bool WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
    UNUSED(lParam);
    UNUSED(windowHandle);

    unsigned char keyCode = (unsigned char)wParam;
    switch (wmMessageCode)
    {
    case WM_CLOSE:
    case WM_DESTROY:
    case WM_QUIT:
    {
        g_theApp->OnExitRequested();
        return true;
    }
    case WM_SETFOCUS:
    {
        return true;
    }

    case WM_KILLFOCUS:
    {
        return true;
    }

    case WM_KEYDOWN:
    {
        g_theApp->OnKeyDown(keyCode);
        g_theApp->m_keyCode = keyCode;
        g_theApp->m_isKeyDown = true;
        return true;
    }

    case WM_KEYUP:
    {
        g_theApp->OnKeyUp(keyCode);
        g_theApp->m_isKeyDown = false;
        return true;
    }

    case WM_LBUTTONDOWN:
    {
        g_theApp->OnKeyDown(MOUSE_LEFTBTTN);
        return true;
    }

    case WM_RBUTTONDOWN:
    {
        g_theApp->OnKeyDown(MOUSE_RIGHTBTTN);
        return true;
    }

    case WM_LBUTTONUP:
    {
        g_theApp->OnKeyUp(MOUSE_LEFTBTTN);
        return true;
    }

    case WM_RBUTTONUP:
    {
        g_theApp->OnKeyUp(MOUSE_RIGHTBTTN);
        return true;
    }

    }


    return false;
}

//*******************************************************************
void Initialize(HINSTANCE applicationInstanceHandle)
{
    UNUSED(applicationInstanceHandle);
    SetProcessDPIAware();

    g_theApp = new App();

    ORHTO_WIDTH = 1600.0f;
    ORHTO_HEIGHT = 900.0f;
    WINDOW_WIDTH = ORHTO_WIDTH;
    WINDOW_HEIGHT = ORHTO_HEIGHT;
    WINDOW_TITLE = "THESIS - UTILITY AI";

    SCREEN_WIDTH = (ORHTO_WIDTH / 2.0f) / TILE_WIDTH;
    SCREEN_HEIGHT = (ORHTO_HEIGHT / 2.0f) / TILE_HEIGHT;
    MAX_NUM_TILES = (int)SCREEN_WIDTH * (int)SCREEN_HEIGHT;

    g_theSimpleRenderer->SetUp(WindowsMessageHandlingProcedure);
    g_theConsole->m_cursorY = (int)WINDOW_HEIGHT - g_theSimpleRenderer->m_font->m_lineHeight;

    g_theGame->SetupSimulationSpace();
}

//*******************************************************************
void Shutdown()
{
    delete g_theApp;
    g_theApp = nullptr;
}

//*******************************************************************
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
	UNUSED(commandLineString);
	Initialize(applicationInstanceHandle);

	while (!g_theApp->IsQuitting())
	{
        g_theApp->GetOutput()->m_window->ProcessMessages();
        g_theApp->RunFrame();
	}

	Shutdown();
	return 0;
}

