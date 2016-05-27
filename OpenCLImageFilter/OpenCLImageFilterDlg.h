
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

	/**
	 * Поток инициализации OpenCL
	 */
	static void InitOpenCL(PVOID* param);
	/**
	 * Поток для получения платформ
	 */
	static void GetPlatformsThread(PVOID* param);
	/**
	 * Поток для получения устройств
	 */
	static void GetDevicesThread(PVOID* param);
	/**
	 * Поток для выполнения фильтрации
	 */
	static void StartFilter(PVOID* param);

	/**
	 * Поток для выполнения фильтрации линейным алгоритмом
	 */
	static void StartFilterLA(PVOID* param);

	void LAFilter(unsigned int* in, unsigned int* out, int width, int height, int edge);

	void sort(unsigned char* tmp, int n);

	/**
	 * Зашумление ищображения
	 */
	void AddNoise(unsigned int* image, int width, int height);

	/**
	 * Получение устройств
	 */
	void GetDevices();

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

	VECTOR_CLASS<Platform> platforms; // Вектор платформ
	VECTOR_CLASS<Device> devices; // Вектор устройств

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedGetplatformsbutton();
	afx_msg void OnCbnSelchangePlatformscombo();
	afx_msg void OnCbnSelchangeDevicescombo();
	afx_msg void OnBnClickedAddnoisebutton();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedBrowsebutton();
	afx_msg void OnBnClickedUsealldevicescheck();

	COpenCL m_OpenCL; // Объект для работы с OpenCL

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
};
