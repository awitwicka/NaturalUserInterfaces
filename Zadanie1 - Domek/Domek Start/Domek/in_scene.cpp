#include "in_scene.h"
#include "mini_xfileLoader.h"
#include "mini_effectLoader.h"
#include <sstream>
#include "mini_exceptions.h"
#include <Windowsx.h>

using namespace std;
using namespace mini;
using namespace utils;
using namespace DirectX;

#define AXIS_LX_R 101
#define AXIS_LX_L 102
#define AXIS_LY_U 103 //zle
#define AXIS_LY_D 104 //zle
#define AXIS_RZ_R 105
#define AXIS_RZ_L 106
#define SLIDER 107
#define POV_U 108
#define POV_D 109
#define POV_R 110
#define POV_L 111
//alternatively AXIS_LX + value



bool INScene::ProcessMessage(WindowMessage& msg)
{
	/*Process windows messages here*/
	/*if message was processed, return true and set value to msg.result*/

	d_rotx = 0;
	d_roty = 0;


	bool result = false;

	if (msg.message == WM_KEYDOWN) 
	{
		switch (msg.wParam)
		{
		case 'W':
			vertical = 1;
			break;
		case 'S':
			vertical = -1;
			break;
		case 'A':
			horizontal = -1;
			break;
		case 'D':
			horizontal = 1;
			break;
		case 'E':
			if (FacingDoor() && DistanceToDoor() <= 1) {
				ToggleDoor();
			}
			//default

		}
		result = true;
	}
	if (msg.message == WM_KEYUP)
	{
		switch (msg.wParam)
		{
		case 'W':
			vertical = 0;
			break;
		case 'S':
			vertical = 0;
			break;
		case 'A':
			horizontal = 0;
			break;
		case 'D':
			horizontal = 0;
			break;
			//default:

		}
		result = true;
	}

	switch (msg.message)
	{
	case WM_LBUTTONDOWN: //setcapture
		SetCapture(m_window.getHandle());
		captureMouse = true;
		prev_rotx = GET_X_LPARAM(msg.lParam);
		prev_roty = GET_X_LPARAM(msg.lParam);
		break;
	case WM_LBUTTONUP: //release
		ReleaseCapture();
		captureMouse = false;
		break;
	case WM_MOUSEMOVE: //ruszaj kamera jesli klawisz wciscniety myszy lewy
		if (captureMouse) {
			rotx = GET_X_LPARAM(msg.lParam);
			roty = GET_Y_LPARAM(msg.lParam);

			d_rotx = prev_rotx - rotx;
			d_roty = prev_roty - roty;

			prev_rotx = rotx;
			prev_roty = roty;
		
		}
		break;
	//default:
		
	}
		
	
	msg.result = 0;
	return result;
}

void INScene::InitializeInput()
{
	/*Initialize Direct Input here*/
	HRESULT result;
	result = DirectInput8Create(getHandle(), DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&di), nullptr);
	//mouse
	result = di->CreateDevice(GUID_SysMouse, &pMouse, nullptr);
	result = pMouse->SetDataFormat(&c_dfDIMouse);
	result = pMouse->SetCooperativeLevel(m_window.getHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	result = pMouse->Acquire();
	//keyboatd
	result = di->CreateDevice(GUID_SysKeyboard, &pKeyboard, nullptr);
	result = pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	result = pKeyboard->SetCooperativeLevel(m_window.getHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	result = pKeyboard->Acquire();
	//joystick
	result = di->EnumDevices(DI8DEVCLASS_GAMECTRL, StaticEnumCallback, this, DIEDFL_ATTACHEDONLY);
	
}

BOOL CALLBACK INScene::StaticEnumCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	//INScene* pThis = (INScene*)pvRef;
	//return pThis->EnumCallback(lpddi);

	return reinterpret_cast<INScene*>(pvRef)->EnumCallback(lpddi);// , pvRef);
}

