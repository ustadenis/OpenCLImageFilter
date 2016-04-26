
// OpenCLImageFilterDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "OpenCLImageFilter.h"
#include "OpenCLImageFilterDlg.h"
#include "afxdialogex.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_ID_OPENCL_INIT 0
#define TIMER_ID_PLATFORMS_READY 1
#define TIMER_ID_DEVICES_READY 2

static void InitOpenCL(PVOID* param);
static void GetPlatformsThread(PVOID* param);
static void GetDevicesThread(PVOID* param);

// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// диалоговое окно COpenCLImageFilterDlg



COpenCLImageFilterDlg::COpenCLImageFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COpenCLImageFilterDlg::IDD, pParent)
	, m_sFindDir(_T(""))
	, m_sOpenCLStatus(_T("Не инициализировано"))
	, m_nEdge(5)
	, m_nNoizeLevel(2)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COpenCLImageFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sFindDir);
	DDX_Text(pDX, IDC_EDIT2, m_nEdge);
	DDX_Text(pDX, IDC_EDIT3, m_nNoizeLevel);
	DDX_Text(pDX, IDC_OPENCLSTATUS, m_sOpenCLStatus);
	DDX_Control(pDX, IDC_IMAGE, mImage);
	DDX_Control(pDX, IDC_IMAGEFILTERED, mImageFiltered);
	DDX_Control(pDX, IDC_IMAGEFILTERED2, mImageNoize);
	DDX_Control(pDX, IDC_STARTBUTTON, m_StartButton);
	DDX_Control(pDX, IDC_GETPLATFORMSBUTTON, m_GetPLatformsButton);
	DDX_Control(pDX, IDC_GETDEVICESBUTTON, m_GetDevicesButton);
	DDX_Control(pDX, IDC_ADDNOISEBUTTON, m_NoizeButton);
	DDX_Control(pDX, IDC_PLATFORMSCOMBO, m_PlatformsListBox);
	DDX_Control(pDX, IDC_DEVICESCOMBO, m_GetDevicesListBox);
}

BEGIN_MESSAGE_MAP(COpenCLImageFilterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_GETPLATFORMSBUTTON, &COpenCLImageFilterDlg::OnBnClickedGetplatformsbutton)
	ON_CBN_SELCHANGE(IDC_PLATFORMSCOMBO, &COpenCLImageFilterDlg::OnCbnSelchangePlatformscombo)
	ON_BN_CLICKED(IDC_GETDEVICESBUTTON, &COpenCLImageFilterDlg::OnBnClickedGetdevicesbutton)
	ON_CBN_SELCHANGE(IDC_DEVICESCOMBO, &COpenCLImageFilterDlg::OnCbnSelchangeDevicescombo)
	ON_BN_CLICKED(IDC_ADDNOISEBUTTON, &COpenCLImageFilterDlg::OnBnClickedAddnoisebutton)
	ON_BN_CLICKED(IDC_STARTBUTTON, &COpenCLImageFilterDlg::OnBnClickedStartbutton)
	ON_BN_CLICKED(IDC_BROWSEBUTTON, &COpenCLImageFilterDlg::OnBnClickedBrowsebutton)
END_MESSAGE_MAP()


// обработчики сообщений COpenCLImageFilterDlg

