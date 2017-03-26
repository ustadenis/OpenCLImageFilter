
// OpenCLImageFilterDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "OpenCLImageFilter.h"
#include "OpenCLImageFilterDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define RED(x) ((x & 0x00FF0000) >> 16)
#define GREEN(x) ((x & 0x0000FF00) >> 8)
#define BLUE(x) ((x & 0x000000FF) >> 0)

#define OUTRED(x) (x << 16)
#define OUTGREEN(x) (x << 8)
#define OUTBLUE(x) (x << 0)

#define TIMER_ID_IMAGE_FILTER_LA 0

#define PI 3.1415

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

////////////////////////////////////////////////////////////////////////////

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

////////////////////////////////////////////////////////////////////////////

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////

// диалоговое окно COpenCLImageFilterDlg
COpenCLImageFilterDlg::COpenCLImageFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COpenCLImageFilterDlg::IDD, pParent)
	, m_sFindDir(_T(""))
	, m_nEdge(3)
	, m_nNoizeLevel(15)
	, m_BmpIn(nullptr)
	, m_BmpNoize(nullptr)
	, m_BmpOut(nullptr)
	, m_nTime(0)
	, m_sImageSize(_T(""))
	, m_bLinearAlgorithm(TRUE)
	, m_Sigma(0.84)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sFindDir);
	DDX_Text(pDX, IDC_EDIT2, m_nEdge);
	DDX_Text(pDX, IDC_EDIT3, m_nNoizeLevel);
	DDX_Control(pDX, IDC_IMAGE, mImage);
	DDX_Control(pDX, IDC_IMAGEFILTERED, mImageFiltered);
	DDX_Control(pDX, IDC_IMAGEFILTERED2, mImageNoize);
	DDX_Control(pDX, IDC_STARTBUTTON, m_StartButton);
	DDX_Control(pDX, IDC_ADDNOISEBUTTON, m_NoizeButton);
	DDX_Text(pDX, IDC_TIME, m_nTime);
	DDX_Text(pDX, IDC_IMAGESIZE, m_sImageSize);
	DDX_Control(pDX, IDC_USEALLDEVICESCHECK2, m_LinearAlgorithm);
	DDX_Check(pDX, IDC_USEALLDEVICESCHECK2, m_bLinearAlgorithm);
	DDX_Control(pDX, IDC_ADDNOISEBUTTON2, m_SaltAndPepperNoise);
	DDX_Control(pDX, IDC_ADDNOISEBUTTON3, m_ImpulseNoise);
	DDX_Control(pDX, IDC_STARTBUTTON2, m_StartBoxFilter);
	DDX_Text(pDX, IDC_EDIT4, m_Sigma);
	DDX_Control(pDX, IDC_STARTBUTTON3, m_StartGaussianFilter);
}

