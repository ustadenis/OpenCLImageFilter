
// OpenCLImageFilterDlg.cpp : ���� ����������
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

// ���������� ���� CAboutDlg ������������ ��� �������� �������� � ����������

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ������ ����������� ����
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // ��������� DDX/DDV

// ����������
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


// ���������� ���� COpenCLImageFilterDlg



COpenCLImageFilterDlg::COpenCLImageFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COpenCLImageFilterDlg::IDD, pParent)
	, m_sFindDir(_T(""))
	, m_sOpenCLStatus(_T("�� ����������������"))
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


// ����������� ��������� COpenCLImageFilterDlg

BOOL COpenCLImageFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���������� ������ "� ���������..." � ��������� ����.

	// IDM_ABOUTBOX ������ ���� � �������� ��������� �������.
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

	// ������ ������ ��� ����� ����������� ����. ����� ������ ��� �������������,
	//  ���� ������� ���� ���������� �� �������� ����������
	SetIcon(m_hIcon, TRUE);			// ������� ������
	SetIcon(m_hIcon, FALSE);		// ������ ������

	// TODO: �������� �������������� �������������
	m_OpenCL = new COpenCL();

	m_bIsOpenCLInit = false;
	m_bIsPlatformsReady = false;
	m_bIsDevicesReady = false;

	return TRUE;  // ������� �������� TRUE, ���� ����� �� ������� �������� ����������
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

// ��� ���������� ������ ����������� � ���������� ���� ����� ��������������� ����������� ���� �����,
//  ����� ���������� ������. ��� ���������� MFC, ������������ ������ ���������� ��� �������������,
//  ��� ������������� ����������� ������� ��������.

void COpenCLImageFilterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �������� ���������� ��� ���������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ������������ ������ �� ������ ����������� ��������������
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ��������� ������
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ������� �������� ��� ������� ��� ��������� ����������� ������� ��� �����������
//  ���������� ����.
HCURSOR COpenCLImageFilterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void COpenCLImageFilterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �������� ���� ��� ����������� ��������� ��� ����� ������������
	if(nIDEvent == TIMER_ID_OPENCL_INIT)
	{
		if(m_bIsOpenCLInit)
		{
			m_sOpenCLStatus = "����������������"; // �������� ������ �� UI
			m_NoizeButton.EnableWindow(true); // �������� ������ ���������� ����
			UpdateData(false);
			KillTimer(nIDEvent);
		}
	}
	else if(nIDEvent == TIMER_ID_PLATFORMS_READY)
	{
		if(m_bIsPlatformsReady)
		{
			// ������ ����� ���� ����� �����������
			for( int i = 0; i < m_PlatformsListBox.GetCount(); i++ )
			{
				m_PlatformsListBox.DeleteString(i);
			}

			// ��������� ����� ����
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
			// ������ ����� ���� ����� �����������
			for( int i = 0; i < m_GetDevicesListBox.GetCount(); i++ )
			{
				m_GetDevicesListBox.DeleteString(i);
			}

			// ��������� ����� ����
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
	// TODO: �������� ���� ��� ����������� �����������
	// ��������� ����� ��������� ��������
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetPlatformsThread, this, NULL, NULL);
	m_GetPLatformsButton.EnableWindow(false); // ��������� ������ 
	SetTimer(TIMER_ID_PLATFORMS_READY, 500, NULL); // ��������� ������
}


void COpenCLImageFilterDlg::OnCbnSelchangePlatformscombo()
{
	// TODO: �������� ���� ��� ����������� �����������
	m_OpenCL->SetSelectedPlatform(m_PlatformsListBox.GetCurSel()); // �������� ���������
	m_PlatformsListBox.EnableWindow(false); // ��������� ������ ������ ��������
	m_GetDevicesButton.EnableWindow(true); // �������� ������ ������ �������
}


void COpenCLImageFilterDlg::OnBnClickedGetdevicesbutton()
{
	// TODO: �������� ���� ��� ����������� �����������
	// ��������� ����� ��������� ��������
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)GetDevicesThread, this, NULL, NULL);
	m_GetDevicesButton.EnableWindow(false); // ��������� ������ 
	SetTimer(TIMER_ID_DEVICES_READY, 500, NULL); // ��������� ������
}


void COpenCLImageFilterDlg::OnCbnSelchangeDevicescombo()
{
	// TODO: �������� ���� ��� ����������� �����������
	m_OpenCL->SetSelectedDevice(m_GetDevicesListBox.GetCurSel()); // �������� ����������
	m_GetDevicesListBox.EnableWindow(false); // ��������� ������ ������ ���������
	// ��������� ����� ������������� OpenCL
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)InitOpenCL, this, NULL, NULL);
	SetTimer(TIMER_ID_OPENCL_INIT, 500, NULL); // ��������� ������
}