BOOL COpenCLImageFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна. Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	m_OpenCL = new COpenCL();

	m_bIsOpenCLInit = false;
	m_bIsPlatformsReady = false;
	m_bIsDevicesReady = false;

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void COpenCLImageFilterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок. Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void COpenCLImageFilterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR COpenCLImageFilterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COpenCLImageFilterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	if(nIDEvent == TIMER_ID_OPENCL_INIT)
	{
		if(m_bIsOpenCLInit)
		{
			m_sOpenCLStatus = "Инициализировано"; // Изменяем статус на UI
			m_NoizeButton.EnableWindow(true); // Включаем кнопку добавления шума
			UpdateData(false);
			KillTimer(nIDEvent);
		}
	}
	else if(nIDEvent == TIMER_ID_PLATFORMS_READY)
	{
		if(m_bIsPlatformsReady)
		{
			// Чистим комбо бокс перед заполнением
			for( int i = 0; i < m_PlatformsListBox.GetCount(); i++ )
			{
				m_PlatformsListBox.DeleteString(i);
			}

			// Заполняем комбо бокс
			for_each(platforms.begin(), platforms.end(),
					[&](Platform &platform)
			{
				STRING_CLASS pInfo;
				static int num = 0;

				platform.getInfo(CL_PLATFORM_NAME, &pInfo);
				CStringW str(pInfo.data());
				m_PlatformsListBox.AddString(str);
			});
	
			m_PlatformsListBox.SetCurSel(0);
			m_GetPLatformsButton.EnableWindow(false);

			UpdateData(false);
			KillTimer(nIDEvent);
		}
	}
	else if(nIDEvent == TIMER_ID_DEVICES_READY)
	{
		if(m_bIsDevicesReady)
		{
			// Чистим комбо бокс перед заполнением
			for( int i = 0; i < m_GetDevicesListBox.GetCount(); i++ )
			{
				m_GetDevicesListBox.DeleteString(i);
			}

			// Заполняем комбо бокс
			for_each(devices.begin(), devices.end(),
					[&](Device &device)
			{
				STRING_CLASS pInfo;
				static int num = 0;

				device.getInfo(CL_DEVICE_NAME, &pInfo);
				CStringW str(pInfo.data());
				m_GetDevicesListBox.AddString(str);
			});
	
			m_GetDevicesListBox.SetCurSel(0);
			m_GetDevicesButton.EnableWindow(false);

			UpdateData(false);
			KillTimer(nIDEvent);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void COpenCLImageFilterDlg::OnBnClickedGetplatformsbutton()
{
	// TODO: добавьте свой код обработчика уведомлений
	// Запускаем поток получения платформ
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetPlatformsThread, this, NULL, NULL);
	m_GetPLatformsButton.EnableWindow(false); // Выключаем кнопку 
	SetTimer(TIMER_ID_PLATFORMS_READY, 500, NULL); // Запускаем таймер
}


void COpenCLImageFilterDlg::OnCbnSelchangePlatformscombo()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_OpenCL->SetSelectedPlatform(m_PlatformsListBox.GetCurSel()); // Выбираем платформу
	m_PlatformsListBox.EnableWindow(false); // Выключаем кнопку выбора платформ
	m_GetDevicesButton.EnableWindow(true); // включаем кнопку выбора девайса
}


void COpenCLImageFilterDlg::OnBnClickedGetdevicesbutton()
{
	// TODO: добавьте свой код обработчика уведомлений
	// Запускаем поток получения девайсов
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetDevicesThread, this, NULL, NULL);
	m_GetDevicesButton.EnableWindow(false); // Выключаем кнопку 
	SetTimer(TIMER_ID_DEVICES_READY, 500, NULL); // Запускаем таймер
}


void COpenCLImageFilterDlg::OnCbnSelchangeDevicescombo()
{
	// TODO: добавьте свой код обработчика уведомлений
	m_OpenCL->SetSelectedDevice(m_GetDevicesListBox.GetCurSel()); // Выбираем устройство
	m_GetDevicesListBox.EnableWindow(false); // Выключаем кнопку выбора устройств
	// Запускаем поток инициализации OpenCL
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitOpenCL, this, NULL, NULL);
	SetTimer(TIMER_ID_OPENCL_INIT, 500, NULL); // Запускаем таймер
}


void COpenCLImageFilterDlg::OnBnClickedAddnoisebutton()
{
	// TODO: добавьте свой код обработчика уведомлений
	if(m_OpenCL != NULL && m_bIsOpenCLInit && m_BmpIn != NULL)
	{
		UpdateData(true);
		// Получаеем код OpenCL
		HMODULE hModule = ::GetModuleHandle(nullptr);
		HRSRC hRes = ::FindResource(hModule, MAKEINTRESOURCE(IDR_KERNEL1), L"KERNEL");
		HGLOBAL res = ::LoadResource(hModule, hRes);
		char *code = (char *)::LockResource(res);
		// получаем высоту и ширину изображения
		int width = m_BmpIn->GetWidth();
		int height = m_BmpIn->GetHeight();

		// Загружаем и компилируем kernel
		if(m_OpenCL->LoadKernel("AddNoize", code) != 0)
		{ /* Error */ }

		// Декодим изображение
		Gdiplus::BitmapData* bitmapDataIn = new Gdiplus::BitmapData();
		m_BmpIn->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, bitmapDataIn);
		
		int stride = bitmapDataIn->Stride; // Получаем страйд
		int n = width * height; // Количество пикселей

		UINT* in = (UINT*)bitmapDataIn->Scan0;
		UINT* out = new UINT[n];

		for(int i = 0; i < n; i++)
		{
			out[i] = 0;
		}

		// Запускаем krenel
		m_OpenCL->RunAddNoizeKernel(in, out, m_nNoizeLevel, width, height);

		m_BmpIn->UnlockBits(bitmapDataIn);
 
		// Кодируем байты обратно в изображение
		m_BmpNoize = reinterpret_cast<Gdiplus::Bitmap*>(Gdiplus::Bitmap::Bitmap(width, height, stride, PixelFormat32bppARGB, (BYTE*)out).Clone(Gdiplus::Rect(0, 0, width, height), PixelFormat32bppARGB));
		// Показываем изображение на экране
		mImageNoize.SetImage(m_BmpNoize);
		// Включаем кнопку фильтрации
		m_StartButton.EnableWindow(true);
	} else {
		/* Not Init */
	}
}


