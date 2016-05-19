#include "stdafx.h"
#include "COpenCL.h"
#include <time.h>

using namespace std;
using namespace cl;

COpenCL::COpenCL() // Конструктор
{
	m_nSelectedPlatform = 0;
	m_nSelectedDevice = 0;
	m_bUseAllDevices = false;
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

cl_int COpenCL::CreateContext(bool useAllDevices)
{
	m_bUseAllDevices = useAllDevices;
	if(devices.size() > 0)
	{
		if(!m_bUseAllDevices)
		{
			Device &dev = devices[m_nSelectedDevice];
			ctx = Context(dev); // Создание контекста для выбранного устройства
			queue = CommandQueue(ctx, dev); // Создание очереди
		}
		else
		{
			if(devices.size() > 1)
			{
				ctx = Context(devices);
				Device &dev1 = devices[0];
				queue = CommandQueue(ctx, dev1); // Создание первой очереди
				Device &dev2 = devices[1];
				second_queue = CommandQueue(ctx, dev2); // Создание второй очереди
			}
			else
			{
				Device &dev = devices[m_nSelectedDevice];
				ctx = Context(dev); // Создание контекста для выбранного устройства
				queue = CommandQueue(ctx, dev); // Создание очереди
			}
		}
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

cl_int COpenCL::RunFilterKernel(UINT* in, UINT* out, int width, int height, int edge)
{
	try
	{
		if(!m_bUseAllDevices || devices.size() == 1)
		{
			int imagesize = width * height; // Кол-во пикселей
			std::size_t datasize = imagesize * sizeof(UINT); // Размер бефера с изображением

			Buffer bIn(ctx, CL_MEM_READ_ONLY, datasize); // Создаем буфер для изображения
			Buffer bOut(ctx, CL_MEM_WRITE_ONLY, datasize); // Создаем буфер для отфильтрованного изображения

			queue.enqueueWriteBuffer(bIn, CL_TRUE, 0, datasize, in); // Записываем изображение в буфер

			// Записываем буфферы в ядро
			int arg = 0;
			kernel.setArg(arg++, bIn);
			kernel.setArg(arg++, bOut);
			kernel.setArg(arg++, edge);

			// Добавляем ядро в очередь и ждем конца выполнения
			queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height), NullRange);
			queue.finish();

			// Вычитываем получившееся изображение
			queue.enqueueReadBuffer(bOut, CL_TRUE, 0, datasize, out);
		}
		else
		{
			int imagesize = width * height / 2; // Кол-во пикселей
			std::size_t datasize = imagesize * sizeof(UINT); // Размер бефера с изображением

			Buffer bIn1(ctx, CL_MEM_READ_ONLY, datasize); // Создаем буфер для изображения
			Buffer bIn2(ctx, CL_MEM_READ_ONLY, datasize); // Создаем буфер для изображения
		
			Buffer bOut1(ctx, CL_MEM_WRITE_ONLY, datasize); // Создаем буфер для отфильтрованного изображения
			Buffer bOut2(ctx, CL_MEM_WRITE_ONLY, datasize); // Создаем буфер для отфильтрованного изображения

			queue.enqueueWriteBuffer(bIn1, CL_TRUE, 0, datasize, in); // Записываем изображение в буфер
			second_queue.enqueueWriteBuffer(bIn2, CL_TRUE, 0, datasize, in + imagesize); // Записываем изображение в буфер

			// Записываем буфферы в ядро
			int arg = 0;
			kernel.setArg(arg++, bIn1);
			kernel.setArg(arg++, bOut1);
			kernel.setArg(arg++, edge);

			// Добавляем ядро в очередь и ждем конца выполнения
			queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height / 2), NullRange);

			// Записываем буфферы в ядро
			arg = 0;
			kernel.setArg(arg++, bIn2);
			kernel.setArg(arg++, bOut2);
			kernel.setArg(arg++, edge);

			// Добавляем ядро в очередь и ждем конца выполнения
			second_queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height / 2), NullRange);

			queue.finish();
			second_queue.finish();

			// Вычитываем получившееся изображение
			queue.enqueueReadBuffer(bOut1, CL_TRUE, 0, datasize, out);

			// Вычитываем получившееся изображение
			second_queue.enqueueReadBuffer(bOut2, CL_TRUE, 0, datasize, out + imagesize);
		}
	} 
	catch(Error &e)
	{
		// Выводим сообщение об ошибке если что-то пошло не так
		MessageBoxA(NULL, e.what(), "Ошибка исполнения ядра", MB_ICONERROR);
		return e.err();
	}

	return CL_SUCCESS;
}