
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
	/**
	 * ����� ��� ���������� ����������
	 */
	static void StartFilter(PVOID* param);

	/**
	 * ����� ��� ���������� ���������� �������� ����������
	 */
	static void StartFilterLA(PVOID* param);

	void LAFilter(unsigned int* in, unsigned int* out, int width, int height, int edge);

	void sort(unsigned char* tmp, int n);

	/**
	 * ���������� �����������
	 */
	void AddNoise(unsigned int* image, int width, int height);

	/**
	 * ��������� ���������
	 */
	void GetDevices();

private:

	bool m_bIsOpenCLInit; // ���� ���������� OpenCL
	bool m_bIsPlatformsReady; // ���� ���������� ��������
	bool m_bIsDevicesReady; // ���� ���������� ���������
	bool m_bIsImageFiltered; // ���� ���������� ����������
	bool m_bIsImageFilteredLA;

	double mTimeStart;
	Timer mTimer;

	Gdiplus::Bitmap *m_BmpIn; // ������� �����������
	Gdiplus::Bitmap *m_BmpNoize; // ����������� �����������
	Gdiplus::Bitmap *m_BmpOut; // ����������� ����� �������

	VECTOR_CLASS<Platform> platforms; // ������ ��������
	VECTOR_CLASS<Device> devices; // ������ ���������

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedGetplatformsbutton();
	afx_msg void OnCbnSelchangePlatformscombo();
	afx_msg void OnCbnSelchangeDevicescombo();
	afx_msg void OnBnClickedAddnoisebutton();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedBrowsebutton();
	afx_msg void OnBnClickedUsealldevicescheck();

	COpenCL m_OpenCL; // ������ ��� ������ � OpenCL

	int m_nEdge; // ������� ����������
	int m_nNoizeLevel; // ������� ����
	double m_nTime;

	CString m_sOpenCLStatus; // ��������� ���������� OpenCL
	CString m_sFindDir; // ���� � ����� �����������
	CString m_sImageSize;

	CImageWnd mImage; // ��������� ��������� �����������
	CImageWnd mImageNoize; // ��������� ������������ �����������
	CImageWnd mImageFiltered; // ��������� ���������������� �����������

	CButton m_GetPLatformsButton; // ������ ��������� ��������
	CButton m_StartButton; // ������ ������ ���������� 
	CButton m_NoizeButton; // ������ ������ ����������
	CButton m_UseAllDevicesButton;
	CButton m_LinearAlgorithm;

	CComboBox m_PlatformsListBox; // ����� ��������
	CComboBox m_GetDevicesListBox; // ����� ���������

	BOOL m_bUseAllDevices;
	BOOL m_bLinearAlgorithm;
};
