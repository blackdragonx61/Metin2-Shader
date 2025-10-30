/**
 * @file   ShaderManager.cpp
 * @author blackdragonx61 / Mali
 * @date   27.10.2025
 * __BL_SHADER__
 */

#include "StdAfx.h"
#include "ShaderManager.h"
#include "PythonApplication.h"
#include "../EterPack/EterPackManager.h"

#if defined(__BL_SHADER__)

struct SQuadVertex
{
	float x, y, z, rhw;
	float u, v;
};

CShaderManager::CShaderManager() :
	m_ScreenQuad(nullptr),
	m_RenderTextureA(nullptr),
	m_RenderTextureB(nullptr),
	m_RenderSurfaceA(nullptr),
	m_RenderSurfaceB(nullptr),
	m_OriginalRenderTexture(nullptr)
{
}

CShaderManager::~CShaderManager()
{
	Destroy();
}

bool CShaderManager::Initialize()
{
	if (!CreateTextures())
		return false;
	
	if (!__CreateScreenQuad())
		return false;

	if (!__CreateFX())
		return false;

	return true;
}

void CShaderManager::Destroy()
{
	stl_wipe_second(m_ShaderMap);
	safe_release(m_ScreenQuad);
	ReleaseTextures();
}

bool CShaderManager::__CreateScreenQuad()
{
	if (m_ScreenQuad)
		return true;

	CPythonApplication& app = CPythonApplication::Instance();
	const float width = static_cast<float>(app.GetWidth());
	const float height = static_cast<float>(app.GetHeight());

	SQuadVertex vertices[] =
	{
		{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f },
		{ -0.5f, height - 0.5f, 0.5f, 1.0f, 0.0f, 1.0f },
		{ width - 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f },
		{ width - 0.5f, height - 0.5f, 0.5f, 1.0f, 1.0f, 1.0f }
	};

	LPDIRECT3DDEVICE9 device = STATEMANAGER.GetDevice();
	if (FAILED(device->CreateVertexBuffer(
		sizeof(vertices),
		D3DUSAGE_WRITEONLY,
		D3DFVF_XYZRHW | D3DFVF_TEX1,
		D3DPOOL_MANAGED,
		&m_ScreenQuad,
		nullptr)))
	{
		TraceError("CShaderManager::__CreateScreenQuad CreateVertexBuffer Failed.");
		return false;
	}

	void* data;
	if (FAILED(m_ScreenQuad->Lock(0, 0, &data, 0)))
	{
		TraceError("CShaderManager::__CreateScreenQuad Lock Failed.");
		return false;
	}

	memcpy(data, vertices, sizeof(vertices));

	if (FAILED(m_ScreenQuad->Unlock()))
	{
		TraceError("CShaderManager::__CreateScreenQuad Unlock Failed.");
		return false;
	}

	return true;
}

bool CShaderManager::__CreateFX()
{
	if (!AddFX(IShader::EType::WAVE, "d:/ymir work/shader/wave.fx"))
		return false;

	if (!AddFX(IShader::EType::GRAY, "d:/ymir work/shader/gray.fx"))
		return false;

	if (!AddFX(IShader::EType::PIXELATE, "d:/ymir work/shader/pixelate.fx"))
		return false;

	if (!AddFX(IShader::EType::INVERT, "d:/ymir work/shader/invert.fx"))
		return false;

	if (!AddFX(IShader::EType::ATLAS_SPOTLIGHT, "d:/ymir work/shader/atlas_spotlight.fx"))
		return false;

	if (!AddFX(IShader::EType::UI_BLUR, "d:/ymir work/shader/ui_blur.fx"))
		return false;

	/*{
		IShader* shader = GetShader(IShader::EType::PIXELATE);
		if (shader)
			dynamic_cast<CPixelateShader*>(shader)->SetPixelSize(10.0f);
	}*/

	return true;
}