BOOL INScene::EnumCallback(LPCDIDEVICEINSTANCE lpddi)
{
	if (joystick_count != 0) {
		joystick_count++;
		return DIENUM_CONTINUE;
	}
	HRESULT result;
	result = di->CreateDevice(lpddi->guidInstance, &pJoystick, nullptr);
	if (pJoystick != nullptr) {
		result = pJoystick->SetDataFormat(&c_dfDIJoystick);
		result = pJoystick->SetCooperativeLevel(m_window.getHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		result = pJoystick->Acquire();
	}
	joystick_count++;
	return DIENUM_CONTINUE;
}



void INScene::Shutdown()
{
	/*Release Direct Input resources here*/
	pKeyboard->Unacquire(); 
	pKeyboard->Release();
	pMouse->Unacquire();
	pMouse->Release();
	if (pJoystick != nullptr) {
		pJoystick->Unacquire();
		pJoystick->Release();
	}
	di->Release();
	
	m_font.reset();
	m_fontFactory.reset();
	
	m_sampler.reset();
	m_texturedEffect.reset();
	m_cbProj.reset();
	m_cbView.reset();
	m_cbModel.reset();
	m_sceneGraph.reset();
	DxApplication::Shutdown();
}

void INScene::Update(float dt)
{
	/*proccess Direct Input here*/
	dx = 0;
	dy = 0;
	d_rotx = 0;
	d_roty = 0;
	DIMOUSESTATE state_m;
	if (GetDeviceState(pMouse, sizeof(DIMOUSESTATE), &state_m))
	{
		/*if (state_m.rgbButtons[0] > 0)
		{
			d_rotx = state_m.lX;
			d_roty = state_m.lY;
		}*/
		if (mousePicked && launchSetup == false && launchKeyPick == false) {
			if (state_m.rgbButtons[0] > 0)
			{
				d_rotx = state_m.lX;
				d_roty = state_m.lY;
			}
		}

	}
	BYTE state_k[256];
	if (GetDeviceState(pKeyboard, sizeof(BYTE) * 256, state_k))
	{
		/*if (state_k[DIK_W] > 0)
		dy = state_k[DIK_W] / 20;
		if (state_k[DIK_S] > 0)
		dy = -state_k[DIK_S] / 20;
		if (state_k[DIK_A] > 0)
		dx = -state_k[DIK_A] / 20;
		if (state_k[DIK_D] > 0)
		dx = state_k[DIK_D] / 20;
		if (state_k[DIK_E] > 0) {
		if (FacingDoor() && DistanceToDoor() <= 1 && !((m_doorAngVel > 0 && m_doorAngle < XM_PIDIV2) || (m_doorAngVel < 0 && m_doorAngle > 0)))
		ToggleDoor();
		}*/

		if (mousePicked && launchSetup == false && launchKeyPick == false) {
			if (state_k[up] > 0)
				dy = state_k[up] / 20;
			if (state_k[back] > 0)
				dy = -state_k[back] / 20;
			if (state_k[left] > 0)
				dx = -state_k[left] / 20;
			if (state_k[right] > 0)
				dx = state_k[right] / 20;
			if (state_k[door] > 0) {
				if (FacingDoor() && DistanceToDoor() <= 1 && !((m_doorAngVel > 0 && m_doorAngle < XM_PIDIV2) || (m_doorAngVel < 0 && m_doorAngle > 0)))
					ToggleDoor();
			}
		}
		if (launchSetup) {
			if (state_k[DIK_1] > 0) {
				mousePicked = true;
				launchSetup = false;
				launchKeyPick = true;
				prevIndex = DIK_1;
			}
			if (state_k[DIK_2] > 0) {
				joystickPicked = true;
				launchSetup = false;
				launchKeyPick = true;
				prevIndex = DIK_2;
			}
		}
		if (launchKeyPick) {
			if (mousePicked) {
				int index = -1;
				for (int i = 0; i < 256; i++) {
					if (state_k[i] > 0)
						index = i;
				}
				if (index != -1 && prevIndex != index) {  
					if (keycount == 0 )
						up = index;
					else if (keycount == 1)
						back = index;
					else if (keycount == 2)
						right = index;
					else if (keycount == 3)
						left = index;
					else if (keycount == 4) {
						door = index;
						launchKeyPick = false;
					}
					keycount++;
					prevIndex = index;
				}
					
			}
		}	
		
	}
	DIJOYSTATE state_j;
	if (GetDeviceState(pJoystick, sizeof(DIJOYSTATE), &state_j)) {

		/*if ((state_j.lX - 32767) > e || (state_j.lX - 32767) < -e)
			dx = (state_j.lX - 32767) / 10000;
		if ((state_j.lY - 32767) > e || (state_j.lY - 32767) < -e)
			dy = -(state_j.lY - 32767) / 10000;
		if ((state_j.lRz - 32767) > e || (state_j.lRz - 32767) < -e)
			d_rotx = (state_j.lRz - 32767) / 4000;
		if (state_j.lRx != 0)
			d_rotx = state_j.lRx;
		if (state_j.lRy != 0)
			d_roty = state_j.lRy;
		if (state_j.rgbButtons[0] > 0) {
			if (FacingDoor() && DistanceToDoor() <= 1 && !((m_doorAngVel > 0 && m_doorAngle < XM_PIDIV2) || (m_doorAngVel < 0 && m_doorAngle > 0)))
				ToggleDoor();
		}*/

		int e = 1000;
		if (joystickPicked && launchSetup == false && launchKeyPick == false) {
			int index = GetControl(state_j); 
			int indexControl = -1;
			for (int i = 0; i < Controls.size(); i++) {
				if (Controls[i].ControlNo == index)
					indexControl = i;
			}
			if (indexControl != -1) {
				//int val = GetNormalizedControlValue(state_j, Controls[indexControl].action_direction); <- in case we want to have velocity, accel on pads
				int val = 2 * Controls[indexControl].action_direction;
				if (Controls[indexControl].action == MOVE_X) {
					dx = val;
				}
				if (Controls[indexControl].action == MOVE_Y) {
					dy = val;
				}
				if (Controls[indexControl].action == ROT_X) {
					d_rotx = val;
				}
				if (Controls[indexControl].action == ROT_Y) {
					d_roty = val;
				}
				if (Controls[indexControl].action == DOOR) {
					open_the_door = true; 
				}

			}
		}
		
		if (launchKeyPick && joystickPicked) {
			//launchKeyPick = false;

			int index = GetControl(state_j);
			if (index != -1 && prevIndex != index) {

				if (keycount == 0) //up
					Controls.push_back(Control(MOVE_Y, 1, index));
				else if (keycount == 1) //down
					Controls.push_back(Control(MOVE_Y, -1, index));
				else if (keycount == 2) //right
					Controls.push_back(Control(MOVE_X, 1, index));
				else if (keycount == 3) //left
					Controls.push_back(Control(MOVE_X, -1, index));
				else if (keycount == 4)
					Controls.push_back(Control(DOOR, 0, index));
				else if (keycount == 5)
					Controls.push_back(Control(ROT_Y, -1, index));
				else if (keycount == 6)
					Controls.push_back(Control(ROT_Y, 1, index));
				else if (keycount == 7)
					Controls.push_back(Control(ROT_X, 1, index));
				else if (keycount == 8) {
					Controls.push_back(Control(ROT_X, -1, index));
					launchKeyPick = false;
				}
				keycount++;
				prevIndex = index;
			}

		}
	}

	MoveCharacter(dx*dt, dy*dt);
	m_camera.Rotate(d_roty*0.002*3, d_rotx*0.002*3);
	if (open_the_door)
		if (FacingDoor() && DistanceToDoor() <= 1 && !((m_doorAngVel > 0 && m_doorAngle < XM_PIDIV2) || (m_doorAngVel < 0 && m_doorAngle > 0)))
			ToggleDoor();
	open_the_door = false;


	/*remove the following line to stop the initial camera animation*/
	//m_camera.Rotate(0.0f, dt*XM_PIDIV4);

	//normalize character moves
	//dx = horizontal;
	//dy = vertical;
	//MoveCharacter(dx*dt, dy*dt);
	//m_camera.Rotate(-d_roty*0.002*3, -d_rotx*0.002*3);
	
	m_counter.NextFrame(dt);
	UpdateDoor(dt);
}

int INScene::GetControl(DIJOYSTATE state_j) {
	int e = 20000;
	int e_slid = 10000;
	//AXIS
	if ((state_j.lX - 32767) > e || (state_j.lX - 32767) < -e) {
		if (state_j.lX < 32767)
			return AXIS_LX_L;
		if (state_j.lX > 32767)
			return AXIS_LX_R;
	}
	if ((state_j.lY - 32767) > e || (state_j.lY - 32767) < -e) {
		if (state_j.lY < 32767)
			return AXIS_LY_D;
		if (state_j.lY > 32767)
			return AXIS_LY_U;
	}
	if ((state_j.lRz - 32767) > e || (state_j.lRz - 32767) < -e) {
		if (state_j.lRz < 32767)
			return AXIS_RZ_L;
		if (state_j.lRz > 32767)
			return AXIS_RZ_R;
	}

	//POV
	if (state_j.rgdwPOV[0] == 0 || state_j.rgdwPOV[0] == 9000 || state_j.rgdwPOV[0] == 18000 || state_j.rgdwPOV[0] == 27000) {
		if (state_j.rgdwPOV[0] == 0)
			return POV_U;
		else if (state_j.rgdwPOV[0] == 9000)
			return POV_R;
		else if (state_j.rgdwPOV[0] == 18000)
			return POV_D;
		else if (state_j.rgdwPOV[0] == 27000)
			return POV_L;
	}
	//SLIDER
	if ((65535 - state_j.rglSlider[0]) > e_slid)
		return SLIDER;

	//BUTTONS
	int index = -1;
	for (int i = 0; i < 32; i++) {
		if (state_j.rgbButtons[i] > 0)
			index = i;
	}
	if (index != -1)
		return index;

	return -1;
}

//int INScene::GetNormalizedControlValue(DIJOYSTATE state_j, int dir) {
//	int e = 1000;
//	//AXIS
//	//hamski CLAMP screw u metoda
//	if ((state_j.lX - 32767) > e || (state_j.lX - 32767) < -e) {
//		if (state_j.lX < 32767)
//			return (32767 - state_j.lX) * dir;
//		if (state_j.lX > 32767)
//			return (state_j.lX - 32767) * dir;
//	}
//	if ((state_j.lY - 32767) > e || (state_j.lY - 32767) < -e) {
//		if (state_j.lY < 32767)
//			return (32767 - state_j.lY) * dir;
//		if (state_j.lY > 32767)
//			return (state_j.lY - 32767) * dir;
//	}
//	if ((state_j.lRz - 32767) > e || (state_j.lRz - 32767) < -e) {
//		if (state_j.lRz < 32767)
//			return (32767 - state_j.lRz) * dir;
//		if (state_j.lRz > 32767)
//			return (state_j.lRz - 32767) * dir;
//	}
//
//	//POV
//	if (state_j.rgdwPOV[0] == 0 || state_j.rgdwPOV[0] == 9000 || state_j.rgdwPOV[0] == 1800 || state_j.rgdwPOV[0] == 2700) {
//		if (state_j.rgdwPOV[0] == 0)
//			return POV_U;
//		else if (state_j.rgdwPOV[0] == 9000)
//			return POV_R;
//		else if (state_j.rgdwPOV[0] == 1800)
//			return POV_D;
//		else if (state_j.rgdwPOV[0] == 2700)
//			return POV_L;
//	}
//	//SLIDER
//	if (state_j.rglSlider != 0)
//		return SLIDER;
//
//	//BUTTONS
//	int index = -1;
//	for (int i = 0; i < 32; i++) {
//		if (state_j.rgbButtons[i] > 0)
//			index = i;
//	}
//	if (index != -1)
//		return index;
//
//	return -1;
//}

const unsigned int GET_STATE_RETRIES = 2; 
const unsigned int ACQUIRE_RETRIES = 2;
bool INScene::GetDeviceState(IDirectInputDevice8* pDevice, unsigned int size, void* ptr) {
	if (!pDevice) //m_device?
		return false; 
	for (int i = 0; i < GET_STATE_RETRIES; ++i) 
	{
		HRESULT result = pDevice->GetDeviceState(size, ptr); 
		if (SUCCEEDED(result)) 
			return true; 
		if (result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED)
			//throw INScene();
			return false;// ... //error! throw exeption 
			for (int j = 0; j < ACQUIRE_RETRIES; ++j) 
			{ 
				result = pDevice->Acquire(); 
				if (SUCCEEDED(result)) break; 
				if (result != DIERR_INPUTLOST && result != E_ACCESSDENIED)
					return false; //error! throw exeption 
			} 
	} 
	return false; 
} 

void INScene::RenderText() const
{
	wstringstream str;
	str << L"FPS: " << m_counter.getCount();
	m_font->DrawString(m_context.get(), str.str().c_str(), 20.0f, 10.0f, 10.0f, 0xff0099ff,
		FW1_RESTORESTATE|FW1_NOGEOMETRYSHADER);
	if (DistanceToDoor() < 1.0f && FacingDoor())
	{
		wstring prompt(L"(E) Otwórz/Zamknij");
		FW1_RECTF layout;
		auto rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
		auto width = rect.Right - rect.Left;
		auto height = rect.Bottom - rect.Top;
		auto clSize = m_window.getClientSize();
		m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width)/2, (clSize.cy - height)/2,
			0xff00ff99,  FW1_RESTORESTATE|FW1_NOGEOMETRYSHADER);
	}
	
}