////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(COpenCLImageFilterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ADDNOISEBUTTON, &COpenCLImageFilterDlg::OnBnClickedAddnoisebutton)
	ON_BN_CLICKED(IDC_STARTBUTTON, &COpenCLImageFilterDlg::OnBnClickedStartbutton)
	ON_BN_CLICKED(IDC_BROWSEBUTTON, &COpenCLImageFilterDlg::OnBnClickedBrowsebutton)
	ON_BN_CLICKED(IDC_ADDNOISEBUTTON2, &COpenCLImageFilterDlg::OnBnClickedAddnoisebutton2)
	ON_BN_CLICKED(IDC_ADDNOISEBUTTON3, &COpenCLImageFilterDlg::OnBnClickedAddnoisebutton3)
	ON_BN_CLICKED(IDC_STARTBUTTON2, &COpenCLImageFilterDlg::OnClickedStartbutton2)
	ON_BN_CLICKED(IDC_STARTBUTTON3, &COpenCLImageFilterDlg::OnBnClickedStartbutton3)
	ON_BN_CLICKED(IDC_STARTBUTTON4, &COpenCLImageFilterDlg::OnBnClickedStartbutton4)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
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
	m_bIsImageFilteredLA = false;

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR COpenCLImageFilterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	switch (nIDEvent)
	{
		case TIMER_ID_IMAGE_FILTER_LA:
		{
			if(m_bIsImageFilteredLA)
			{
				mImageFiltered.SetImage(m_BmpOut);
				m_bIsImageFilteredLA = false;
				m_StartButton.EnableWindow(TRUE);
				m_StartBoxFilter.EnableWindow(TRUE);
				m_StartGaussianFilter.EnableWindow(TRUE);
				m_nTime = floor((mTimer.Now() - mTimeStart) * 1000) / 1000.;
				UpdateData(FALSE);
				KillTimer(TIMER_ID_IMAGE_FILTER_LA);
			}
			break;
		}
		default:
			break;
	}
	
	CDialogEx::OnTimer(nIDEvent);
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::AddGaussianNoise(unsigned int* image, int width, int height)
{
	int pixelcount = width * height; // Количество пикселей
	UpdateData(true);

	// Заполняем маску шума
	srand((UINT)time(0));
	for (int i = 0; i < m_nNoizeLevel * pixelcount / 100; i++)
	{
		int x = rand() % width;
		int y = rand() % height;
		image[y * width + x] = RGB(rand() % 255, rand() % 255, rand() % 255);
	}
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::AddSaltAndPepperNoise(unsigned int* image, int width, int height) 
{
	int pixelcount = width * height; // Количество пикселей
	UpdateData(true);

	// Заполняем маску шума
	srand((UINT)time(0));
	for (int i = 0; i < m_nNoizeLevel * pixelcount / 100; i++)
	{
		int x = rand() % width;
		int y = rand() % height;

		int nNumber = rand() % 255;

		int color = 0;

		if (nNumber > 127.6) {
			color = 255;
		}

		image[y * width + x] = RGB(color, color, color);
	}
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::AddImpulseNoise(unsigned int* image, int width, int height)
{
	int pixelcount = width * height; // Количество пикселей
	UpdateData(true);

	// Заполняем маску шума
	srand((UINT)time(0));
	for (int i = 0; i < m_nNoizeLevel * pixelcount / 100; i++)
	{
		int x = rand() % width;
		int y = rand() % height;

		image[y * width + x] = RGB(255, 255, 255);
	}
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::OnBnClickedStartbutton()
{
	UpdateData(TRUE);
	if(!m_bLinearAlgorithm)
	{

	}
	else
	{
		PVOID* params = new PVOID[2];
		params[0] = this;
		params[1] = 0;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFilter, params, NULL, NULL);
		SetTimer(TIMER_ID_IMAGE_FILTER_LA, 100, NULL); // Запускаем таймер
		m_StartButton.EnableWindow(FALSE);
		m_StartBoxFilter.EnableWindow(FALSE);
		m_StartGaussianFilter.EnableWindow(FALSE);
	}
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::OnBnClickedBrowsebutton()
{
	// TODO: добавьте свой код обработчика уведомлений
	CFileDialog fileDlg(true);
	fileDlg.DoModal(); // Открываем диалог выбора изображения
	UpdateData(true);
	m_sFindDir = fileDlg.GetFolderPath() + "\\" + fileDlg.GetFileName(); // Путь до изображения

	if(!m_sFindDir.IsEmpty())
	{
		m_BmpIn = Gdiplus::Bitmap::FromFile(m_sFindDir); // Получаем изображение
		
		mImage.Clear();
		mImage.SetImage(m_BmpIn); // Показываем изображение на экране

		int nWidth = m_BmpIn->GetWidth();
		int nHeight = m_BmpIn->GetHeight();
		char chWidth[5];
		char chHeight[5];
		_itoa(nWidth, chWidth, 10);
		_itoa(nHeight, chHeight, 10);

		m_sImageSize = chWidth;
		m_sImageSize += "x";
		m_sImageSize += chHeight;
		m_sImageSize += " px";

		m_NoizeButton.EnableWindow();
		m_SaltAndPepperNoise.EnableWindow();
		m_ImpulseNoise.EnableWindow();
	}

	UpdateData(false);
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::StartFilter(PVOID* param)
{
	COpenCLImageFilterDlg* dlg = (COpenCLImageFilterDlg*)param[0];
	int type = (int)param[1];

	if (dlg->m_BmpOut != NULL)
	{
		// получаем высоту и ширину изображения
		int width = dlg->m_BmpNoize->GetWidth();
		int height = dlg->m_BmpNoize->GetHeight();

		Gdiplus::Bitmap *tmpBmp = dlg->m_BmpOut->Clone(Gdiplus::Rect(0, 0, width, height), PixelFormat32bppRGB);

		// Декодим изображение
		Gdiplus::BitmapData bitmapDataNoise;
		Gdiplus::BitmapData bitmapDataOut;

		//dlg->m_BmpNoize->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, &bitmapDataNoise);
		dlg->m_BmpOut->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeWrite, PixelFormat32bppRGB, &bitmapDataOut);
		tmpBmp->LockBits(&Gdiplus::Rect(0, 0, width, height), Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, &bitmapDataNoise);


		int stride = bitmapDataNoise.Stride;
		int n = width * height;

		UINT* in = (UINT*)bitmapDataNoise.Scan0;
		UINT* out = (UINT*)bitmapDataOut.Scan0;

		dlg->mTimeStart = dlg->mTimer.Now();
		int edge = dlg->m_nEdge;
		if (edge % 2 != 0)
			edge += 1;

		switch (type)
		{
		case 0:
		{
				  dlg->MedianFilter(in, out, width, height, edge);
				  break;
		}
		case 1:
		{
				  dlg->BoxFilter(in, out, width, height, edge);
				  break;
		}
		case 2:
		{
				  dlg->GaussianFilter(in, out, width, height, edge);
				  break;
		}
		default:
			break;
		}

		tmpBmp->UnlockBits(&bitmapDataNoise);
		dlg->m_BmpOut->UnlockBits(&bitmapDataOut);

		dlg->m_bIsImageFilteredLA = true;

		delete tmpBmp;
	}
	else if(dlg->m_BmpNoize != NULL)
	{
		// получаем высоту и ширину изображения
		int width = dlg->m_BmpNoize->GetWidth();
		int height = dlg->m_BmpNoize->GetHeight();

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
		
		switch (type)
		{
			case 0:
			{
				dlg->MedianFilter(in, out, width, height, edge);
				break;
			}
			case 1:
			{
				dlg->BoxFilter(in, out, width, height, edge);
				break;
			}
			case 2:
			{
				dlg->GaussianFilter(in, out, width, height, edge);
				break;
			}
			default:
				break;
		}

		dlg->m_BmpNoize->UnlockBits(&bitmapDataNoise);
		dlg->m_BmpOut->UnlockBits(&bitmapDataOut);

		dlg->m_bIsImageFilteredLA = true;
	} else {
		/* Not Init */
	}
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::MedianFilter(unsigned int* in, unsigned int* out, int width, int height, int edge)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int tmpSize = edge * edge;
			unsigned char *colorTmp = new unsigned char[tmpSize]; // Массив для цветов
			unsigned int *tmp = new unsigned int[tmpSize]; // Создадим массив для фильтрующего окна
			unsigned int pixel = 0x000000;

			// Берем окно размером edge x edge
			for (int l = -edge / 2; l < edge / 2; l++)
			{
				int line = l;
				if (l + y >= height)
				{
					line = height - (l + y);
				}
				else if (y + l < 0)
				{
					line = -(y + l);
				}
				for (int r = -edge / 2; r < edge / 2; r++)
				{
					int raw = r;
					if (r + x >= width)
					{
						raw = width - (r + x);
					}
					else if (r + x < 0)
					{
						raw = -(r + x);
					}

					tmp[(l + edge / 2) * edge + (r + edge / 2)] = in[(width * (y + line)) + (x + raw)];
				}
			}

			// Красный
			for (int i = 0; i < tmpSize; i++)
			{
				colorTmp[i] = RED(tmp[i]);
			}

			sort(colorTmp, tmpSize);

			pixel = pixel + OUTRED(colorTmp[(edge * edge - 1) / 2]);

			// Зеленый
			for (int i = 0; i < tmpSize; i++)
			{
				colorTmp[i] = GREEN(tmp[i]);
			}

			sort(colorTmp, tmpSize);

			pixel = pixel + OUTGREEN(colorTmp[(edge * edge - 1) / 2]);

			// Синий
			for (int i = 0; i < tmpSize; i++)
			{
				colorTmp[i] = BLUE(tmp[i]);
			}

			sort(colorTmp, tmpSize);

			pixel = pixel + OUTBLUE(colorTmp[(edge * edge - 1) / 2]);

			// Записываем в пиксель медиану (центральный пиксель)
			out[width * y + x] = pixel;

			delete[] tmp;
			delete[] colorTmp;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::BoxFilter(unsigned int* in, unsigned int* out, int width, int height, int edge)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int tmpSize = edge * edge;
			unsigned char *colorTmp = new unsigned char[tmpSize]; // Массив для цветов
			unsigned int *tmp = new unsigned int[tmpSize]; // Создадим массив для фильтрующего окна
			unsigned int pixel = 0x000000;

			// Берем окно размером edge x edge
			for (int l = 0; l < edge; l++)
			{
				int line = l;
				if (l + y >= height)
				{
					line = height - (l + y);
				}
				else if (y + l < 0)
				{
					line = -(y + l);
				}
				for (int r = 0; r < edge; r++)
				{
					int raw = r;
					if (r + x >= width)
					{
						raw = width - (r + x);
					}
					else if (r + x < 0)
					{
						raw = -(r + x);
					}

					tmp[l * edge + r] = in[(width * (y + line)) + (x + raw)];
				}
			}

			// Красный
			for (int i = 0; i < tmpSize; i++)
			{
				colorTmp[i] = RED(tmp[i]);
			}

			double d = sum(colorTmp, edge);

			pixel = pixel + OUTRED((unsigned char)(d / tmpSize));

			// Зеленый
			for (int i = 0; i < tmpSize; i++)
			{
				colorTmp[i] = GREEN(tmp[i]);
			}

			d = sum(colorTmp, edge);

			pixel = pixel + OUTGREEN((unsigned char)(d / tmpSize));

			// Синий
			for (int i = 0; i < tmpSize; i++)
			{
				colorTmp[i] = BLUE(tmp[i]);
			}

			d = sum(colorTmp, edge);

			pixel = pixel + OUTBLUE((unsigned char)(d / tmpSize));

			// Записываем в пиксель медиану (центральный пиксель)
			out[width * y + x] = pixel;

			delete[] tmp;
			delete[] colorTmp;
		}
	}
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::GaussianFilter(unsigned int* in, unsigned int* out, int width, int height, int edge)
{
	int size = 2 * edge + 1;
	double **kernel = gaussianKernel(edge, m_Sigma);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			unsigned int pixel = 0x000000;
			double sum = 0;

			for (int krnRow = 0; krnRow < size; krnRow++)
			{
				for (int krnCol = 0; krnCol < size; krnCol++)
				{
					if (((y + krnRow - 1) >= 0) && ((y + krnRow - 1) < width) && ((x + krnCol - 1) >= 0) && ((x + krnCol - 1) < height))
					{
						sum += kernel[krnRow][krnCol];
						unsigned int p = in[(y + krnRow - 1) * width + x + krnCol - 1];
						pixel += OUTRED((unsigned char)(RED(p) * kernel[krnRow][krnCol]));
						pixel += OUTGREEN((unsigned char)(GREEN(p) * kernel[krnRow][krnCol]));
						pixel += OUTBLUE((unsigned char)(BLUE(p) * kernel[krnRow][krnCol]));
					}
				}
			}

			unsigned int tmp = 0x000000;

			tmp += OUTRED((unsigned char)(RED(pixel)/sum));
			tmp += OUTGREEN((unsigned char)(GREEN(pixel) / sum));
			tmp += OUTBLUE((unsigned char)(BLUE(pixel) / sum));

			out[y*width + x] = tmp;
		}
	}

	for (int i = 0; i < size; i++)
	{
		delete[] kernel[i];
	}
	delete[] kernel;
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::sort(unsigned char* tmp, int n)
{
	unsigned char p;
	for(int k = 0; k < n; k++)
	{            
		int minElIndex = k;
        for(int s = k; s < n; s++)
		{     
            if(tmp[minElIndex] > tmp[s])
			{
				minElIndex = s;
            }
        }
		p = tmp[k];
        tmp[k] = tmp[minElIndex];
        tmp[minElIndex] = p;
    }
}

////////////////////////////////////////////////////////////////////////////

double COpenCLImageFilterDlg::gaussian(double x, double mu, double sigma)
{
	return ((1 / (sigma*PI))*exp(-(((x - mu) / (sigma))*((x - mu) / (sigma))) / 2.0)); //normal distribution function
}


double** COpenCLImageFilterDlg::gaussianKernel(int deep, double s)
{
	int size = 2 * deep + 1;
	double **result = new double*[size];
	for (int i = 0; i < size; i++)
	{
		result[i] = new double[size];
	}
	if (s == 0)
	{
		s = deep / 2;
	}

	double sum = 0;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			result[i][j] = gaussian(i, deep, s)*gaussian(j, deep, s);
			sum += result[i][j];
		}
	}

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			result[i][j] /= sum;
		}
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////