bool CShaderManager::CreateTextures()
{
	LPDIRECT3DDEVICE9 device = STATEMANAGER.GetDevice();
	CPythonApplication& app = CPythonApplication::Instance();
	const int width = app.GetWidth();
	const int height = app.GetHeight();

	if (FAILED(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_RenderTextureA, nullptr)))
	{
		TraceError("CShaderManager::CreateTextures m_RenderTextureA: Create Failed.");
		return false;
	}

	if (FAILED(m_RenderTextureA->GetSurfaceLevel(0, &m_RenderSurfaceA)))
	{
		TraceError("CShaderManager::CreateTextures m_RenderTextureA: Surface Level Failed.");
		return false;
	}

	if (FAILED(device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_RenderTextureB, nullptr)))
	{
		TraceError("CShaderManager::CreateTextures m_RenderTextureB: Create Failed.");
		return false;
	}

	if (FAILED(m_RenderTextureB->GetSurfaceLevel(0, &m_RenderSurfaceB)))
	{
		TraceError("CShaderManager::CreateTextures m_RenderTextureB: Surface Level Failed.");
		return false;
	}

	for (const auto& elem : m_ShaderMap)
	{
		IShader* shader = elem.second;
		if (shader)
			shader->ResetDevice();
	}

	return true;
}

void CShaderManager::ReleaseTextures()
{
	for (const auto& elem : m_ShaderMap)
	{
		IShader* shader = elem.second;
		if (shader)
			shader->LostDevice();
	}
	
	safe_release(m_RenderTextureA);
	safe_release(m_RenderTextureB);
	safe_release(m_RenderSurfaceA);
	safe_release(m_RenderSurfaceB);
	safe_release(m_OriginalRenderTexture);
}

void CShaderManager::Begin()
{
	LPDIRECT3DDEVICE9 device = STATEMANAGER.GetDevice();

	if (FAILED(device->GetRenderTarget(0, &m_OriginalRenderTexture)))
		return;

	device->SetRenderTarget(0, m_RenderSurfaceA);
	device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0.0f);
}

void CShaderManager::End(IShader::ERenderType renderType)
{
	LPDIRECT3DDEVICE9 device = STATEMANAGER.GetDevice();

	STATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_ZWRITEENABLE, FALSE);

	DWORD state_COLORARG1;
	STATEMANAGER.GetTextureStageState(0, D3DTSS_COLORARG1, &state_COLORARG1);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	STATEMANAGER.SetStreamSource(0, m_ScreenQuad, sizeof(SQuadVertex));

	LPDIRECT3DTEXTURE9 src = m_RenderTextureA;
	LPDIRECT3DTEXTURE9 dst = m_RenderTextureB;

	for (const auto& elem : m_ShaderMap)
	{
		IShader* shader = elem.second;
		if (!shader || !shader->IsVisible() || !shader->GetEffect() || shader->GetRenderType() != renderType)
			continue;

		if (dst == m_RenderTextureA)
			device->SetRenderTarget(0, m_RenderSurfaceA);
		else
			device->SetRenderTarget(0, m_RenderSurfaceB);

		device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0.0f);

		shader->ApplySettings(src);
		shader->Render([] {
			STATEMANAGER.GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		});

		std::swap(src, dst);
	}

	device->SetRenderTarget(0, m_OriginalRenderTexture);
	safe_release(m_OriginalRenderTexture);

	STATEMANAGER.SetTexture(0, src);
	STATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	STATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ZWRITEENABLE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, state_COLORARG1);
}

bool CShaderManager::AddFX(IShader::EType type, const char* path)
{
	if (GetShader(type))
	{
		TraceError("CShaderManager::AddFX Shader Already Exists: %s", path);
		return false;
	}

	IShader* shader;

	switch (type)
	{
	case IShader::EType::WAVE:
		shader = new CWaveShader;
		break;
	case IShader::EType::GRAY:
		shader = new CGrayShader;
		break;
	case IShader::EType::PIXELATE:
		shader = new CPixelateShader;
		break;
	case IShader::EType::INVERT:
		shader = new CInvertColorShader;
		break;
	case IShader::EType::ATLAS_SPOTLIGHT:
		shader = new CAtlasSpotlightShader;
		break;
	case IShader::EType::UI_BLUR:
		shader = new CUIBlurShader;
		break;
	default:
		TraceError("CShaderManager::AddFX Unknown Shader Type: %d", type);
		return false;
	}

	if (!shader->Load(path))
	{
		delete shader;
		return false;
	}

	m_ShaderMap[type] = shader;
	return true;
}

