#pragma once

#include "mini_sceneGraph.h"
#include "mini_camera.h"
#include "in_texturedEffect.h"
#include "mini_dxApplication.h"
#include "mini_collision.h"
#include "FW1\FW1FontWrapper.h"
#include "mini_material.h"
#include "mini_fpsCounter.h"

#define DIRECTINPUT_VERSION 0x0800
//#include <Dinput8.h>
#include <dinput.h>

namespace mini
{
	class INScene : public DxApplication
	{
	public:
		explicit INScene(HINSTANCE hInstance,
			int wndWidth = Window::m_defaultWindowWidth,
			int wndHeight = Window::m_defaultWindowHeight,
			std::wstring wndTitle = L"DirectX Window")
			: DxApplication(hInstance, wndWidth, wndHeight, wndTitle), m_camera(DirectX::XMFLOAT3(0, 1.6f, 0)),
			  m_doorNode(0), m_doorAngle(0), m_doorAngVel(0)
		{ }

	protected:
		IDirectInput8 *di;
		IDirectInputDevice8* pKeyboard = nullptr;
		IDirectInputDevice8* pMouse = nullptr;
		IDirectInputDevice8* pJoystick = nullptr;

		bool Initialize() override;
		void Render() override;
		void Shutdown() override;
		void Update(float dt) override;
		bool ProcessMessage(WindowMessage& msg) override;

	private:

		enum Action { MOVE_X, MOVE_Y, ROT_X, ROT_Y, DOOR };

		struct Control {
			Action action;
			int action_direction; // 1 or -1

			int ControlNo;
			//int value; //?
			Control(Action a, int dir, int no) { action = a; action_direction = dir; ControlNo = no; };

		};

		std::vector<Control> Controls;
		//flags
		bool launchSetup = true;
		bool launchKeyPick = false;
		bool joystickPicked = false;
		bool mousePicked = false;
		int keycount = 0;
		int prevIndex = -1;
		bool open_the_door = false;
		int joystick_count = 0;

		int up = 0;
		int back = 0;
		int right = 0;
		int left = 0;
		int door = 0;
		
		int vertical = 0;
		int horizontal = 0;
		bool captureMouse = false;
		int dx = 0;
		int dy = 0;
		int rotx = 0;
		int roty = 0;
		int prev_rotx = 0;
		int prev_roty = 0;
		int d_rotx = 0;
		int d_roty = 0;

		static BOOL CALLBACK StaticEnumCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
		BOOL EnumCallback(LPCDIDEVICEINSTANCE lpddi);

		int GetControl(DIJOYSTATE state_j);
		//int GetNormalizedControlValue(DIJOYSTATE state_j, int dir);

		bool GetDeviceState(IDirectInputDevice8* pDevice, unsigned int size, void* ptr);

		//Starts the door opening animation if the door isn't fully open
		void OpenDoor();
		//Starts the door closing animation if the door isn't fully closed
		void CloseDoor();
		//Toggles between the door opening and closing animation
		void ToggleDoor();
		//Moves the character forward by dx and right by dz in relation to the current camera orientation
		void MoveCharacter(float dx, float dz);
		//Checks if the camera is facing the door
		bool FacingDoor() const;
		//Returns the distance from the character to the door
		float DistanceToDoor() const;

		void InitializeInput();
		void UpdateDoor(float dt);
		void RenderText() const;
		void RenderMenu();

		std::unique_ptr<ConstantBuffer<DirectX::XMFLOAT4X4>> m_cbProj;
		std::unique_ptr<ConstantBuffer<DirectX::XMFLOAT4X4>> m_cbView;
		std::unique_ptr<ConstantBuffer<DirectX::XMFLOAT4X4, 2> > m_cbModel;
		std::unique_ptr<ConstantBuffer<Material::MaterialData>> m_cbMaterial;
		std::unique_ptr<TexturedEffect> m_texturedEffect;
		std::unique_ptr<SceneGraph> m_sceneGraph;
		dx_ptr<ID3D11InputLayout> m_layout;
		dx_ptr<ID3D11RasterizerState> m_rsState;
		dx_ptr<ID3D11SamplerState> m_sampler;
		
		dx_ptr<IFW1Factory> m_fontFactory;
		dx_ptr<IFW1FontWrapper> m_font;
		
		Camera m_camera;
		DirectX::XMFLOAT4X4 m_proj;
		FPSCounter m_counter;

		unsigned int m_doorNode;
		float m_doorAngle;
		float m_doorAngVel;
		DirectX::XMFLOAT4X4 m_doorTransform;
		CollisionEngine m_collisions;
	};
}
