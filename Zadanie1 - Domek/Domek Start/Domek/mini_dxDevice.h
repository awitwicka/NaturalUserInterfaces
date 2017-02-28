#pragma once

#include "mini_dxptr.h"
#include "mini_dxstructures.h"
#include <vector>

namespace mini
{
	class DxDevice
	{
	public:
		void Initialize(ID3D11Device* device);
		void Release();

		dx_ptr<ID3D11Texture2D> CreateTexture(const utils::Texture2DDescription& desc) const;
		dx_ptr<ID3D11RenderTargetView> CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture) const;
		dx_ptr<ID3D11DepthStencilView> CreateDepthStencilView(const dx_ptr<ID3D11Texture2D>& depthTexture) const;

		dx_ptr<ID3D11InputLayout> CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* layout,
															 unsigned int layoutElements,
															 const std::vector<BYTE>& vsByteCode) const;

		template<class T>
		dx_ptr<ID3D11Buffer> CreateVertexBuffer(const std::vector<T>& data) const
		{
			return _CreateBuffer(reinterpret_cast<const void*>(data.data()),
				data.size()*sizeof(T), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT);
		}

		dx_ptr<ID3D11Buffer> CreateIndexBuffer(const std::vector<unsigned short>& indices) const
		{
			return _CreateBuffer(reinterpret_cast<const void*>(indices.data()),
				sizeof(unsigned short) * indices.size(), D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT);
		}

		dx_ptr<ID3D11Buffer> CreateBuffer(const utils::BufferDescription& desc, const void* pData = nullptr) const;

		static std::vector<BYTE> LoadByteCode(const std::wstring& fileName);
		dx_ptr<ID3D11VertexShader> CreateVertexShader(const std::vector<BYTE>& byteCode) const;
		dx_ptr<ID3D11PixelShader> CreatePixelShader(const std::vector<BYTE>& byteCode) const;
		dx_ptr<ID3D11RasterizerState> CreateRasterizerState(const utils::RasterizerDescription& desc) const;
		dx_ptr<ID3D11SamplerState> CreateSamplerState(const utils::SamplerDescription& desc) const;
		dx_ptr<ID3D11ShaderResourceView> CreateShaderResourceView(const std::wstring& texFilePath,
			const dx_ptr<ID3D11DeviceContext>& context = nullptr) const;

		ID3D11Device * getDevicePtr() const	{ return m_device.get(); }
	private:
		dx_ptr<ID3D11Buffer> _CreateBuffer(const void * data, unsigned int byteLenght, D3D11_BIND_FLAG flag,
			D3D11_USAGE usage) const;

		dx_ptr<ID3D11Device> m_device;
	};
}