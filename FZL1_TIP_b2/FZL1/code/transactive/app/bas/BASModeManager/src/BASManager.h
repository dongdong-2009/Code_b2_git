// BASManager.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "bus/generic_gui/src/TransActiveWinApp.h"


// CBASManagerApp:
// 有关此类的实现，请参阅 BASManager.cpp
//

class CBASManagerApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CBASManagerApp();

// 重写
	public:

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CBASManagerApp theApp;