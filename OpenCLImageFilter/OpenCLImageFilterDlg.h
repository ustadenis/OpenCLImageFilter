
// OpenCLImageFilterDlg.h : ���� ���������
//

#pragma once
#include "imagewnd.h"
#include "afxwin.h"
#include "afxcmn.h"


// ���������� ���� COpenCLImageFilterDlg
class COpenCLImageFilterDlg : public CDialogEx
{
	// ������
	class Timer
	{
		LARGE_INTEGER _freq;
		LARGE_INTEGER _last;

	public:

		Timer()
		{
			::QueryPerformanceFrequency(&_freq);
			Reset();
		}

		void Reset()
		{
			::QueryPerformanceCounter(&_last);
		}

		double Now()
		{
			LARGE_INTEGER now;
			::QueryPerformanceCounter(&now);
			return 1000.0 *(now.QuadPart - _last.QuadPart) / _freq.QuadPart;
		}
	};
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
	* ����� ��� ���������� ���������� �������� ����������
	*/
	static void StartFilter(PVOID* param);

	/**
	* Box ������
	*/
	void BoxFilter(unsigned int* in, unsigned int* out, int width, int height, int edge);

	/**
	* ��������� ������
	*/
	void MedianFilter(unsigned int* in, unsigned int* out, int width, int height, int edge);

	/**
	* Gaussian ������
	*/
	void GaussianFilter(unsigned int* in, unsigned int* out, int width, int height, int edge);

	void sort(unsigned char* tmp, int n);

	double sum(unsigned char* tmp, int size);

	double gaussian(double x, double mu, double sigma);

	double** gaussianKernel(int deep, double s);

	/**
	 * Gaussian noise
	 */
	void AddGaussianNoise(unsigned int* image, int width, int height);

	/**
	* Salt and Pepper noise
	*/
	void AddSaltAndPepperNoise(unsigned int* image, int width, int height);

	/**
	* Impulse noise
	*/
	void AddImpulseNoise(unsigned int* image, int width, int height);

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

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedAddnoisebutton();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedBrowsebutton();

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
	afx_msg void OnBnClickedAddnoisebutton2();
	afx_msg void OnBnClickedAddnoisebutton3();
	CButton m_SaltAndPepperNoise;
	CButton m_ImpulseNoise;
	afx_msg void OnClickedStartbutton2();
	CButton m_StartBoxFilter;
	afx_msg void OnBnClickedStartbutton3();
	double m_Sigma;
	CButton m_StartGaussianFilter;
	afx_msg void OnBnClickedStartbutton4();
};
