#pragma once

#include "mini_effect.h"
#include "mini_constantBuffer.h"
#include "mini_material.h"

namespace mini
{
	class TexturedEffect : public Effect
	{
	public:

		TexturedEffect(dx_ptr<ID3D11VertexShader>&& vs, 
			dx_ptr<ID3D11PixelShader>&& ps,
			std::unique_ptr<ConstantBuffer<DirectX::XMFLOAT4X4>>& cbProj,
			std::unique_ptr<ConstantBuffer<DirectX::XMFLOAT4X4>>& cbView,
			std::unique_ptr<ConstantBuffer<DirectX::XMFLOAT4X4, 2> >& cbModel,
			std::unique_ptr<ConstantBuffer<Material::MaterialData>>& cbMaterial
			);


		void Begin(const dx_ptr<ID3D11DeviceContext>& context) override;

	private:
		std::unique_ptr<ConstantBuffer<DirectX::XMFLOAT4X4>>& m_cbProj;
		std::unique_ptr<ConstantBuffer<DirectX::XMFLOAT4X4>>& m_cbView;
		std::unique_ptr<ConstantBuffer<DirectX::XMFLOAT4X4, 2> >& m_cbModel;
		std::unique_ptr<ConstantBuffer<Material::MaterialData>>& m_cbMaterial;
	};
}