// BASManager.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "bus/generic_gui/src/TransActiveWinApp.h"


// CBASManagerApp:
// �йش����ʵ�֣������ BASManager.cpp
//

class CBASManagerApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CBASManagerApp();

// ��д
	public:

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CBASManagerApp theApp;