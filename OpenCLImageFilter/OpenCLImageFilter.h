
// OpenCLImageFilter.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// COpenCLImageFilterApp:
// � ���������� ������� ������ ��. OpenCLImageFilter.cpp
//

class COpenCLImageFilterApp : public CWinApp
{
public:
	COpenCLImageFilterApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern COpenCLImageFilterApp theApp;