
// OpenCLImageFilterDlg.h : файл заголовка
//

#pragma once
#include "COpenCL.h"
#include "imagewnd.h"
#include "afxwin.h"
#include "afxcmn.h"


// диалоговое окно COpenCLImageFilterDlg
class COpenCLImageFilterDlg : public CDialogEx
{
// Создание
public:
	COpenCLImageFilterDlg(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
	enum { IDD = IDD_OPENCLIMAGEFILTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	static void InitOpenCL(PVOID* param);
	static void GetPlatformsThread(PVOID* param);
	static void GetDevicesThread(PVOID* param);

public:
	CImageWnd mImage;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

private:
	CString m_sFindDir;
	CButton m_InitButton;

	bool m_bIsOpenCLInit;
	bool m_bIsPlatformsReady;
	bool m_bIsDevicesReady;

	Gdiplus::Bitmap *m_BmpIn;
	Gdiplus::Bitmap *m_BmpNoize;
	Gdiplus::Bitmap *m_BmpOut;

	VECTOR_CLASS<Platform> platforms;
	VECTOR_CLASS<Device> devices;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_sOpenCLStatus;
	CImageWnd mImageFiltered;
	afx_msg void OnBnClickedButton3();

	COpenCL* m_OpenCL;
	afx_msg void OnBnClickedButton4();
	CButton m_GetPLatformsButton;
	CButton m_GetDevicesButton;
	afx_msg void OnBnClickedButton5();
	CButton m_StartButton;
	int m_nEdge;
	CComboBox m_PlatformsListBox;
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox m_GetDevicesListBox;
	afx_msg void OnCbnSelchangeCombo2();
	CImageWnd mImageNoize;
	CButton m_NoizeButton;
	afx_msg void OnBnClickedButton6();
	int m_nNoizeLevel;
};