void COpenCLImageFilterDlg::OnBnClickedAddnoisebutton()
{
	// TODO: �������� ���� ��� ����������� �����������
	if(m_OpenCL != NULL && m_bIsOpenCLInit && m_BmpIn != NULL)
	{
		UpdateData(true);
		// ��������� ��� OpenCL
		HMODULE hModule = ::GetModuleHandle(nullptr);
		HRSRC hRes = ::FindResource(hModule, MAKEINTRESOURCE(IDR_KERNEL1), L"KERNEL");
		HGLOBAL res = ::LoadResource(hModule, hRes);
		char *code = (char *)::LockResource(res);
		// �������� ������ � ������ �����������
		int width = m_BmpIn->GetWidth();
		int height = m_BmpIn->GetHeight();

		// ��������� � ����������� kernel
		if(m_OpenCL->LoadKernel("AddNoize", code) != 0)
		{ /* Error */ }

		// ������� �����������
		Gdiplus::BitmapData* bitmapDataIn = new Gdiplus::BitmapData();
		m_BmpIn->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, bitmapDataIn);
		
		int stride = bitmapDataIn->Stride; // �������� ������
		int n = width * height; // ���������� ��������

		UINT* in = (UINT*)bitmapDataIn->Scan0;
		UINT* out = new UINT[n];

		for(int i = 0; i < n; i++)
		{
			out[i] = 0;
		}

		// ��������� krenel
		m_OpenCL->RunAddNoizeKernel(in, out, m_nNoizeLevel, width, height);

		m_BmpIn->UnlockBits(bitmapDataIn);
 
		// �������� ����� ������� � �����������
		m_BmpNoize = reinterpret_cast<Gdiplus::Bitmap*>(Gdiplus::Bitmap::Bitmap(width, height, stride, PixelFormat32bppARGB, (BYTE*)out).Clone(Gdiplus::Rect(0, 0, width, height), PixelFormat32bppARGB));
		// ���������� ����������� �� ������
		mImageNoize.SetImage(m_BmpNoize);
		// �������� ������ ����������
		m_StartButton.EnableWindow(true);
	} else {
		/* Not Init */
	}
}


void COpenCLImageFilterDlg::OnBnClickedStartbutton()
{
	// TODO: �������� ���� ��� ����������� �����������
	if(m_OpenCL != NULL && m_bIsOpenCLInit && m_BmpNoize != NULL)
	{
		UpdateData(true);
		// ��������� ��� OpenCL
		HMODULE hModule = ::GetModuleHandle(nullptr);
		HRSRC hRes = ::FindResource(hModule, MAKEINTRESOURCE(IDR_KERNEL1), L"KERNEL");
		HGLOBAL res = ::LoadResource(hModule, hRes);
		char *code = (char *)::LockResource(res);
		// �������� ������ � ������ �����������
		int width = m_BmpNoize->GetWidth();
		int height = m_BmpNoize->GetHeight();

		// ��������� � ����������� kernel
		if(m_OpenCL->LoadKernel("Filter", code) != 0)
		{ /* Error */ }

		// ������� �����������
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

		// ��������� krenel
		m_OpenCL->RunFilterKernel(in, out, width, height, m_nEdge);

		m_BmpNoize->UnlockBits(bitmapDataNoize);

		// �������� ����� ������� � �����������
		m_BmpOut = reinterpret_cast<Gdiplus::Bitmap*>(Gdiplus::Bitmap::Bitmap(width, height, stride, PixelFormat32bppARGB, (BYTE*)out).Clone(Gdiplus::Rect(0, 0, width, height), PixelFormat32bppARGB));
		// ���������� ����������� �� ������
		mImageFiltered.SetImage(m_BmpOut);
	} else {
		/* Not Init */
	}
}


void COpenCLImageFilterDlg::OnBnClickedBrowsebutton()
{
	// TODO: �������� ���� ��� ����������� �����������
	CFileDialog fileDlg(true);
	fileDlg.DoModal(); // ��������� ������ ������ �����������
	UpdateData(true);
	m_sFindDir = fileDlg.GetFolderPath() + "\\" + fileDlg.GetFileName(); // ���� �� �����������
	UpdateData(false);

	if(!m_sFindDir.IsEmpty())
	{
		mImage.Clear();

		Gdiplus::Image jpg(m_sFindDir);
		m_BmpIn = reinterpret_cast<Gdiplus::Bitmap*>(jpg.Clone()); // �������� �����������
		mImage.SetImage(m_BmpIn); // ���������� ����������� �� ������
	}
}

void COpenCLImageFilterDlg::InitOpenCL(PVOID* param) // ����� ������������� OpenCL
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	if(dlg->m_OpenCL->CreateContext() != 0)
	{
		dlg->MessageBox(L"CreateContext::Error");
		return;
	}

	dlg->m_bIsOpenCLInit = true; // ���������� ���� ����������
}


void COpenCLImageFilterDlg::GetPlatformsThread(PVOID* param) // ����� ���������� ��������
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	dlg->platforms = dlg->m_OpenCL->GetPlatforms();

	dlg->m_bIsPlatformsReady = true; // ���������� ���� ����������
}


void COpenCLImageFilterDlg::GetDevicesThread(PVOID* param) // ����� ���������� ���������
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param;

	dlg->devices = dlg->m_OpenCL->GetDevices();

	dlg->m_bIsDevicesReady = true; // ���������� ���� ����������
}