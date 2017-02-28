#pragma once

#include "mini_window.h"

namespace mini
{
	class WindowApplication : public IWindowMessageHandler
	{
	public:
		explicit WindowApplication(HINSTANCE hInstance,
			int wndWidht = Window::m_defaultWindowWidth,
			int wndHeight = Window::m_defaultWindowHeight,
			std::wstring wndTitle = L"Default Window");
		virtual ~WindowApplication();

		int Run(int cmdShow = SW_SHOWNORMAL);
		HINSTANCE getHandle() const { return m_hInstance; }
		bool ProcessMessage(WindowMessage& msg) override { return false; }

	protected:
		virtual bool Initialize() { return true; }
		virtual int MainLoop();
		virtual void Shutdown() { };

		Window m_window;

	private:
		HINSTANCE m_hInstance;
	};
}