IShader* CShaderManager::GetShader(IShader::EType type) const
{
	auto it = m_ShaderMap.find(type);
	if (it == m_ShaderMap.end())
		return nullptr;

	return it->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

PyObject* shaderMgrBegin(PyObject* poSelf, PyObject* poArgs)
{
	CShaderManager::Instance().Begin();
	return Py_BuildNone();
}

PyObject* shaderMgrEnd(PyObject* poSelf, PyObject* poArgs)
{
	CShaderManager::Instance().End(IShader::ERenderType::UI);
	return Py_BuildNone();
}

PyObject* shaderMgrShowShader(PyObject* poSelf, PyObject* poArgs)
{
	int iShaderTypeIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iShaderTypeIndex))
		return Py_BuildException();

	IShader* shader = CShaderManager::Instance().GetShader(static_cast<IShader::EType>(iShaderTypeIndex));
	if (!shader)
	{
		TraceError("shadermgrShowShader Cannot Find Shader Type: (%d)", iShaderTypeIndex);
		return Py_BuildNone();
	}

	shader->Show();
	return Py_BuildNone();
}

PyObject* shaderMgrHideShader(PyObject* poSelf, PyObject* poArgs)
{
	int iShaderTypeIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iShaderTypeIndex))
		return Py_BuildException();
	
	IShader* shader = CShaderManager::Instance().GetShader(static_cast<IShader::EType>(iShaderTypeIndex));
	if (!shader)
	{
		TraceError("shadermgrHideShader Cannot Find Shader Type: (%d)", iShaderTypeIndex);
		return Py_BuildNone();
	}

	shader->Hide();
	return Py_BuildNone();
}

