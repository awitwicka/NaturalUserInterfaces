#pragma once

#include "mini_windowApplication.h"
#include "mini_dxDevice.h"

namespace mini
{
	class DxApplication : public WindowApplication
	{
	public:
		explicit DxApplication(HINSTANCE hInstance,
			int wndWidth = Window::m_defaultWindowWidth,
			int wndHeight = Window::m_defaultWindowHeight,
			std::wstring wndTitle = L"DirectX Window")
			: WindowApplication(hInstance, wndWidth, wndHeight, wndTitle) { }
		virtual ~DxApplication() { DxApplication::Shutdown(); }

	protected:
		bool Initialize() override;
		int MainLoop() override;
		void Shutdown() override;

		virtual void Update(float dt) { }
		virtual void Render();

		DxDevice m_device;
		dx_ptr<ID3D11DeviceContext> m_context;
		dx_ptr<IDXGISwapChain> m_swapChain;
		dx_ptr<ID3D11RenderTargetView> m_backBuffer;
		dx_ptr<ID3D11DepthStencilView> m_depthBuffer;


	private:
		void CreateDevice(int wndWidth, int wndHeight);
		void CreateBackBuffer(int wndWidth, int wndHeight);

		dx_ptr<ID3D11Texture2D> m_backTexture, m_depthTexture;
	};
}