void INScene::RenderMenu()
{
	wstringstream str;
	FW1_RECTF layout;
	FW1_RECTF rect;
	wstring prompt;
	float width;
	float height;
	SIZE clSize;

	if (launchSetup)
	{
		//str << L"FPS: " << m_counter.getCount();
		//m_font->DrawString(m_context.get(), str.str().c_str(), 20.0f, 10.0f, 10.0f, 0xff0099ff, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);

		if (pJoystick)
		{
			prompt.append(L"Joystick Detected!");
			rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
			width = rect.Right - rect.Left;
			height = rect.Bottom - rect.Top;
			clSize = m_window.getClientSize();
			m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);

			prompt.clear();
			prompt.append(L"Press 1 to use Mouse/Keyboard, 2 to use Joystick");
			rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
			width = rect.Right - rect.Left;
			height = rect.Bottom - rect.Top;
			clSize = m_window.getClientSize();
			m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2 + height + 5, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
		}
		else 
		{
			mousePicked = true;
			launchSetup = false;
			launchKeyPick = true;
		}
	}
	if (launchKeyPick) {
		if (mousePicked || joystickPicked)
		{
			if (keycount == 0) {
				prompt.clear();
				prompt.append(L"Pick Up Key");
				rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
				width = rect.Right - rect.Left;
				height = rect.Bottom - rect.Top;
				clSize = m_window.getClientSize();
				m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
			}
			else if (keycount == 1) {
				prompt.clear();
				prompt.append(L"Pick Back Key");
				rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
				width = rect.Right - rect.Left;
				height = rect.Bottom - rect.Top;
				clSize = m_window.getClientSize();
				m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
			}
			else if (keycount == 2) {
				prompt.clear();
				prompt.append(L"Pick Right Key");
				rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
				width = rect.Right - rect.Left;
				height = rect.Bottom - rect.Top;
				clSize = m_window.getClientSize();
				m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
			}
			else if (keycount == 3) {
				prompt.clear();
				prompt.append(L"Pick Left Key");
				rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
				width = rect.Right - rect.Left;
				height = rect.Bottom - rect.Top;
				clSize = m_window.getClientSize();
				m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
			}
			else if (keycount == 4) {
				prompt.clear();
				prompt.append(L"Pick Open Door Key");
				rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
				width = rect.Right - rect.Left;
				height = rect.Bottom - rect.Top;
				clSize = m_window.getClientSize();
				m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
			}
			if (joystickPicked) {
				//rotation
				if (keycount == 5) {
					prompt.clear();
					prompt.append(L"Pick Rotation Up");
					rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
					width = rect.Right - rect.Left;
					height = rect.Bottom - rect.Top;
					clSize = m_window.getClientSize();
					m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
				}
				else if (keycount == 6) {
					prompt.clear();
					prompt.append(L"Pick Rotation Down");
					rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
					width = rect.Right - rect.Left;
					height = rect.Bottom - rect.Top;
					clSize = m_window.getClientSize();
					m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
				}
				else if (keycount == 7) {
					prompt.clear();
					prompt.append(L"Pick Rotation Right");
					rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
					width = rect.Right - rect.Left;
					height = rect.Bottom - rect.Top;
					clSize = m_window.getClientSize();
					m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
				}
				else if (keycount == 8) {
					prompt.clear();
					prompt.append(L"Pick Rotation Left");
					rect = m_font->MeasureString(prompt.c_str(), L"Calibri", 20.0f, &layout, FW1_NOWORDWRAP);
					width = rect.Right - rect.Left;
					height = rect.Bottom - rect.Top;
					clSize = m_window.getClientSize();
					m_font->DrawString(m_context.get(), prompt.c_str(), 20.0f, (clSize.cx - width) / 2, (clSize.cy - height) / 2, 0xff00ff99, FW1_RESTORESTATE | FW1_NOGEOMETRYSHADER);
				}
			}
			
		}

	}

	
}

