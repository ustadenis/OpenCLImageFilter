
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
#define TIMER_ID_IMAGE_FILTER 3

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
	, m_nEdge(4)
	, m_nNoizeLevel(15)
	, m_BmpIn(nullptr)
	, m_BmpNoize(nullptr)
	, m_BmpOut(nullptr)
	, m_nTime(0)
	, m_bUseAllDevices(FALSE)
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
	DDX_Control(pDX, IDC_ADDNOISEBUTTON, m_NoizeButton);
	DDX_Control(pDX, IDC_PLATFORMSCOMBO, m_PlatformsListBox);
	DDX_Control(pDX, IDC_DEVICESCOMBO, m_GetDevicesListBox);
	DDX_Text(pDX, IDC_TIME, m_nTime);
	DDX_Check(pDX, IDC_USEALLDEVICESCHECK, m_bUseAllDevices);
	DDX_Control(pDX, IDC_USEALLDEVICESCHECK, m_UseAllDevicesButton);
}

BEGIN_MESSAGE_MAP(COpenCLImageFilterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_GETPLATFORMSBUTTON, &COpenCLImageFilterDlg::OnBnClickedGetplatformsbutton)
	ON_CBN_SELCHANGE(IDC_PLATFORMSCOMBO, &COpenCLImageFilterDlg::OnCbnSelchangePlatformscombo)
	ON_CBN_SELCHANGE(IDC_DEVICESCOMBO, &COpenCLImageFilterDlg::OnCbnSelchangeDevicescombo)
	ON_BN_CLICKED(IDC_ADDNOISEBUTTON, &COpenCLImageFilterDlg::OnBnClickedAddnoisebutton)
	ON_BN_CLICKED(IDC_STARTBUTTON, &COpenCLImageFilterDlg::OnBnClickedStartbutton)
	ON_BN_CLICKED(IDC_BROWSEBUTTON, &COpenCLImageFilterDlg::OnBnClickedBrowsebutton)
	ON_BN_CLICKED(IDC_USEALLDEVICESCHECK, &COpenCLImageFilterDlg::OnBnClickedUsealldevicescheck)
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

	m_bIsOpenCLInit = false;
	m_bIsPlatformsReady = false;
	m_bIsDevicesReady = false;
	m_bIsImageFiltered = false;

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
	switch (nIDEvent)
	{
		case TIMER_ID_OPENCL_INIT:
		{
			if(m_bIsOpenCLInit)
			{
				m_sOpenCLStatus = "Инициализировано"; // Изменяем статус на UI
				UpdateData(false);
				KillTimer(nIDEvent);
			}
			break;
		}
		case TIMER_ID_PLATFORMS_READY:
		{
			if(m_bIsPlatformsReady)
			{
				// Чистим комбо бокс перед заполнением
				m_PlatformsListBox.ResetContent();

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
	
				// выбирать можно, если есть из чего
				if (m_PlatformsListBox.GetCount() > 0)
				{
					m_PlatformsListBox.SetCurSel(0);
				
					// включение списка и кнопки
					m_GetPLatformsButton.EnableWindow();
					m_PlatformsListBox.EnableWindow();

					// имитация изменения выбора платформы
					OnCbnSelchangePlatformscombo();
				}

				UpdateData(false);
				KillTimer(nIDEvent);
			}
			break;
		}
		case TIMER_ID_DEVICES_READY:
		{
			if(m_bIsDevicesReady)
			{
				// Чистим комбо бокс перед заполнением
				m_GetDevicesListBox.ResetContent();

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
	
				if (m_GetDevicesListBox.GetCount() > 0)
				{
					m_GetDevicesListBox.SetCurSel(0);
				
					// включение списков
					m_PlatformsListBox.EnableWindow();
					if(!m_bUseAllDevices)
						m_GetDevicesListBox.EnableWindow();

					// имитация выбора устройства
					OnCbnSelchangeDevicescombo();
				}

				m_UseAllDevicesButton.EnableWindow(TRUE);

				UpdateData(false);
				KillTimer(nIDEvent);
			}
			break;
		}
		case TIMER_ID_IMAGE_FILTER:
		{
			if(m_bIsImageFiltered)
			{
				mImageFiltered.SetImage(m_BmpOut);
				m_bIsImageFiltered = false;
				m_StartButton.EnableWindow(TRUE);
				m_nTime = mTimer.Now() - mTimeStart;
				UpdateData(FALSE);
				KillTimer(TIMER_ID_IMAGE_FILTER);
			}
			break;
		}
		default:
			break;
	}
	
	CDialogEx::OnTimer(nIDEvent);
}


