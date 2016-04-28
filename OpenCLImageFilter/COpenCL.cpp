#include "stdafx.h"
#include "COpenCL.h"
#include <time.h>

using namespace std;
using namespace cl;

COpenCL::COpenCL() // Конструктор
{
	m_nSelectedPlatform = 0;
	m_nSelectedDevice = 0;
}

COpenCL::~COpenCL() // Деструктор
{

}

VECTOR_CLASS<Platform> COpenCL::GetPlatforms()
{
	cl_int ret = 0;

	ret = Platform::get(&platforms); // Получить платформы

	return platforms;
}

void COpenCL::SetSelectedPlatform(int num)
{
	m_nSelectedPlatform = num; // Установить индекс выбранной платформы
}

void COpenCL::SetSelectedDevice(int num)
{
	m_nSelectedDevice = num; // Установить индекс выбранного устройства
}

VECTOR_CLASS<Device> COpenCL::GetDevices()
{
	cl_int ret = 0;

	if(platforms.size() > 0)
		ret = platforms[m_nSelectedPlatform].getDevices(CL_DEVICE_TYPE_ALL, &devices); // Получить устройства

	return devices;
}

cl_int COpenCL::CreateContext()
{
	if(devices.size() > 0)
	{
		Device &dev = devices[m_nSelectedDevice];
		ctx = Context(dev); // Создание контекста для выбранного устройства
		queue = CommandQueue(ctx, dev); // Создание очереди
		return 0;
	}

	return -1;
}

cl_int COpenCL::LoadKernel(char* name, char* code)
{
	try
	{
		/* создать бинарник из кода программы */
		program = Program(ctx, code); // Создание программы
		program.build(); // Компиляция программы

		kernel = Kernel(program, name); // Создание ядра

		return CL_SUCCESS;
	}
	catch (Error &e)
	{
		// протокол сборки программы
		STRING_CLASS log;
		
		switch (e.err())
		{
		case CL_BUILD_PROGRAM_FAILURE:
		
			// получение протокола сборки
			program.getBuildInfo(devices[m_nSelectedDevice], CL_PROGRAM_BUILD_LOG, &log);

			log = "\n\n" + log;
			log = e.what() + log;

			MessageBoxA(NULL, log.c_str(), "Ошибка создания ядра", MB_ICONERROR);
			break;
		
		default:
			MessageBoxA(NULL, e.what(), "Ошибка создания ядра", MB_ICONERROR);
			break;
		}

		return e.err();
	}
}

cl_int COpenCL::RunAddNoizeKernel(UINT* in, UINT* out, int noizeLevel, int width, int height)
{
	try
	{
		int pixelcount = width * height; // Количество пикселей
		std::size_t datasize = pixelcount * sizeof(UINT); // Размер буффера
		Buffer bIn(ctx, CL_MEM_READ_WRITE, datasize); // Выделяем буффер для изображения
		Buffer bNoise(ctx, CL_MEM_READ_ONLY, datasize); // Выделяем буффер для маски шума

		UINT* noize = new UINT[pixelcount]; // Создаем и обнуляем маску шума
		for(int i = 0; i < pixelcount; i++)
		{
			noize[i] = 0;
		}

		// Заполняем маску шума
		srand((UINT)time(0));
		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width * noizeLevel / 100; j++)
			{
				int index = rand() % width;
				
				for(int k = index - 2; k < index + 2; k++)
				{
					int offset = rand() % 16;
					if(k > 0 && k < width)
						noize[i * width + k] = 0xFF000000 | (0xFF << offset);
				}
			}
		}

		queue.enqueueWriteBuffer(bIn, CL_TRUE, 0, datasize, in); // Записываем изображение в буффер
		queue.enqueueWriteBuffer(bNoise, CL_TRUE, 0, datasize, noize); // Записываем маску шума в буффер

		// Записываем буфферы в ядро
		int arg = 0;
		kernel.setArg(arg++, bIn);
		kernel.setArg(arg++, bNoise);

		// Добавляем ядро в очередь и ждем конца выполнения
		queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height), NullRange);
		queue.finish();
		// Выводим сообщение об успешном выполнение
		//MessageBox(NULL, L"Done", L"Success", MB_OK); 

		// Вычитываем получившееся изображение
		queue.enqueueReadBuffer(bIn, CL_TRUE, 0, datasize, out);
	} 
	catch(exception e)
	{
		// Выводим сообщение об ошибке если что-то пошло не так
		MessageBox(NULL, (wchar_t*)e.what(), L"ERROR", MB_OK); 
	}

	return 0;
}

cl_int COpenCL::RunFilterKernel(UINT* in, UINT* out, int width, int height, int edge)
{
	try
	{
		int imagesize = width * height; // Кол-во пикселей
		std::size_t datasize = imagesize * sizeof(UINT); // Размер беффера с изображением

		Buffer bIn(ctx, CL_MEM_READ_ONLY, datasize); // Создаем буффер для изображения
		Buffer bOut(ctx, CL_MEM_WRITE_ONLY, datasize); // Создаем буффер для отфильтрованного изображения

		queue.enqueueWriteBuffer(bIn, CL_TRUE, 0, datasize, in); // Записываем изображение в буффер

		// Записываем буфферы в ядро
		int arg = 0;
		kernel.setArg(arg++, bIn);
		kernel.setArg(arg++, bOut);
		kernel.setArg(arg++, edge);

		// Добавляем ядро в очередь и ждем конца выполнения
		queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height), NullRange);
		queue.finish();
		// Выводим сообщение об успешном выполнение
		//MessageBox(NULL, L"Done", L"Success", MB_OK); 

		// Вычитываем получившееся изображение
		queue.enqueueReadBuffer(bOut, CL_TRUE, 0, datasize, out);
	} 
	catch(exception e)
	{
		// Выводим сообщение об ошибке если что-то пошло не так
		MessageBox(NULL, (wchar_t*)e.what(), L"ERROR", MB_OK); 
	}

	return 0;
}