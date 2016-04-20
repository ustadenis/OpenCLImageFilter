
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

static void InitOpenCL(PVOID* param);

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
	, m_sOpenCLStatus(_T("Not Init"))
	, m_nEdge(2)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COpenCLImageFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, mImage);
	DDX_Text(pDX, IDC_EDIT1, m_sFindDir);
	DDX_Control(pDX, IDC_BUTTON2, m_InitButton);
	DDX_Text(pDX, IDC_OPENCLSTATUS, m_sOpenCLStatus);
	DDX_Control(pDX, IDC_IMAGEFILTERED, mImageFiltered);
	//  DDX_Control(pDX, IDC_COMBOBOXEX1, m_PlatformsComboBox);
	DDX_Control(pDX, IDC_LIST2, m_PlatformsListBox);
	DDX_Control(pDX, IDC_BUTTON4, m_GetPLatformsButton);
	DDX_Control(pDX, IDC_BUTTON5, m_GetDevicesButton);
	DDX_Control(pDX, IDC_LIST3, m_GetDevicesListBox);
	DDX_Control(pDX, IDC_BUTTON3, m_StartButton);
	DDX_Text(pDX, IDC_EDIT2, m_nEdge);
}

BEGIN_MESSAGE_MAP(COpenCLImageFilterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &COpenCLImageFilterDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &COpenCLImageFilterDlg::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON3, &COpenCLImageFilterDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &COpenCLImageFilterDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &COpenCLImageFilterDlg::OnBnClickedButton5)
	ON_LBN_SELCHANGE(IDC_LIST2, &COpenCLImageFilterDlg::OnLbnSelchangeList2)
	ON_LBN_SELCHANGE(IDC_LIST3, &COpenCLImageFilterDlg::OnLbnSelchangeList3)
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
	m_OpenCL = new COpenCL();

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



void COpenCLImageFilterDlg::OnBnClickedButton1()
{
	// TODO: добавьте свой код обработчика уведомлений
	CFileDialog fileDlg(true);
	fileDlg.DoModal();
	UpdateData(true);
	m_sFindDir = fileDlg.GetFolderPath() + "\\" + fileDlg.GetFileName();
	UpdateData(false);

	if(!m_sFindDir.IsEmpty())
	{
		mImage.Clear();

		Gdiplus::Image jpg(m_sFindDir);
		m_BmpIn = reinterpret_cast<Gdiplus::Bitmap*>(jpg.Clone());
		mImage.SetImage(m_BmpIn);
	}
}


void COpenCLImageFilterDlg::OnBnClickedButton2()
{
	// TODO: добавьте свой код обработчика уведомлений
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitOpenCL, this, NULL, NULL);
	m_InitButton.EnableWindow(false);
	SetTimer(0, 500, NULL);
}

void COpenCLImageFilterDlg::InitOpenCL(PVOID* param)
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	if(dlg->m_OpenCL->CreateContext() != 0)
	{
		dlg->MessageBox(L"CreateContext::Error");
		return;
	}

	dlg->m_bIsOpenCLInit = true;
}


void COpenCLImageFilterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	if(nIDEvent == 0)
	{
		if(m_bIsOpenCLInit)
		{
			m_sOpenCLStatus = "Init";
			m_StartButton.EnableWindow(true);
			UpdateData(false);
			KillTimer(nIDEvent);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void COpenCLImageFilterDlg::OnBnClickedButton3()
{
	// TODO: добавьте свой код обработчика уведомлений
	if(m_OpenCL != NULL && m_bIsOpenCLInit && m_BmpIn != NULL)
	{
		UpdateData(true);
		HMODULE hModule = ::GetModuleHandle(nullptr);
		HRSRC hRes = ::FindResource(hModule, MAKEINTRESOURCE(IDR_KERNEL1), L"KERNEL");
		HGLOBAL res = ::LoadResource(hModule, hRes);
		char *code = (char *)::LockResource(res);
		int width = m_BmpIn->GetWidth();
		int height = m_BmpIn->GetHeight();

		if(m_OpenCL->LoadKernel("MultMatrix", code) != 0)
		{ /* Error */ }

		Gdiplus::BitmapData* bitmapDataIn = new Gdiplus::BitmapData();
		m_BmpIn->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeWrite, PixelFormat32bppRGB, bitmapDataIn);

		
		int stride = bitmapDataIn->Stride;
		int n = stride * height;

		BYTE* in = (BYTE*)bitmapDataIn->Scan0;
		BYTE* out = new BYTE[n];

		for(int i = 0; i < n; i++)
		{
			out[i] = 0;
		}

		m_OpenCL->RunKernel(in, out, stride, height, m_nEdge);

		m_BmpIn->UnlockBits(bitmapDataIn);
 
		m_BmpOut = reinterpret_cast<Gdiplus::Bitmap*>(Gdiplus::Bitmap::Bitmap(width, height, stride, PixelFormat32bppRGB, out).Clone(Gdiplus::Rect(0, 0, width, height), PixelFormat32bppRGB));

		mImageFiltered.SetImage(m_BmpOut);
	} else {
		/* Not Init */
	}
}


void COpenCLImageFilterDlg::OnBnClickedButton4()
{
	// TODO: добавьте свой код обработчика уведомлений
	VECTOR_CLASS<Platform> platforms = m_OpenCL->GetPlatforms();

	for( int i = 0; i < m_PlatformsListBox.GetCount(); i++ )
	{
		m_PlatformsListBox.DeleteString(i);
	}

	for_each(platforms.begin(), platforms.end(),
			[&](Platform &platform)
	{
		STRING_CLASS pInfo;
		static int num = 0;

		platform.getInfo(CL_PLATFORM_NAME, &pInfo);
		CStringW str(pInfo.data());
		m_PlatformsListBox.AddString(str);
	});
	
	m_PlatformsListBox.SetSel(0);
	m_GetDevicesButton.EnableWindow(true);
	m_GetPLatformsButton.EnableWindow(false);
}


void COpenCLImageFilterDlg::OnBnClickedButton5()
{
	// TODO: добавьте свой код обработчика уведомлений
	VECTOR_CLASS<Device> devices = m_OpenCL->GetDevices();

	for( int i = 0; i < m_GetDevicesListBox.GetCount(); i++ )
	{
		m_GetDevicesListBox.DeleteString(i);
	}

	for_each(devices.begin(), devices.end(),
			[&](Device &device)
	{
		STRING_CLASS pInfo;
		static int num = 0;

		device.getInfo(CL_DEVICE_NAME, &pInfo);
		CStringW str(pInfo.data());
		m_GetDevicesListBox.AddString(str);
	});
	
	m_GetDevicesListBox.SetSel(0);
	//m_GetDevicesButton.EnableWindow(false);
	m_InitButton.EnableWindow(true);
}


void COpenCLImageFilterDlg::OnLbnSelchangeList2()
{
	// TODO: добавьте свой код обработчика уведомлений
	int index = 0;
	for( int i = 0; i < m_PlatformsListBox.GetCount(); i++ )
	{
		if(m_PlatformsListBox.GetSel( i ) > 0)
		{
			index = i;
			break;
		}
	}
	m_OpenCL->SetSelectedPlatform(index);
	m_PlatformsListBox.EnableWindow(false);
}


void COpenCLImageFilterDlg::OnLbnSelchangeList3()
{
	// TODO: добавьте свой код обработчика уведомлений
	int index = 0;
	for( int i = 0; i < m_GetDevicesListBox.GetCount(); i++ )
	{
		if(m_GetDevicesListBox.GetSel( i ) > 0)
		{
			index = i;
			break;
		}
	}
	m_OpenCL->SetSelectedDevice(index);
	m_GetDevicesListBox.EnableWindow(false);
}
