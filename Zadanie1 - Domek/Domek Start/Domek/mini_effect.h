#pragma once

#include "mini_dxptr.h"
#include <D3D11.h>

namespace mini
{
	class Effect
	{
	public:
		virtual ~Effect()
		{
		}

		Effect(dx_ptr<ID3D11VertexShader>&& vs, dx_ptr<ID3D11PixelShader>&& ps);
		virtual void Begin(const dx_ptr<ID3D11DeviceContext>& context);
		
	private:
		dx_ptr<ID3D11VertexShader> m_vs;
		dx_ptr<ID3D11PixelShader> m_ps;
	};
}
