#include "mini_effectLoader.h"

using namespace mini;
using namespace std;

EffectLoader::EffectLoader(DxDevice& device)
	: m_device(device)
{ }

void EffectLoader::Load(wstring effectName)
{
	m_vsCode = m_device.LoadByteCode(effectName + L"VS.cso");
	m_psCode = m_device.LoadByteCode(effectName + L"PS.cso");
	m_vs = m_device.CreateVertexShader(m_vsCode);
	m_ps = m_device.CreatePixelShader(m_psCode);
}