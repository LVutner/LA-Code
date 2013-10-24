#include "pch_script.h"
#include "UIStatic.h"
#include "UIAnimatedStatic.h"

using namespace luabind;

#pragma optimize("s",on)

void CUIStatic::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUILines>("CUILines")
		.def("SetFont",				&CUILines::SetFont)
		.def("SetText",				&CUILines::SetText)
		.def("SetTextST",			&CUILines::SetTextST)
		.def("GetText",				&CUILines::GetText)
		.def("SetElipsis",			&CUILines::SetEllipsis)
		.def("SetTextColor",		&CUILines::SetTextColor),


		class_<CUIStatic, CUIWindow>("CUIStatic")
		.def(						constructor<>())
		.def("TextControl",			&CUIStatic::TextItemControl)
		.def("SetText",				&CUIStatic::SetText)
		.def("SetTextST",			&CUIStatic::SetTextST)
		.def("GetText",				&CUIStatic::GetText)
		.def("SetFont",				&CUIStatic::SetFont)
		.def("GetFont",				&CUIStatic::GetFont)
		.def("SetColor",			&CUIStatic::SetColor)
		.def("GetColor",			&CUIStatic::GetColor)
		.def("SetTextColor",		&CUIStatic::SetTextColor)
		.def("GetTextColor",		&CUIStatic::GetTextColor)
		.def("SetTextAlignment",	&CUIStatic::SetTextAlignment)
		.def("SetVTextAlignment",	&CUIStatic::SetVTextAlignment)
		.def("SetEllipsis",			&CUIStatic::SetEllipsis)
		.def("SetTextOffset",		&CUIStatic::SetTextOffset)
		.def("SetTextureColor",			&CUIStatic::SetTextureColor)
		.def("GetTextureColor",			&CUIStatic::GetTextureColor)
		.def("InitTexture",			&CUIStatic::InitTexture )
		.def("SetTextureOffset",	&CUIStatic::SetTextureOffset )
		.def("SetTextureRect",		&CUIStatic::SetTextureRect_script)
		.def("SetOriginalRect",		(void(CUIStatic::*)(float,float,float,float))&CUIStatic::SetTextureRect)
		.def("SetStretchTexture",	&CUIStatic::SetStretchTexture)
		.def("GetStretchTexture",	&CUIStatic::GetStretchTexture)
		.def("SetTextAlign",		&CUIStatic::SetTextAlign_script)
		.def("GetTextAlign",		&CUIStatic::GetTextAlign_script)
		.def("GetTextureRect",		&CUIStatic::GetTextureRect_script)
		.def("SetComplexMode",	&CUIStatic::SetTextComplexMode)
		.def("IsComplexMode",	&CUIStatic::IsTextComplexMode),

		class_<CUITextWnd, CUIWindow>("CUITextWnd")
		.def(						constructor<>())
		.def("AdjustHeightToText",	&CUITextWnd::AdjustHeightToText)
		.def("AdjustWidthToText",	&CUITextWnd::AdjustWidthToText)
		.def("SetText",				&CUITextWnd::SetText)
		.def("SetTextST",			&CUITextWnd::SetTextST)
		.def("GetText",				&CUITextWnd::GetText)
		.def("SetFont",				&CUITextWnd::SetFont)
		.def("GetFont",				&CUITextWnd::GetFont)
		.def("SetTextColor",		&CUITextWnd::SetTextColor)
		.def("GetTextColor",		&CUITextWnd::GetTextColor)
		.def("SetTextComplexMode",	&CUITextWnd::SetTextComplexMode)
		.def("SetTextAlignment",	&CUITextWnd::SetTextAlignment)
		.def("SetVTextAlignment",	&CUITextWnd::SetVTextAlignment)
		.def("SetEllipsis",			&CUITextWnd::SetEllipsis)
		.def("SetTextOffset",		&CUITextWnd::SetTextOffset)
	];
}