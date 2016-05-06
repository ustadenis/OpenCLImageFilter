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

cl_int COpenCL::CreateContext(bool useAllDevices)
{
	if(devices.size() > 0)
	{
		Device &dev = devices[m_nSelectedDevice];
		if(!useAllDevices)
		{
			ctx = Context(dev); // Создание контекста для выбранного устройства
		}
		else
		{
			ctx = Context(devices);
		}
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

cl_int COpenCL::RunFilterKernel(UINT* in, UINT* out, int width, int height, int edge)
{
	try
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
	catch(Error &e)
	{
		// Выводим сообщение об ошибке если что-то пошло не так
		MessageBoxA(NULL, e.what(), "Ошибка исполнения ядра", MB_OK);
		return e.err();
	}

	return CL_SUCCESS;
}