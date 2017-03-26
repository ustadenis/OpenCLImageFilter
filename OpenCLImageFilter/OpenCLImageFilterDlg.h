
// OpenCLImageFilterDlg.h : файл заголовка
//

#pragma once
#include "imagewnd.h"
#include "afxwin.h"
#include "afxcmn.h"


// диалоговое окно COpenCLImageFilterDlg
class COpenCLImageFilterDlg : public CDialogEx
{
	// Таймер
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

	/**
	* Поток для выполнения фильтрации линейным алгоритмом
	*/
	static void StartFilter(PVOID* param);

	/**
	* Box фильтр
	*/
	void BoxFilter(unsigned int* in, unsigned int* out, int width, int height, int edge);

	/**
	* Медианный фильтр
	*/
	void MedianFilter(unsigned int* in, unsigned int* out, int width, int height, int edge);

	/**
	* Gaussian фильтр
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

	bool m_bIsOpenCLInit; // Флаг готовности OpenCL
	bool m_bIsPlatformsReady; // Флаг готовности платформ
	bool m_bIsDevicesReady; // Флаг готовности устройств
	bool m_bIsImageFiltered; // Флаг готовности фильтрации
	bool m_bIsImageFilteredLA;

	double mTimeStart;
	Timer mTimer;

	Gdiplus::Bitmap *m_BmpIn; // Входное изображение
	Gdiplus::Bitmap *m_BmpNoize; // Зашумленное изображение
	Gdiplus::Bitmap *m_BmpOut; // Изображение после фильтра

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedAddnoisebutton();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedBrowsebutton();

	int m_nEdge; // Глубина фильтрации
	int m_nNoizeLevel; // Уровень шума
	double m_nTime;

	CString m_sOpenCLStatus; // Индикатор готовности OpenCL
	CString m_sFindDir; // Путь к файлу изображения
	CString m_sImageSize;

	CImageWnd mImage; // Отрисовка исходного изображения
	CImageWnd mImageNoize; // Отрисовка зашумленного изображения
	CImageWnd mImageFiltered; // Отрисовка отфильтрованного изображения

	CButton m_GetPLatformsButton; // Кнопка получения платформ
	CButton m_StartButton; // Кнопка начала фильтрации 
	CButton m_NoizeButton; // Кнопка начала зашумления
	CButton m_UseAllDevicesButton;
	CButton m_LinearAlgorithm;

	CComboBox m_PlatformsListBox; // Комбо платформ
	CComboBox m_GetDevicesListBox; // Комбо устройств

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