bool INScene::Initialize()
{
	if (!DxApplication::Initialize())
		return false;
	XFileLoader xloader(m_device, m_context);
	xloader.Load("house.x");
	m_sceneGraph.reset(new SceneGraph(move(xloader.m_nodes), move(xloader.m_meshes), move(xloader.m_materials)));

	m_doorNode = m_sceneGraph->nodeByName("Door");
	m_doorTransform = m_sceneGraph->getNodeTransform(m_doorNode);
	m_doorAngle = 0;
	m_doorAngVel = -XM_PIDIV2;

	m_cbProj.reset(new ConstantBuffer<XMFLOAT4X4>(m_device));
	m_cbView.reset(new ConstantBuffer<XMFLOAT4X4>(m_device));
	m_cbModel.reset(new ConstantBuffer<XMFLOAT4X4, 2>(m_device));
	m_cbMaterial.reset(new ConstantBuffer<Material::MaterialData>(m_device));

	EffectLoader eloader(m_device);
	eloader.Load(L"textured");
	m_texturedEffect.reset(new TexturedEffect(move(eloader.m_vs), move(eloader.m_ps), m_cbProj, m_cbView, m_cbModel,
		m_cbMaterial));
	D3D11_INPUT_ELEMENT_DESC elem[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_layout = m_device.CreateInputLayout(elem, 3, eloader.m_vsCode);

	auto s = m_window.getClientSize();
	auto ar = static_cast<float>(s.cx) / s.cy;
	XMStoreFloat4x4(&m_proj, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), ar, 0.01f, 100.0f));
	m_cbProj->Update(m_context, m_proj);

	vector<OrientedBoundingRectangle> obstacles;
	obstacles.push_back(OrientedBoundingRectangle(XMFLOAT2(-3.0f, 3.8f), 6.0f, 0.2f, 0.0f));
	obstacles.push_back(OrientedBoundingRectangle(XMFLOAT2(-3.0f, -4.0f), 6.0f, 0.2f, 0.0f));
	obstacles.push_back(OrientedBoundingRectangle(XMFLOAT2(2.8f, -3.8f), 0.2f, 7.6f, 0.0f));
	obstacles.push_back(OrientedBoundingRectangle(XMFLOAT2(-3.0f, -3.8f), 0.2f, 4.85f, 0.0f));
	obstacles.push_back(OrientedBoundingRectangle(XMFLOAT2(-3.0f, 1.95f), 0.2f, 1.85f, 0.0f));
	obstacles.push_back(OrientedBoundingRectangle(XMFLOAT2(-3.05f, 1.0f), 0.1f, 1.0f, 0.0f));
	m_collisions.SetObstacles(move(obstacles));

	RasterizerDescription rsDesc;
	m_rsState = m_device.CreateRasterizerState(rsDesc);
	SamplerDescription sDesc;
	m_sampler = m_device.CreateSamplerState(sDesc);
	ID3D11SamplerState* sstates[1] = { m_sampler.get() };
	m_context->PSSetSamplers(0, 1, sstates);
	
	IFW1Factory *pf;
	auto result = FW1CreateFactory(FW1_VERSION, &pf);
	m_fontFactory.reset(pf);
	if (FAILED(result))
		THROW_WINAPI;
	IFW1FontWrapper *pfw;
	result = m_fontFactory->CreateFontWrapper(m_device.getDevicePtr(), L"Calibri", &pfw);
	m_font.reset(pfw);
	if (FAILED(result))
		THROW_WINAPI;
		
	InitializeInput();
	return true;
}

