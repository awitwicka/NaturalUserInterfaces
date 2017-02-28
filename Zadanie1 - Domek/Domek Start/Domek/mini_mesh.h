#pragma once

#include "mini_dxptr.h"
#include <vector>
#include <DirectXMath.h>
#include <D3D11.h>

namespace mini
{
	class Mesh
	{
	public:
		Mesh();
		Mesh(std::vector<dx_ptr<ID3D11Buffer>>&& vbuffers,
			 std::vector<unsigned int>&& vstrides,
			 std::vector<unsigned int>&& voffsets,
			 dx_ptr<ID3D11Buffer>&& indices,
			 unsigned int indexCount,
			 unsigned int materialIdx);

		Mesh(Mesh&& right);
		Mesh(const Mesh& right) = delete;
		void Release();
		~Mesh();

		Mesh& operator=(const Mesh& right) = delete;
		Mesh& operator=(Mesh&& right);
		void Render(const dx_ptr<ID3D11DeviceContext>& context) const;
		const DirectX::XMFLOAT4X4& getTransform() const { return m_transform; }
		void setTransform(const DirectX::XMFLOAT4X4& transform) { m_transform = transform; }

		unsigned int getMaterialIdx() const { return m_materialIdx; }
		void setMaterialIdx(unsigned int idx) { m_materialIdx = idx; }

	private:

		dx_ptr<ID3D11Buffer> m_indexBuffer;
		std::unique_ptr<ID3D11Buffer*[]> m_vertexBuffers;
		std::unique_ptr<unsigned int[]> m_strides;
		std::unique_ptr<unsigned int[]> m_offsets;
		DirectX::XMFLOAT4X4 m_transform;
		unsigned int m_buffersCount;
		unsigned int m_indexCount;
		unsigned int m_materialIdx;
	};
}
