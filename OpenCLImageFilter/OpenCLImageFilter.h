
// OpenCLImageFilter.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// COpenCLImageFilterApp:
// О реализации данного класса см. OpenCLImageFilter.cpp
//

class COpenCLImageFilterApp : public CWinApp
{
public:
	COpenCLImageFilterApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern COpenCLImageFilterApp theApp;