///Add
#if defined(__BL_SHADER__)
#include "../UserInterface/ShaderManager.h"
#endif

//Find
	STATEMANAGER.DrawPrimitive(D3DPT_TRIANGLELIST, 0, uPriCount);

///Change
#if defined(__BL_SHADER__)
	IShader* shader = CShaderManager::Instance().GetShader(IShader::EType::WATER);
	if (shader && shader->IsVisible())
	{
		LPDIRECT3DBASETEXTURE9 texture;
		STATEMANAGER.GetTexture(0, &texture);
		shader->ApplySettings(texture);
		shader->Render([&] {
			STATEMANAGER.DrawPrimitive(D3DPT_TRIANGLELIST, 0, uPriCount);
			});
	}
	else
#endif
		STATEMANAGER.DrawPrimitive(D3DPT_TRIANGLELIST, 0, uPriCount);