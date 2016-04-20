
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

public:
	CImageWnd mImage;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

private:
	CString m_sFindDir;
	CButton m_InitButton;
	bool m_bIsOpenCLInit;
	Gdiplus::Bitmap *m_BmpIn;
	Gdiplus::Bitmap *m_BmpOut;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CString m_sOpenCLStatus;
	CImageWnd mImageFiltered;
	afx_msg void OnBnClickedButton3();

	COpenCL* m_OpenCL;
	afx_msg void OnBnClickedButton4();
//	CComboBoxEx m_PlatformsComboBox;
	CListBox m_PlatformsListBox;
	CButton m_GetPLatformsButton;
	CButton m_GetDevicesButton;
	CListBox m_GetDevicesListBox;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnLbnSelchangeList3();
	CButton m_StartButton;
	int m_nEdge;
};
