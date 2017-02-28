#include "mini_constantBuffer.h"
#include "mini_dxstructures.h"
#include "mini_exceptions.h"

using namespace std;
using namespace mini;
using namespace utils;

ConstantBufferBase::ConstantBufferBase(DxDevice& device, unsigned int dataSize, unsigned int dataCount)
	: m_mapped(0), m_dataSize(dataSize), m_dataCount(dataCount ? dataCount : 1)
{
	auto bufferSize = m_dataCount * m_dataSize;
	auto fill = 16 - (bufferSize%16);
	if (fill < 16)
		bufferSize += fill;
	BufferDescription desc(bufferSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_bufferObject = device.CreateBuffer(desc);
}

void ConstantBufferBase::Map(const dx_ptr<ID3D11DeviceContext>& context)
{
	if (m_mapped++)
		return;
	auto hr = context->Map(m_bufferObject.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &m_resource);
	if (FAILED(hr))
		THROW_WINAPI;
}

void ConstantBufferBase::Unmap(const dx_ptr<ID3D11DeviceContext>& context)
{
	if (!m_mapped || --m_mapped)
		return;
	context->Unmap(m_bufferObject.get(), 0);
}

void* ConstantBufferBase::get() const
{
	if (!m_mapped)
		THROW(L"Buffer not mapped!");
	return m_resource.pData;
}

void ConstantBufferBase::Update(const dx_ptr<ID3D11DeviceContext>& context, const void* dataPtr, unsigned int dataCount)
{
	if (!dataCount)
		return;
	if (dataCount > m_dataCount)
		dataCount = m_dataCount;
	Map(context);
	memcpy(m_resource.pData, dataPtr, m_dataSize * dataCount);
	Unmap(context);
}