#pragma once

#include "mini_dxDevice.h"

namespace mini
{
	class ConstantBufferBase
	{
	public:
		const dx_ptr<ID3D11Buffer>& getBufferObject() const { return m_bufferObject; }

	protected:
		ConstantBufferBase(DxDevice& device, unsigned int dataSize, unsigned int dataCount);
		ConstantBufferBase(const ConstantBufferBase& right) = delete;
		ConstantBufferBase& operator=(const ConstantBufferBase& right) = delete;

		void Update(const dx_ptr<ID3D11DeviceContext>& context, const void* dataPtr, unsigned int dataCount);
		
		void Map(const dx_ptr<ID3D11DeviceContext>& context);
		void* get() const;
		void Unmap(const dx_ptr<ID3D11DeviceContext>& context);

		int m_mapped;
		unsigned int m_dataSize;
		unsigned int m_dataCount;
		dx_ptr<ID3D11Buffer> m_bufferObject;
		D3D11_MAPPED_SUBRESOURCE m_resource;
	};

	template<typename T, unsigned int N = 1>
	class ConstantBuffer : public ConstantBufferBase
	{
	public:
		explicit ConstantBuffer(DxDevice& device)
			: ConstantBufferBase(device, sizeof(T), N)
		{ }
		ConstantBuffer(const ConstantBuffer<T, N>& right) = delete;
		ConstantBuffer<T, N>& operator=(const ConstantBuffer<T, N>& right) = delete;

		void Update(const dx_ptr<ID3D11DeviceContext>& context, const T& data)
		{
			return ConstantBufferBase::Update(context, reinterpret_cast<const void*>(&data), 1);
		}

		void Update(const dx_ptr<ID3D11DeviceContext>& context, const T* data)
		{
			return ConstantBufferBase::Update(context, reinterpret_cast<const void*>(data), N);
		}

		void Map(const dx_ptr<ID3D11DeviceContext>& context) { ConstantBufferBase::Map(context); }
		T* get() { return reinterpret_cast<T*>(ConstantBufferBase::get()); }
		void Unmap(const dx_ptr<ID3D11DeviceContext>& context) { ConstantBufferBase::Unmap(context); }
	};
}