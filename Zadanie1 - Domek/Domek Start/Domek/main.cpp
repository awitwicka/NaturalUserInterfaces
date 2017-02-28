#include "in_scene.h"
#include "mini_exceptions.h"

using namespace std;
using namespace mini;
using namespace DirectX;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);
	unique_ptr<INScene> app;
	auto exitCode = 0;
	try
	{
		app = make_unique<INScene>(hInstance, 1280, 720);
		exitCode = app->Run(cmdShow);
	}
	catch (Exception& e)
	{
		MessageBoxW(nullptr, e.getMessage().c_str(), L"B³¹d", MB_OK);
		exitCode = e.getExitCode();
	}
	catch (exception& e)
	{
		string s(e.what());
		MessageBoxW(nullptr, wstring(s.begin(), s.end()).c_str(), L"B³¹d!", MB_OK);
	}
	catch(const char* str)
	{
		string s(str);
		MessageBoxW(nullptr, wstring(s.begin(), s.end()).c_str(), L"B³¹d!", MB_OK);
	}
	catch(const wchar_t* str)
	{
		MessageBoxW(nullptr, str, L"B³¹d!", MB_OK);
	}
	catch(...)
	{
		MessageBoxW(nullptr, L"Nieznany B³¹d", L"B³¹d", MB_OK);
		exitCode = -1;
	}
	app.reset();
	return exitCode;
}