void INScene::Render()
{
	if (!m_context)
		return;
	DxApplication::Render();
	m_context->RSSetState(m_rsState.get());
	XMFLOAT4X4 mtx[2];
	XMStoreFloat4x4(&mtx[0], m_camera.GetViewMatrix());
	m_cbView->Update(m_context, mtx[0]);
	m_texturedEffect->Begin(m_context);
	for ( unsigned int i = 0; i < m_sceneGraph->meshCount(); ++i)
	{
		auto& m = m_sceneGraph->getMesh(i);
		auto& material = m_sceneGraph->getMaterial(m.getMaterialIdx());
		if (!material.getDiffuseTexture())
			continue;
		ID3D11ShaderResourceView* srv[2] = { material.getDiffuseTexture().get(), material.getSpecularTexture().get() };
		m_context->PSSetShaderResources(0, 2, srv);
		mtx[0] = m.getTransform();
		auto modelit = XMLoadFloat4x4(&mtx[0]);
		XMVECTOR det;
		modelit = XMMatrixTranspose(XMMatrixInverse(&det, modelit));
		XMStoreFloat4x4(&mtx[1], modelit);
		m_cbMaterial->Update(m_context, material.getMaterialData());
		m_cbModel->Update(m_context, mtx);
		m_context->IASetInputLayout(m_layout.get());
		m.Render(m_context);
	}
	RenderText();
	RenderMenu();
}

