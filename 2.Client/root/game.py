#Add
if app.__BL_SHADER__:
	import shaderMgr

#Find
	def OnRender(self):
		app.RenderGame()

#Change
	def OnRender(self):
		if app.__BL_SHADER__:
			shaderMgr.Begin()

		app.RenderGame()

#Add new func
	if app.__BL_SHADER__:
		def OnRenderEnd(self):
			grp.SetGameRenderState()
			shaderMgr.End(shaderMgr.SHADER_RENDER_TYPE_GAME_SCREEN)
			grp.SetInterfaceRenderState()