#pragma once

#include "mini_dxDevice.h"

namespace mini
{
	class EffectLoader
	{
	public:
		explicit EffectLoader(DxDevice& device);

		void Load(std::wstring effectName);

		std::vector<BYTE> m_vsCode;
		std::vector<BYTE> m_psCode;
		dx_ptr<ID3D11VertexShader> m_vs;
		dx_ptr<ID3D11PixelShader> m_ps;

	private:
		DxDevice& m_device;
	};
}