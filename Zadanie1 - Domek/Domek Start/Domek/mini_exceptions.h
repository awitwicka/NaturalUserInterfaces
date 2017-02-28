#pragma once

#include <Windows.h>
#include <string>

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#define STRINGIFY(x) #x
#define TOWSTRING(x) WIDEN(STRINGIFY(x))
#define __AT__ __WFILE__ L":" TOWSTRING(__LINE__)

namespace mini
{
	class Exception
	{
	public:
		virtual ~Exception()
		{
		}

		Exception(const wchar_t* location) { m_location = location; }
		virtual std::wstring getMessage() const = 0;
		virtual int getExitCode() const = 0;
		const wchar_t* getErrorLocation() const { return m_location; }
	private:
		const wchar_t* m_location;
	};

	class WinAPIException : public Exception
	{
	public:
		explicit WinAPIException(const wchar_t* location, DWORD errorCode = GetLastError());
		int getExitCode() const override { return getErrorCode(); }
		DWORD getErrorCode() const { return m_code; }
		std::wstring getMessage() const override;

	private:
		DWORD m_code;
	};

	class CustomException : public Exception
	{
	public:
		CustomException(const wchar_t* location, const std::wstring& message);
		CustomException(const wchar_t* location, std::wstring&& message);
		int getExitCode() const override { return -1; }
		std::wstring getMessage() const override;

	private:
		std::wstring m_message;
	};
}

#define THROW_WINAPI throw mini::WinAPIException(__AT__)
#define THROW(message) throw mini::CustomException(__AT__, message)