void COpenCLImageFilterDlg::OnBnClickedGetplatformsbutton()
{
	// Запускаем поток получения платформ
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetPlatformsThread, this, NULL, NULL);

	m_GetPLatformsButton.EnableWindow(FALSE); // Выключаем кнопку 
	m_PlatformsListBox.EnableWindow(FALSE); // Выключаем список выбора платформ
	m_GetDevicesListBox.EnableWindow(FALSE); // включаем список выбора девайса

	SetTimer(TIMER_ID_PLATFORMS_READY, 500, NULL); // Запускаем таймер
}


void COpenCLImageFilterDlg::OnCbnSelchangePlatformscombo()
{
	m_OpenCL.SetSelectedPlatform(m_PlatformsListBox.GetCurSel()); // Выбираем платформу
	
	m_PlatformsListBox.EnableWindow(FALSE); // Выключаем список выбора платформ
	m_GetDevicesListBox.EnableWindow(FALSE); // включаем список выбора девайса

	// определение устройств
	GetDevices();
}

void COpenCLImageFilterDlg::GetDevices()
{
	// Запускаем поток получения девайсов
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetDevicesThread, this, NULL, NULL);
	SetTimer(TIMER_ID_DEVICES_READY, 500, NULL); // Запускаем таймер
}


void COpenCLImageFilterDlg::OnCbnSelchangeDevicescombo()
{
	m_OpenCL.SetSelectedDevice(m_GetDevicesListBox.GetCurSel()); // Выбираем устройство

	// Запускаем поток инициализации OpenCL
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitOpenCL, this, NULL, NULL);
	SetTimer(TIMER_ID_OPENCL_INIT, 500, NULL); // Запускаем таймер
}


void COpenCLImageFilterDlg::OnBnClickedAddnoisebutton()
{
	// TODO: добавьте свой код обработчика уведомлений
	if(m_BmpIn != NULL)
	{
		UpdateData();

		// получаем высоту и ширину изображения
		int width = m_BmpIn->GetWidth();
		int height = m_BmpIn->GetHeight();

		// изначально зашумленное изображение совпадает с исходным
		m_BmpNoize = m_BmpIn->Clone(Gdiplus::Rect(0, 0, width, height), PixelFormat32bppRGB);

		// Декодим изображение
		Gdiplus::BitmapData bitmapDataIn;
		Gdiplus::BitmapData bitmapDataOut;

		m_BmpIn->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, &bitmapDataIn);
		m_BmpNoize->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeWrite, PixelFormat32bppRGB, &bitmapDataOut);

		int stride = bitmapDataIn.Stride; // Получаем страйд
		int n = width * height; // Количество пикселей

		UINT* in = (UINT*)bitmapDataIn.Scan0;
		UINT* out = (UINT*)bitmapDataOut.Scan0;

		// Зашумляем изображение
		AddNoise(out, width, height);

		// сохранение изменений
		m_BmpIn->UnlockBits(&bitmapDataIn);
		m_BmpNoize->UnlockBits(&bitmapDataOut);

		// Показываем изображение на экране
		mImageNoize.SetImage(m_BmpNoize);
	
		// Включаем кнопку фильтрации
		m_StartButton.EnableWindow();
	} else {
		/* Not Init */
	}
}

void COpenCLImageFilterDlg::AddNoise(unsigned int* image, int width, int height)
{
	int pixelcount = width * height; // Количество пикселей
	UpdateData(true);

	// Заполняем маску шума
	srand((UINT)time(0));
	for (int i = 0; i < m_nNoizeLevel * pixelcount / 100; i++)
	{
		int x = rand() % width;
		int y = rand() % height;
		int offset = rand() % 24;
		int intensity = rand() % 0xFF;
		image[y * width + x] = (intensity << offset);
	}
}