void INScene::OpenDoor()
{
	if (m_doorAngVel < 0)
		m_doorAngVel = -m_doorAngVel;
}

void INScene::CloseDoor()
{
	if (m_doorAngVel > 0)
		m_doorAngVel = -m_doorAngVel;
}

void INScene::ToggleDoor()
{
	m_doorAngVel = -m_doorAngVel;
}

void INScene::UpdateDoor(float dt)
{
	if ((m_doorAngVel > 0 && m_doorAngle < XM_PIDIV2) || (m_doorAngVel < 0 && m_doorAngle > 0))
	{
		m_doorAngle += dt*m_doorAngVel;
		if ( m_doorAngle < 0 )
			m_doorAngle = 0;
		else if (m_doorAngle > XM_PIDIV2)
			m_doorAngle = XM_PIDIV2;
		XMFLOAT4X4 doorTransform;
		auto mtx = XMLoadFloat4x4(&m_doorTransform);
		auto v = XMVectorSet(0.000004f, 0.0f, -1.08108f, 1.0f);
		v = XMVector3TransformCoord(v, mtx);
		XMStoreFloat4x4(&doorTransform, mtx * XMMatrixTranslationFromVector(-v) * XMMatrixRotationZ(m_doorAngle) *
			XMMatrixTranslationFromVector(v));
		m_sceneGraph->setNodeTransform(m_doorNode, doorTransform);
		auto tr = m_collisions.MoveObstacle(5, OrientedBoundingRectangle(XMFLOAT2(-3.05f, 1.0f), 0.1f, 1.0f,
			m_doorAngle));
		m_camera.Move(XMFLOAT3(tr.x, 0, tr.y));
	}
}

