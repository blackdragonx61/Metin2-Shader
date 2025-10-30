//Find in void CWindow::Render()
		std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::Render));

///Add
#if defined(__BL_SHADER__)
		OnRenderEnd();
#endif

//Find
	void CWindow::OnRender()
	{
		...
	}

///Add
#if defined(__BL_SHADER__)
	void CWindow::OnRenderEnd()
	{
		if (!m_poHandler)
			return;

		if (!IsShow())
			return;

		PyCallClassMemberFunc(m_poHandler, "OnRenderEnd", BuildEmptyTuple());
	}
#endif