#pragma once
#include <memory>

namespace mini
{
	template<class T>
	class DxDeleter
	{
	public:
		typedef T* pointer;

		void operator () (T* comObject) { if (comObject) comObject->Release(); }
	};

	template<class T>
	using dx_ptr = std::unique_ptr<T, DxDeleter<T>>;
}