//THIS IS AN EXAMPLE
//YOU DO NOT NEED TO ADD THIS

///Add
#if defined(__BL_SHADER__)
#include "ShaderManager.h"
#endif

//Find in void CPythonMiniMap::RenderAtlas(float fScreenX, float fScreenY)
	if (m_fAtlasScreenX != fScreenX || m_fAtlasScreenY != fScreenY)
	{
		m_matWorldAtlas._41 = fScreenX;
		m_matWorldAtlas._42 = fScreenY;
		m_fAtlasScreenX = fScreenX;
		m_fAtlasScreenY = fScreenY;
	}

///Add
#if defined(__BL_SHADER__)
	CShaderManager::Instance().Begin();
#endif

//Add to end of void CPythonMiniMap::RenderAtlas(float fScreenX, float fScreenY)
#if defined(__BL_SHADER__)
	IShader* shader = CShaderManager::Instance().GetShader(IShader::EType::ATLAS_SPOTLIGHT);
	if (shader)
	{
		CInstanceBase* pkInst = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (pkInst)
		{
			TPixelPosition kInstPos;
			pkInst->NEW_GetPixelPosition(&kInstPos);

			CAtlasSpotlightShader* atlasShader = static_cast<CAtlasSpotlightShader*>(shader);
			std::vector<D3DXVECTOR2> centerPoints;

			float fx, fy;
			
			__GlobalPositionToAtlasPosition(kInstPos.x, kInstPos.y, &fx, &fy);
			centerPoints.push_back(D3DXVECTOR2(fScreenX + fx, fScreenY + fy));

			__GlobalPositionToAtlasPosition(136 * 100, 880 * 100, &fx, &fy);
			centerPoints.push_back(D3DXVECTOR2(fScreenX + fx, fScreenY + fy));

			__GlobalPositionToAtlasPosition(593 * 100, 71 * 100, &fx, &fy);
			centerPoints.push_back(D3DXVECTOR2(fScreenX + fx, fScreenY + fy));

			atlasShader->SetPoints(centerPoints);
			atlasShader->SetTextureSize(D3DXVECTOR2(m_fAtlasImageSizeX, m_fAtlasImageSizeY));
		}
	}
	CShaderManager::Instance().End(IShader::ERenderType::ATLAS);
#endif