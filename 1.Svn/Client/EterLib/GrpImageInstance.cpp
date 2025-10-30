//THIS IS AN EXAMPLE
//YOU DO NOT NEED TO ADD THIS

///Add
#if defined(__BL_SHADER__)
#include "../UserInterface/ShaderManager.h"
#endif

//Find
		STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);

///Change
#if defined(__BL_SHADER__)
		IShader* shader = CShaderManager::Instance().GetShader(IShader::EType::INVERT);
		if (shader && shader->IsVisible() && shader->GetRenderType() == IShader::ERenderType::GRP_IMAGE)
		{
			shader->ApplySettings(pTexture->GetD3DTexture());
			shader->Render([] {
				STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
				});
		}
		else
			STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
#else
		STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
#endif