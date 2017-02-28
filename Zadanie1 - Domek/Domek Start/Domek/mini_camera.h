#pragma once

#include <DirectXMath.h>

namespace mini
{
	class Camera
	{
	public:
		explicit Camera(DirectX::XMFLOAT3 position);
		
		DirectX::XMMATRIX GetViewMatrix() const;
		void GetViewMatrix(DirectX::XMMATRIX& m) const;
		
		DirectX::XMFLOAT4 getPosition() const
		{ return m_position; }
		DirectX::XMVECTOR getForwardDir() const;
		DirectX::XMVECTOR getRightDir() const;

		void Move(DirectX::XMFLOAT3 v);
		void Rotate(float dx, float dy);

	private:
		float m_angleX, m_angleY;
		DirectX::XMFLOAT4 m_position;
	};
}