void COpenCLImageFilterDlg::OnBnClickedStartbutton()
{
	UpdateData(TRUE);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFilter, this, NULL, NULL);
	if(m_bIsOpenCLInit)
	{
		SetTimer(TIMER_ID_IMAGE_FILTER, 100, NULL); // Запускаем таймер
		m_StartButton.EnableWindow(FALSE);
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
		m_BmpIn = Gdiplus::Bitmap::FromFile(m_sFindDir); // Получаем изображение
		
		mImage.Clear();
		mImage.SetImage(m_BmpIn); // Показываем изображение на экране

		m_NoizeButton.EnableWindow();
	}
}

void COpenCLImageFilterDlg::InitOpenCL(PVOID* param) // Поток инициализации OpenCL
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	if(dlg->m_OpenCL.CreateContext(dlg->m_bUseAllDevices) != 0)
	{
		dlg->MessageBox(L"CreateContext::Error");
		return;
	}

	dlg->m_bIsOpenCLInit = true; // Выставляем флаг готовности
}


void COpenCLImageFilterDlg::GetPlatformsThread(PVOID* param) // Поток получаения платформ
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	dlg->platforms = dlg->m_OpenCL.GetPlatforms();

	dlg->m_bIsPlatformsReady = true; // Выставляем флаг готовности
}


void COpenCLImageFilterDlg::GetDevicesThread(PVOID* param) // Поток получаения устройств
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	dlg->devices = dlg->m_OpenCL.GetDevices();

	dlg->m_bIsDevicesReady = true; // Выставляем флаг готовности
}

void COpenCLImageFilterDlg::StartFilter(PVOID* param)
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	// TODO: добавьте свой код обработчика уведомлений
	if(dlg->m_bIsOpenCLInit && dlg->m_BmpNoize != NULL)
	{
		// Получаеем код OpenCL
		HMODULE hModule = ::GetModuleHandle(nullptr);
		HRSRC hRes = ::FindResource(hModule, MAKEINTRESOURCE(IDR_KERNEL1), L"KERNEL");
		HGLOBAL res = ::LoadResource(hModule, hRes);
		char *code = (char *)::LockResource(res);
		// получаем высоту и ширину изображения
		int width = dlg->m_BmpNoize->GetWidth();
		int height = dlg->m_BmpNoize->GetHeight();

		// Загружаем и компилируем kernel
		if(dlg->m_OpenCL.LoadKernel("Filter", code) != 0)
		{
			/* Error */
			return;
		}

		dlg->m_BmpOut = dlg->m_BmpNoize->Clone(Gdiplus::Rect(0, 0, width, height), PixelFormat32bppRGB);

		// Декодим изображение
		Gdiplus::BitmapData bitmapDataNoise;
		Gdiplus::BitmapData bitmapDataOut;

		dlg->m_BmpNoize->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, &bitmapDataNoise);
		dlg->m_BmpOut->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeWrite, PixelFormat32bppRGB, &bitmapDataOut);

		
		int stride = bitmapDataNoise.Stride;
		int n = width * height;

		UINT* in = (UINT*)bitmapDataNoise.Scan0;
		UINT* out = (UINT*)bitmapDataOut.Scan0;

		dlg->mTimeStart = dlg->mTimer.Now();
		int edge = dlg->m_nEdge;
		if(edge % 2 != 0)
			edge += 1;
		// Запускаем krenel
		dlg->m_OpenCL.RunFilterKernel(in, out, width, height, edge);

		dlg->m_BmpNoize->UnlockBits(&bitmapDataNoise);
		dlg->m_BmpOut->UnlockBits(&bitmapDataOut);

		dlg->m_bIsImageFiltered = true;
	} else {
		/* Not Init */
	}
}

void COpenCLImageFilterDlg::OnBnClickedUsealldevicescheck()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData(TRUE);
	if(m_bUseAllDevices)
	{
		m_GetDevicesListBox.EnableWindow(FALSE);
	}
	else
	{
		m_GetDevicesListBox.EnableWindow(TRUE);
	}
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitOpenCL, this, NULL, NULL);
	SetTimer(TIMER_ID_OPENCL_INIT, 500, NULL); // Запускаем таймер
	m_sOpenCLStatus = "Не инициализировано"; // Изменяем статус на UI
	UpdateData(FALSE);
}
