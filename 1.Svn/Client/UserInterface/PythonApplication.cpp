//Find in void CPythonApplication::RenderGame()
	if (!PERF_CHECKER_RENDER_GAME)
	{

///Add
#if defined(__BL_SHADER__)
		m_ShaderManager.Begin();
#endif

//Find
		m_pyBackground.RenderAfterLensFlare();
		
		return;
	}

///Change
		m_pyBackground.RenderAfterLensFlare();

#if defined(__BL_SHADER__)
		m_ShaderManager.End(IShader::ERenderType::GAME_SCREEN);
#endif

		return;
	}

//Find
				rkBG.ReleaseCharacterShadowTexture();

///Add
#if defined(__BL_SHADER__)
				m_ShaderManager.LostDevice();
#endif

//Find
					rkBG.CreateCharacterShadowTexture();

///Add
#if defined(__BL_SHADER__)
					m_ShaderManager.RestoreDevice();
#endif

//Find
		CGraphicImageInstance::CreateSystem(32);

///Add
#if defined(__BL_SHADER__)
		if (!m_ShaderManager.Initialize())
			return false;
#endif

//Find
	DestroyCollisionInstanceSystem();

///Add
#if defined(__BL_SHADER__)
	m_ShaderManager.Destroy();
#endif