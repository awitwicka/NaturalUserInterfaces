#include "mini_camera.h"

using namespace mini;
using namespace DirectX;

Camera::Camera(XMFLOAT3 position)
	: m_angleX(0), m_angleY(0), m_position(position.x, position.y, position.z, 1.0f)
{
}

void Camera::GetViewMatrix(XMMATRIX& viewMtx) const
{
	viewMtx = XMMatrixTranslation(-m_position.x, -m_position.y, -m_position.z) *
			  XMMatrixRotationY(-m_angleY) *
			  XMMatrixRotationX(-m_angleX);
}

XMMATRIX Camera::GetViewMatrix() const
{
	XMMATRIX viewMtx;
	GetViewMatrix(viewMtx);
	return viewMtx;
}

XMVECTOR Camera::getForwardDir() const
{
	auto forward = XMVectorSet(0, 0, 1, 0);
	forward = XMVector3TransformNormal(forward, XMMatrixRotationY(m_angleY));
	return forward;
}

XMVECTOR Camera::getRightDir() const
{
	auto right = XMVectorSet(1, 0, 0, 0);
	right = XMVector3TransformNormal(right, XMMatrixRotationY(m_angleY));
	return right;
}

void Camera::Move(XMFLOAT3 v)
{
	auto pos = XMLoadFloat4(&m_position);
	pos = XMVectorSet(0, 0, 1, 0) * v.z + XMVectorSet(1, 0, 0, 0) * v.x + XMVectorSet(0, 1, 0, 0) * v.y + pos;
	XMStoreFloat4(&m_position, pos);
}

void Camera::Rotate(float dx, float dy)
{
	m_angleX = XMScalarModAngle(m_angleX + dx);
	m_angleY = XMScalarModAngle(m_angleY + dy);
}