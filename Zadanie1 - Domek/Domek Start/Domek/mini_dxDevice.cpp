#include "mini_dxDevice.h"
#include "mini_exceptions.h"
#include <cassert>
#include "WICTextureLoader.h"
#include <fstream>

using namespace std;
using namespace mini;
using namespace utils;

void DxDevice::Initialize(ID3D11Device* device)
{
	m_device.reset(device);
}

void DxDevice::Release()
{
	m_device.reset();
}

dx_ptr<ID3D11Texture2D> DxDevice::CreateTexture(const Texture2DDescription& desc) const
{
	assert(m_device);
	ID3D11Texture2D* t = nullptr;
	auto result = m_device->CreateTexture2D(&desc, nullptr, &t);
	dx_ptr<ID3D11Texture2D> texture(t);
	if (FAILED(result))
		THROW_WINAPI;
	return texture;
}

dx_ptr<ID3D11RenderTargetView> DxDevice::CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& backTexture) const
{
	assert(m_device);
	ID3D11RenderTargetView* bb = nullptr;
	auto result = m_device->CreateRenderTargetView(backTexture.get(), nullptr, &bb);
	dx_ptr<ID3D11RenderTargetView> backBuffer(bb);
	if (FAILED(result))
		THROW_WINAPI;
	return backBuffer;
}

dx_ptr<ID3D11DepthStencilView> DxDevice::CreateDepthStencilView(const dx_ptr<ID3D11Texture2D>& depthTexture) const
{
	assert(m_device);
	ID3D11DepthStencilView* dsv = nullptr;
	auto result = m_device->CreateDepthStencilView(depthTexture.get(), nullptr, &dsv);
	dx_ptr<ID3D11DepthStencilView> depthStencilView(dsv);
	if (FAILED(result))
		THROW_WINAPI;
	return depthStencilView;
}

dx_ptr<ID3D11Buffer> DxDevice::_CreateBuffer(const void * data, unsigned int byteLenght, D3D11_BIND_FLAG flag,
			D3D11_USAGE usage) const
{
	BufferDescription desc(byteLenght, flag, usage);
	if ((usage & D3D11_USAGE_DYNAMIC) != 0 || (usage & D3D11_USAGE_STAGING) != 0)
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	return CreateBuffer(desc, data);
}

dx_ptr<ID3D11Buffer> DxDevice::CreateBuffer(const BufferDescription& desc, const void* pData) const
{
	assert(m_device);
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = pData;
	ID3D11Buffer* b = nullptr;
	auto result = m_device->CreateBuffer(&desc, pData ? &data : nullptr, &b);
	dx_ptr<ID3D11Buffer> buffer(b);
	if (FAILED(result))
		THROW_WINAPI;
	return buffer;
}

dx_ptr<ID3D11InputLayout> DxDevice::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* layout,
	unsigned int layoutElements,
	const vector<BYTE>& vsByteCode) const
{
	assert(m_device);
	ID3D11InputLayout* il = nullptr;
	auto result = m_device->CreateInputLayout(layout, layoutElements, vsByteCode.data(), vsByteCode.size(), &il);
	dx_ptr<ID3D11InputLayout> inputLayout(il);
	if (FAILED(result))
		THROW_WINAPI;
	return inputLayout;
}

dx_ptr<ID3D11VertexShader> DxDevice::CreateVertexShader(const vector<BYTE>& byteCode) const
{
	assert(m_device);
	ID3D11VertexShader* vs = nullptr;
	auto result = m_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, &vs);
	dx_ptr<ID3D11VertexShader> vertexShader(vs);
	if (FAILED(result))
		THROW_WINAPI;
	return vertexShader;
}

dx_ptr<ID3D11PixelShader> DxDevice::CreatePixelShader(const vector<BYTE>& byteCode) const
{
	assert(m_device);
	ID3D11PixelShader* ps = nullptr;
	auto result = m_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, &ps);
	dx_ptr<ID3D11PixelShader> pixelShader(ps);
	if (FAILED(result))
		THROW_WINAPI;
	return pixelShader;
}

dx_ptr<ID3D11RasterizerState> DxDevice::CreateRasterizerState(const RasterizerDescription& desc) const
{
	assert(m_device);
	ID3D11RasterizerState* s = nullptr;
	auto result = m_device->CreateRasterizerState(&desc, &s);
	dx_ptr<ID3D11RasterizerState> state(s);
	if (FAILED(result))
		THROW_WINAPI;
	return state;
}

dx_ptr<ID3D11SamplerState> DxDevice::CreateSamplerState(const SamplerDescription& desc) const
{
	assert(m_device);
	ID3D11SamplerState* s = nullptr;
	auto result = m_device->CreateSamplerState(&desc, &s);
	dx_ptr<ID3D11SamplerState> sampler(s);
	if (FAILED(result))
		THROW_WINAPI;
	return sampler;
}

dx_ptr<ID3D11ShaderResourceView> DxDevice::CreateShaderResourceView(const wstring& texFilePath,
	const dx_ptr<ID3D11DeviceContext>& context) const
{
	assert(m_device);
	ID3D11ShaderResourceView* rv;
	auto result = DirectX::CreateWICTextureFromFile(m_device.get(), context.get(), texFilePath.c_str(), nullptr,
		&rv);
	dx_ptr<ID3D11ShaderResourceView> resourceView(rv);
	if (FAILED(result))
		//Make sure CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); is called before first use of this function!
		THROW_WINAPI;
	return resourceView;
}

vector<BYTE> DxDevice::LoadByteCode(const wstring& fileName)
{
	size_t byteCodeLength;
	ifstream sIn(fileName, ios::in | ios::binary);
	if (!sIn)
		THROW(L"Unable to open " + fileName);
	sIn.seekg(0, ios::end);
	byteCodeLength = sIn.tellg();
	sIn.seekg(0, ios::beg);
	vector<BYTE> byteCode(byteCodeLength);
	if (!sIn.read(reinterpret_cast<char*>(byteCode.data()), byteCodeLength))
		THROW(L"Error reading" + fileName);
	sIn.close();
	return byteCode;
}