double COpenCLImageFilterDlg::sum(unsigned char* tmp, int size)
{
	double res = 0;
	for (int i = 0; i < size * size; i++)
	{
		res += tmp[i];
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::OnBnClickedAddnoisebutton()
{
	// TODO: добавьте свой код обработчика уведомлений
	if (m_BmpIn != NULL)
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
		AddGaussianNoise(out, width, height);

		// сохранение изменений
		m_BmpIn->UnlockBits(&bitmapDataIn);
		m_BmpNoize->UnlockBits(&bitmapDataOut);

		// Показываем изображение на экране
		mImageNoize.SetImage(m_BmpNoize);

		// Включаем кнопку фильтрации
		m_StartButton.EnableWindow();
		m_StartBoxFilter.EnableWindow();
		m_StartGaussianFilter.EnableWindow();
	}
	else {
		/* Not Init */
	}
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::OnBnClickedAddnoisebutton2()
{
	if (m_BmpIn != NULL)
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
		AddSaltAndPepperNoise(out, width, height);

		// сохранение изменений
		m_BmpIn->UnlockBits(&bitmapDataIn);
		m_BmpNoize->UnlockBits(&bitmapDataOut);

		// Показываем изображение на экране
		mImageNoize.SetImage(m_BmpNoize);

		// Включаем кнопку фильтрации
		m_StartButton.EnableWindow();
		m_StartBoxFilter.EnableWindow();
		m_StartGaussianFilter.EnableWindow();
	}
	else {
		/* Not Init */
	}
}

////////////////////////////////////////////////////////////////////////////

void COpenCLImageFilterDlg::OnBnClickedAddnoisebutton3()
{
	if (m_BmpIn != NULL)
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
		AddImpulseNoise(out, width, height);

		// сохранение изменений
		m_BmpIn->UnlockBits(&bitmapDataIn);
		m_BmpNoize->UnlockBits(&bitmapDataOut);

		// Показываем изображение на экране
		mImageNoize.SetImage(m_BmpNoize);

		// Включаем кнопку фильтрации
		m_StartButton.EnableWindow();
		m_StartBoxFilter.EnableWindow();
		m_StartGaussianFilter.EnableWindow();
	}
	else {
		/* Not Init */
	}
}

