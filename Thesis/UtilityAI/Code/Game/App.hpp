#pragma once
#include "Engine/RHI/RHI.hpp"
#include "Engine/Renderer/SimpleRenderer.hpp"
#include "Engine/RHI/ConstantBuffer.hpp"
#include "Engine/Math/Vector3.hpp"

class App
{
public:
	App();
	~App();
public:
	void RunFrame();
    RHIOutput* GetOutput();
	void OnKeyDown(int keyCode);
	void OnKeyUp(int keyCode);
	void OnExitRequested();
	bool IsQuitting();
private:
	void Update(float deltaSeconds);
	void Render() const;
public:
	float GetDeltaSeconds();
private:
	bool m_isQuitting;
public:
    int m_keyCode;
    bool m_isKeyDown;
    bool m_wasEnterPressed;
    bool m_isDeveloperConsoleOpen;
    bool m_didDeveloperWindowJustOpen;
};

extern App* g_theApp;