#include "stdafx.h"
#include "COpenCL.h"
#include <time.h>

using namespace std;
using namespace cl;

COpenCL::COpenCL()
{
	m_nSelectedPlatform = 0;
	m_nSelectedDevice = 0;
}

COpenCL::~COpenCL()
{

}

VECTOR_CLASS<Platform> COpenCL::GetPlatforms()
{
	cl_int ret = 0;

	ret = Platform::get(&platforms);

	return platforms;
}

void COpenCL::SetSelectedPlatform(int num)
{
	m_nSelectedPlatform = num;
}

void COpenCL::SetSelectedDevice(int num)
{
	m_nSelectedDevice = num;
}

VECTOR_CLASS<Device> COpenCL::GetDevices()
{
	cl_int ret = 0;

	if(platforms.size() > 0)
		ret = platforms[m_nSelectedPlatform].getDevices(CL_DEVICE_TYPE_ALL, &devices);

	return devices;
}

cl_int COpenCL::CreateContext()
{
	if(devices.size() > 0)
	{
		Device &dev = devices[m_nSelectedDevice];
		ctx = new Context(dev);
		queue = new CommandQueue(*ctx, dev);
		return 0;
	}

	return -1;
}

cl_int COpenCL::LoadKernel(char* name, char* code)
{
	/* создать бинарник из кода программы */
	program = new Program(*ctx, code);
	program->build();

	kernel = new Kernel(*program, name);

	return 0;
}

cl_int COpenCL::RunFilterKernel(BYTE* in1, BYTE* in2, int width, int height, int edge)
{
	try
	{
		int n = width * height;
		std::size_t datasize = n * sizeof(BYTE);
		Buffer bIn1(*ctx, CL_MEM_READ_ONLY, datasize);
		Buffer bIn2(*ctx, CL_MEM_WRITE_ONLY, datasize);

		queue->enqueueWriteBuffer(bIn1, CL_TRUE, 0, datasize, in1);

		int arg = 0;
		kernel->setArg(arg++, bIn1);
		kernel->setArg(arg++, bIn2);
		kernel->setArg(arg++, edge);

		queue->enqueueNDRangeKernel(*kernel, NullRange, NDRange(width, height), NullRange);
		queue->finish();
		MessageBox(NULL, L"Done", L"Success", MB_OK); 

		queue->enqueueReadBuffer(bIn2, CL_TRUE, 0, n, in2);
	} 
	catch(exception e)
	{
		MessageBox(NULL, L"Something wrong", L"ERROR", MB_OK); 
	}

	return 0;
}

cl_int COpenCL::RunAddNoizeKernel(BYTE* in1, BYTE* in2, int noizeLevel, int width, int height)
{
	try
	{
		int n = width * height;
		std::size_t datasize = n * sizeof(BYTE);
		Buffer bIn1(*ctx, CL_MEM_READ_WRITE, datasize);
		Buffer bIn2(*ctx, CL_MEM_READ_ONLY, datasize);

		BYTE* noize = new BYTE[datasize];
		for(int i = 0; i < datasize; i++)
		{
			noize[i] = 0;
		}

		srand(time(0));
		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width * noizeLevel / 100; j++)
			{
				int index = rand() % width;
				
				for(int k = index - 2; k < index + 2; k++)
				{
					if(k > 0 && k < width)
						noize[i * width + k] = 255;
				}
			}
		}

		queue->enqueueWriteBuffer(bIn1, CL_TRUE, 0, datasize, in1);
		queue->enqueueWriteBuffer(bIn2, CL_TRUE, 0, datasize, noize);

		int arg = 0;
		kernel->setArg(arg++, bIn1);
		kernel->setArg(arg++, bIn2);
		kernel->setArg(arg++, noizeLevel);

		queue->enqueueNDRangeKernel(*kernel, NullRange, NDRange(width, height), NullRange);
		queue->finish();
		MessageBox(NULL, L"Done", L"Success", MB_OK); 

		queue->enqueueReadBuffer(bIn1, CL_TRUE, 0, n, in2);
	} 
	catch(exception e)
	{
		MessageBox(NULL, L"Something wrong", L"ERROR", MB_OK); 
	}

	return 0;
}