////////////////////////////////////////////////////////////////////////////


void COpenCLImageFilterDlg::OnClickedStartbutton2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (!m_bLinearAlgorithm)
	{

	}
	else
	{
		PVOID* params = new PVOID[2];
		params[0] = this;
		params[1] = (PVOID)1;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFilter, params, NULL, NULL);
		SetTimer(TIMER_ID_IMAGE_FILTER_LA, 100, NULL); // Запускаем таймер
		m_StartButton.EnableWindow(FALSE);
		m_StartBoxFilter.EnableWindow(FALSE);
		m_StartGaussianFilter.EnableWindow(FALSE);
	}
}


void COpenCLImageFilterDlg::OnBnClickedStartbutton3()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (!m_bLinearAlgorithm)
	{

	}
	else
	{
		PVOID* params = new PVOID[2];
		params[0] = this;
		params[1] = (PVOID)2;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartFilter, params, NULL, NULL);
		SetTimer(TIMER_ID_IMAGE_FILTER_LA, 100, NULL); // Запускаем таймер
		m_StartButton.EnableWindow(FALSE);
		m_StartBoxFilter.EnableWindow(FALSE);
		m_StartGaussianFilter.EnableWindow(FALSE);
	}
}


void COpenCLImageFilterDlg::OnBnClickedStartbutton4()
{
	// TODO: Add your control notification handler code here
	if (m_BmpOut != NULL) {
		mImageFiltered.Clear();
		m_BmpOut = NULL;
	}
}