void initshadermgr()
{
	static PyMethodDef s_methods[] =
	{
		{ "Begin",			shaderMgrBegin,			METH_VARARGS },
		{ "End",			shaderMgrEnd,			METH_VARARGS },
		{ "ShowShader",		shaderMgrShowShader,	METH_VARARGS },
		{ "HideShader",		shaderMgrHideShader,	METH_VARARGS },

		{ NULL,				NULL,					NULL         },
	};

	PyObject* poModule = Py_InitModule("shaderMgr", s_methods);
	PyModule_AddIntConstant(poModule, "SHADER_TYPE_WAVE",				(long)IShader::EType::WAVE);
	PyModule_AddIntConstant(poModule, "SHADER_TYPE_GRAY",				(long)IShader::EType::GRAY);
	PyModule_AddIntConstant(poModule, "SHADER_TYPE_PIXELATE",			(long)IShader::EType::PIXELATE);
	PyModule_AddIntConstant(poModule, "SHADER_TYPE_INVERT",				(long)IShader::EType::INVERT);
	PyModule_AddIntConstant(poModule, "SHADER_TYPE_ATLAS_SPOTLIGHT",	(long)IShader::EType::ATLAS_SPOTLIGHT);
	PyModule_AddIntConstant(poModule, "SHADER_TYPE_UI_BLUR",			(long)IShader::EType::UI_BLUR);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

IShader::IShader() :
	m_Effect(nullptr),
	m_Visible(true)
{
}

IShader::~IShader()
{
	safe_release(m_Effect);
}

bool IShader::Load(const char* path)
{
	CMappedFile file;
	const VOID* data;
	if (!CEterPackManager::Instance().Get(file, path, &data))
	{
		TraceError("IShader::Load - Cannot Load FX: %s", path);
		return false;
	}

	LPD3DXBUFFER errors = nullptr;
	HRESULT hr = D3DXCreateEffect(
		STATEMANAGER.GetDevice(),
		data,
		file.Size(),
		nullptr,
		nullptr,
		D3DXSHADER_OPTIMIZATION_LEVEL3,
		nullptr,
		&m_Effect,
		&errors);

	if (FAILED(hr))
	{
		if (errors)
		{
			OutputDebugStringA((char*)errors->GetBufferPointer());
			TraceError((char*)errors->GetBufferPointer());
			errors->Release();
		}
		TraceError("IShader::Load - Failed To Compile Effect: %s", path);
		return false;
	}

	return true;
}

void IShader::Render(const std::function<void()>& func) const
{
	if (!m_Effect)
		return;

	UINT i = 0;
	D3DXHANDLE hTech = nullptr;

	while ((hTech = m_Effect->GetTechnique(i++)))
	{
		m_Effect->SetTechnique(hTech);

		UINT numPasses = 0;
		m_Effect->Begin(&numPasses, 0);

		for (UINT p = 0; p < numPasses; ++p)
		{
			m_Effect->BeginPass(p);
			func();
			m_Effect->EndPass();
		}

		m_Effect->End();
	}
}

void IShader::LostDevice() const
{
	if (!m_Effect)
		return;

	m_Effect->OnLostDevice();
}

void IShader::ResetDevice() const
{
	if (!m_Effect)
		return;

	m_Effect->OnResetDevice();
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CWaveShader::CWaveShader() :
	m_Amplitude(0.02f),
	m_Frequency(10.0f)
{
}

void CWaveShader::ApplySettings(LPDIRECT3DTEXTURE9 inputTexture) const
{
	if (!m_Effect)
		return;

	m_Effect->SetTexture("SceneTex", inputTexture);
	m_Effect->SetFloat("g_Time", CTimer::Instance().GetCurrentSecond());
	m_Effect->SetFloat("g_Amplitude", m_Amplitude);
	m_Effect->SetFloat("g_Frequency", m_Frequency);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CGrayShader::CGrayShader() :
	m_LumR(0.299f),
	m_LumG(0.587f),
	m_LumB(0.114f)
{
}

void CGrayShader::ApplySettings(LPDIRECT3DTEXTURE9 inputTexture) const
{
	if (!m_Effect)
		return;

	m_Effect->SetTexture("SceneTex", inputTexture);
	m_Effect->SetFloat("LumR", m_LumR);
	m_Effect->SetFloat("LumG", m_LumG);
	m_Effect->SetFloat("LumB", m_LumB);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CPixelateShader::CPixelateShader() :
	m_PixelSize(3.0f)
{
}

void CPixelateShader::ApplySettings(LPDIRECT3DTEXTURE9 inputTexture) const
{
	if (!m_Effect)
		return;

	CPythonApplication& app = CPythonApplication::Instance();
	const int width = app.GetWidth();
	const int height = app.GetHeight();

	m_Effect->SetTexture("SceneTex", inputTexture);
	m_Effect->SetFloat("PixelSize", m_PixelSize);
	m_Effect->SetVector("ScreenSize", &D3DXVECTOR4((float)width, (float)height, 0, 0));
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CInvertColorShader::CInvertColorShader() = default;

void CInvertColorShader::ApplySettings(LPDIRECT3DTEXTURE9 inputTexture) const
{
	if (!m_Effect)
		return;

	m_Effect->SetTexture("SceneTex", inputTexture);
	m_Effect->SetFloat("gTime", CTimer::Instance().GetCurrentSecond());
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CAtlasSpotlightShader::CAtlasSpotlightShader() :
	m_Radius(20.0f),
	m_DarknessMin(0.0f),
	m_DarknessMax(0.8f),
	m_PulseSpeed(3.0f)
{
}

void CAtlasSpotlightShader::ApplySettings(LPDIRECT3DTEXTURE9 inputTexture) const
{
	if (!m_Effect)
		return;

	CPythonApplication& app = CPythonApplication::Instance();
	const int width = app.GetWidth();
	const int height = app.GetHeight();

	m_Effect->SetTexture("SceneTex", inputTexture);
	m_Effect->SetVector("ScreenSize", &D3DXVECTOR4((float)width, (float)height, 0, 0));
	m_Effect->SetValue("TextureSize", &m_TextureSize, sizeof(D3DXVECTOR2));

	for (int i = 0; i < m_Points.size(); ++i)
	{
		char name[32];
		snprintf(name, sizeof(name), "PixelCenters[%d]", i);
		m_Effect->SetValue(name, &m_Points[i], sizeof(D3DXVECTOR2));
	}
	m_Effect->SetInt("SpotCount", m_Points.size());

	m_Effect->SetFloat("Radius", m_Radius);
	m_Effect->SetFloat("DarknessMin", m_DarknessMin);
	m_Effect->SetFloat("DarknessMax", m_DarknessMax);
	m_Effect->SetFloat("Time", CTimer::Instance().GetCurrentSecond());
	m_Effect->SetFloat("PulseSpeed", m_PulseSpeed);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CUIBlurShader::CUIBlurShader() :
	m_BlurStrength(0.005f)
{
}

void CUIBlurShader::ApplySettings(LPDIRECT3DTEXTURE9 inputTexture) const
{
	if (!m_Effect)
		return;

	m_Effect->SetTexture("SceneTex", inputTexture);
	m_Effect->SetFloat("BlurStrength", m_BlurStrength);
}

#endif // __BL_SHADER__
