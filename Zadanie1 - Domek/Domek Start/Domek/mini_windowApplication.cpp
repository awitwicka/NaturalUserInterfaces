#include "mini_windowApplication.h"

using namespace std;
using namespace mini;

WindowApplication::WindowApplication(HINSTANCE hInstance, int wndWidht, int wndHeight,
									 wstring wndTitle)
									 : m_window(hInstance, wndWidht, wndHeight, wndTitle, this), m_hInstance(hInstance)
{ }

WindowApplication::~WindowApplication()
{
	WindowApplication::Shutdown();
}

int WindowApplication::MainLoop()
{
	MSG msg = { nullptr };
	while ( GetMessageW(&msg, nullptr, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return static_cast<int>(msg.wParam);
}

int WindowApplication::Run(int cmdShow)
{
	if (!Initialize())
		return -1;
	m_window.Show(cmdShow);
	auto r = MainLoop();
	Shutdown();
	return r;
}