void COpenCLImageFilterDlg::OnBnClickedStartbutton()
{
	// TODO: добавьте свой код обработчика уведомлений
	if(m_OpenCL != NULL && m_bIsOpenCLInit && m_BmpNoize != NULL)
	{
		UpdateData(true);
		// Получаеем код OpenCL
		HMODULE hModule = ::GetModuleHandle(nullptr);
		HRSRC hRes = ::FindResource(hModule, MAKEINTRESOURCE(IDR_KERNEL1), L"KERNEL");
		HGLOBAL res = ::LoadResource(hModule, hRes);
		char *code = (char *)::LockResource(res);
		// получаем высоту и ширину изображения
		int width = m_BmpNoize->GetWidth();
		int height = m_BmpNoize->GetHeight();

		// Загружаем и компилируем kernel
		if(m_OpenCL->LoadKernel("Filter", code) != 0)
		{ /* Error */ }

		// Декодим изображение
		Gdiplus::BitmapData* bitmapDataNoize = new Gdiplus::BitmapData();
		m_BmpNoize->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, bitmapDataNoize);

		
		int stride = bitmapDataNoize->Stride;
		int n = width * height;

		UINT* in = (UINT*)bitmapDataNoize->Scan0;
		UINT* out = new UINT[n];

		for(int i = 0; i < n; i++)
		{
			out[i] = 0;
		}

		// Запускаем krenel
		m_OpenCL->RunFilterKernel(in, out, width, height, m_nEdge);

		m_BmpNoize->UnlockBits(bitmapDataNoize);

		// Кодируем байты обратно в изображение
		m_BmpOut = reinterpret_cast<Gdiplus::Bitmap*>(Gdiplus::Bitmap::Bitmap(width, height, stride, PixelFormat32bppARGB, (BYTE*)out).Clone(Gdiplus::Rect(0, 0, width, height), PixelFormat32bppARGB));
		// Показываем изображение на экране
		mImageFiltered.SetImage(m_BmpOut);
	} else {
		/* Not Init */
	}
}


void COpenCLImageFilterDlg::OnBnClickedBrowsebutton()
{
	// TODO: добавьте свой код обработчика уведомлений
	CFileDialog fileDlg(true);
	fileDlg.DoModal(); // Открываем диалог выбора изображения
	UpdateData(true);
	m_sFindDir = fileDlg.GetFolderPath() + "\\" + fileDlg.GetFileName(); // Путь до изображения
	UpdateData(false);

	if(!m_sFindDir.IsEmpty())
	{
		mImage.Clear();

		Gdiplus::Image jpg(m_sFindDir);
		m_BmpIn = reinterpret_cast<Gdiplus::Bitmap*>(jpg.Clone()); // Получаем изображение
		mImage.SetImage(m_BmpIn); // Показываем изображение на экране
	}
}

void COpenCLImageFilterDlg::InitOpenCL(PVOID* param) // Поток инициализации OpenCL
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	if(dlg->m_OpenCL->CreateContext() != 0)
	{
		dlg->MessageBox(L"CreateContext::Error");
		return;
	}

	dlg->m_bIsOpenCLInit = true; // Выставляем флаг готовности
}


void COpenCLImageFilterDlg::GetPlatformsThread(PVOID* param) // Поток получаения платформ
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	dlg->platforms = dlg->m_OpenCL->GetPlatforms();

	dlg->m_bIsPlatformsReady = true; // Выставляем флаг готовности
}


void COpenCLImageFilterDlg::GetDevicesThread(PVOID* param) // Поток получаения устройств
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	dlg->devices = dlg->m_OpenCL->GetDevices();

	dlg->m_bIsDevicesReady = true; // Выставляем флаг готовности
}