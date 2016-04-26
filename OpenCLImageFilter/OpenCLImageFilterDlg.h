
// OpenCLImageFilterDlg.h : ���� ���������
//

#pragma once
#include "COpenCL.h"
#include "imagewnd.h"
#include "afxwin.h"
#include "afxcmn.h"


// ���������� ���� COpenCLImageFilterDlg
class COpenCLImageFilterDlg : public CDialogEx
{
// ��������
public:
	COpenCLImageFilterDlg(CWnd* pParent = NULL);	// ����������� �����������

// ������ ����������� ����
	enum { IDD = IDD_OPENCLIMAGEFILTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// ��������� DDX/DDV


// ����������
protected:
	HICON m_hIcon;

	// ��������� ������� ����� ���������
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	/**
	 * ����� ������������� OpenCL
	 */
	static void InitOpenCL(PVOID* param);
	/**
	 * ����� ��� ��������� ��������
	 */
	static void GetPlatformsThread(PVOID* param);
	/**
	 * ����� ��� ��������� ���������
	 */
	static void GetDevicesThread(PVOID* param);

private:

	bool m_bIsOpenCLInit; // ���� ���������� OpenCL
	bool m_bIsPlatformsReady; // ���� ���������� ��������
	bool m_bIsDevicesReady; // ���� ���������� ���������

	Gdiplus::Bitmap *m_BmpIn; // ������� �����������
	Gdiplus::Bitmap *m_BmpNoize; // ����������� �����������
	Gdiplus::Bitmap *m_BmpOut; // ����������� ����� �������

	VECTOR_CLASS<Platform> platforms; // ������ ��������
	VECTOR_CLASS<Device> devices; // ������ ���������

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedGetplatformsbutton();
	afx_msg void OnCbnSelchangePlatformscombo();
	afx_msg void OnBnClickedGetdevicesbutton();
	afx_msg void OnCbnSelchangeDevicescombo();
	afx_msg void OnBnClickedInitbutton();
	afx_msg void OnBnClickedAddnoisebutton();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedBrowsebutton();

	COpenCL* m_OpenCL; // ������ ��� ������ � OpenCL

	int m_nEdge; // ������� ����������
	int m_nNoizeLevel; // ������� ����

	CString m_sOpenCLStatus; // ��������� ���������� OpenCL
	CString m_sFindDir; // ���� � ����� �����������

	CImageWnd mImage; // ��������� ��������� �����������
	CImageWnd mImageNoize; // ��������� ������������ �����������
	CImageWnd mImageFiltered; // ��������� ���������������� �����������

	CButton m_GetPLatformsButton; // ������ ��������� ��������
	CButton m_GetDevicesButton; // ������ ��������� ��������� � ������������� OpenCL
	CButton m_StartButton; // ������ ������ ���������� 
	CButton m_NoizeButton; // ������ ������ ����������

	CComboBox m_PlatformsListBox; // ����� ��������
	CComboBox m_GetDevicesListBox; // ����� ���������
};
