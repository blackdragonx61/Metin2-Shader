# blackdragonx61
# Mali
# __BL_SHADER__
# 30.10.2025

import ui
import shaderMgr

class BLACKSUBWINDOW(ui.ThinBoardCircle):
	def __init__(self):
		super(BLACKSUBWINDOW, self).__init__()
		self.SetBlurShaderState(False)

	def __del__(self):
		super(BLACKSUBWINDOW, self).__del__()
	
	def OnRender(self):
		shaderMgr.Begin()
	
	def OnRenderEnd(self):
		shaderMgr.End()
	
	def SetBlurShaderState(self, is_show):
		if is_show:
			shaderMgr.ShowShader(shaderMgr.SHADER_TYPE_UI_BLUR)
		else:
			shaderMgr.HideShader(shaderMgr.SHADER_TYPE_UI_BLUR)

class BLACKSHADERUITEST(ui.BoardWithTitleBar):
	def __init__(self):
		ui.BoardWithTitleBar.__init__(self)

		self.__LoadWindow()

	def __del__(self):
		ui.BoardWithTitleBar.__del__(self)
		self.sub_wnd = None
		self.text = None
		self.text2 = None
		self.img = None
		self.blur_btn = None
		self.text3 = None

	def __LoadWindow(self):
		self.SetSize(300, 300)
		self.SetCenterPosition()
		self.SetCloseEvent(ui.__mem_func__(self.Hide))
		self.SetTitleName("BLACK SHADER TEST")
		self.AddFlag("movable")

		self.sub_wnd = BLACKSUBWINDOW()
		self.sub_wnd.SetParent(self)
		self.sub_wnd.SetPosition(20, 70)
		self.sub_wnd.SetSize(260, 200)
		self.sub_wnd.Show()

		self.text = ui.TextLine()
		self.text.SetParent(self.sub_wnd)
		self.text.SetPosition(0, 5)
		self.text.SetWindowHorizontalAlignCenter()
		self.text.SetHorizontalAlignCenter()
		self.text.SetText("BLACKDRAGONX61")
		self.text.Show()

		self.text2 = ui.TextLine()
		self.text2.SetParent(self.sub_wnd)
		self.text2.SetPosition(0, 15)
		self.text2.SetWindowHorizontalAlignCenter()
		self.text2.SetHorizontalAlignCenter()
		self.text2.SetText("MALI")
		self.text2.Show()

		self.img = ui.AniImageBox()
		self.img.SetParent(self.sub_wnd)
		self.img.AppendImage("d:/ymir work/ui/game/questboard/5123.tga")
		self.img.AppendImage("d:/ymir work/ui/game/questboard/2061.tga")
		self.img.AppendImage("d:/ymir work/ui/game/questboard/2205.tga")
		self.img.AppendImage("d:/ymir work/ui/game/questboard/2313.tga")
		self.img.SetDelay(6)
		self.img.SetPosition(20, 30)
		self.img.Show()

		self.blur_btn = ui.ToggleButton()
		self.blur_btn.SetParent(self)
		self.blur_btn.SetUpVisual("d:/ymir work/ui/public/middle_button_01.sub")
		self.blur_btn.SetOverVisual("d:/ymir work/ui/public/middle_button_02.sub")
		self.blur_btn.SetDownVisual("d:/ymir work/ui/public/middle_button_03.sub")
		self.blur_btn.SetText("Blur")
		self.blur_btn.SetPosition(20, 30)
		self.blur_btn.SetToggleDownEvent(ui.__mem_func__(self.__OnBlurToggleDown))
		self.blur_btn.SetToggleUpEvent(ui.__mem_func__(self.__OnBlurToggleUp))
		self.blur_btn.Show()

		self.text3 = ui.TextLine()
		self.text3.SetParent(self)
		self.text3.SetPosition(90, 33)
		self.text3.SetText("Shader: Disabled")
		self.text3.Show()
	
	def __OnBlurToggleDown(self):
		if self.sub_wnd:
			self.sub_wnd.SetBlurShaderState(True)
			self.text3.SetText("Shader: Enabled")
	
	def __OnBlurToggleUp(self):
		if self.sub_wnd:
			self.sub_wnd.SetBlurShaderState(False)
			self.text3.SetText("Shader: Disabled")

	def OnPressEscapeKey(self):
		self.Hide()
		return True

black = BLACKSHADERUITEST()
black.Show()