void INScene::MoveCharacter(float dx, float dz)
{
	auto forward = m_camera.getForwardDir();
	auto right = m_camera.getRightDir();
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, forward*dz + right*dx);
	XMFLOAT2 tr = XMFLOAT2(temp.x, temp.z);
	m_collisions.MoveCharacter(tr);
	m_camera.Move(XMFLOAT3(tr.x, 0, tr.y));
}

bool INScene::FacingDoor() const
{
	auto rect = m_collisions.getObstacle(5);
	// ReSharper disable CppMsExtAddressOfClassRValue
	XMVECTOR points[4] = { XMLoadFloat2(&rect.getP1()), XMLoadFloat2(&rect.getP2()), XMLoadFloat2(&rect.getP3()), XMLoadFloat2(&rect.getP4()) };
	auto forward = XMVectorSwizzle(m_camera.getForwardDir(), 0, 2, 1, 3);
	auto camera = XMVectorSwizzle(XMLoadFloat4(&m_camera.getPosition()), 0, 2, 1, 3);
	// ReSharper restore CppMsExtAddressOfClassRValue
	unsigned int max_i = 0, max_j = 0;
	auto max_a = 0.0f;
	for (unsigned int i = 0; i < 4; ++i)
		for (unsigned int j = i + 1; j < 4; ++j)
		{
			float a = XMVector2AngleBetweenVectors(points[i]-camera, points[j]-camera).m128_f32[0];
			if (a > max_a)
			{
				max_i = i;
				max_j = j;
				max_a = a;
			}
		}
		return XMScalarNearEqual(XMVector2AngleBetweenVectors(forward, points[max_i]-camera).m128_f32[0] + XMVector2AngleBetweenVectors(forward, points[max_j] - camera).m128_f32[0], max_a, 0.001f);
}

float INScene::DistanceToDoor() const
{
	return m_collisions.DistanceToObstacle(5);
}
