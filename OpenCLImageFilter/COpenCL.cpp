#include "stdafx.h"
#include "COpenCL.h"
#include <time.h>

using namespace std;
using namespace cl;

COpenCL::COpenCL() // �����������
{
	m_nSelectedPlatform = 0;
	m_nSelectedDevice = 0;
}

COpenCL::~COpenCL() // ����������
{

}

VECTOR_CLASS<Platform> COpenCL::GetPlatforms()
{
	cl_int ret = 0;

	ret = Platform::get(&platforms); // �������� ���������

	return platforms;
}

void COpenCL::SetSelectedPlatform(int num)
{
	m_nSelectedPlatform = num; // ���������� ������ ��������� ���������
}

void COpenCL::SetSelectedDevice(int num)
{
	m_nSelectedDevice = num; // ���������� ������ ���������� ����������
}

VECTOR_CLASS<Device> COpenCL::GetDevices()
{
	cl_int ret = 0;

	if(platforms.size() > 0)
		ret = platforms[m_nSelectedPlatform].getDevices(CL_DEVICE_TYPE_ALL, &devices); // �������� ����������

	return devices;
}

cl_int COpenCL::CreateContext()
{
	if(devices.size() > 0)
	{
		Device &dev = devices[m_nSelectedDevice];
		ctx = Context(dev); // �������� ��������� ��� ���������� ����������
		queue = CommandQueue(ctx, dev); // �������� �������
		return 0;
	}

	return -1;
}

cl_int COpenCL::LoadKernel(char* name, char* code)
{
	try
	{
		/* ������� �������� �� ���� ��������� */
		program = Program(ctx, code); // �������� ���������
		program.build(); // ���������� ���������

		kernel = Kernel(program, name); // �������� ����

		return CL_SUCCESS;
	}
	catch (Error &e)
	{
		// �������� ������ ���������
		STRING_CLASS log;
		
		switch (e.err())
		{
		case CL_BUILD_PROGRAM_FAILURE:
		
			// ��������� ��������� ������
			program.getBuildInfo(devices[m_nSelectedDevice], CL_PROGRAM_BUILD_LOG, &log);

			log = "\n\n" + log;
			log = e.what() + log;

			MessageBoxA(NULL, log.c_str(), "������ �������� ����", MB_ICONERROR);
			break;
		
		default:
			MessageBoxA(NULL, e.what(), "������ �������� ����", MB_ICONERROR);
			break;
		}

		return e.err();
	}
}

cl_int COpenCL::RunAddNoizeKernel(UINT* in, UINT* out, int noizeLevel, int width, int height)
{
	try
	{
		int pixelcount = width * height; // ���������� ��������
		std::size_t datasize = pixelcount * sizeof(UINT); // ������ �������
		Buffer bIn(ctx, CL_MEM_READ_WRITE, datasize); // �������� ������ ��� �����������
		Buffer bNoise(ctx, CL_MEM_READ_ONLY, datasize); // �������� ������ ��� ����� ����

		UINT* noize = new UINT[pixelcount]; // ������� � �������� ����� ����
		for(int i = 0; i < pixelcount; i++)
		{
			noize[i] = 0;
		}

		// ��������� ����� ����
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

		queue.enqueueWriteBuffer(bIn, CL_TRUE, 0, datasize, in); // ���������� ����������� � ������
		queue.enqueueWriteBuffer(bNoise, CL_TRUE, 0, datasize, noize); // ���������� ����� ���� � ������

		// ���������� ������� � ����
		int arg = 0;
		kernel.setArg(arg++, bIn);
		kernel.setArg(arg++, bNoise);

		// ��������� ���� � ������� � ���� ����� ����������
		queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height), NullRange);
		queue.finish();
		// ������� ��������� �� �������� ����������
		//MessageBox(NULL, L"Done", L"Success", MB_OK); 

		// ���������� ������������ �����������
		queue.enqueueReadBuffer(bIn, CL_TRUE, 0, datasize, out);
	} 
	catch(exception e)
	{
		// ������� ��������� �� ������ ���� ���-�� ����� �� ���
		MessageBox(NULL, (wchar_t*)e.what(), L"ERROR", MB_OK); 
	}

	return 0;
}

cl_int COpenCL::RunFilterKernel(UINT* in, UINT* out, int width, int height, int edge)
{
	try
	{
		int imagesize = width * height; // ���-�� ��������
		std::size_t datasize = imagesize * sizeof(UINT); // ������ ������� � ������������

		Buffer bIn(ctx, CL_MEM_READ_ONLY, datasize); // ������� ������ ��� �����������
		Buffer bOut(ctx, CL_MEM_WRITE_ONLY, datasize); // ������� ������ ��� ���������������� �����������

		queue.enqueueWriteBuffer(bIn, CL_TRUE, 0, datasize, in); // ���������� ����������� � ������

		// ���������� ������� � ����
		int arg = 0;
		kernel.setArg(arg++, bIn);
		kernel.setArg(arg++, bOut);
		kernel.setArg(arg++, edge);

		// ��������� ���� � ������� � ���� ����� ����������
		queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height), NullRange);
		queue.finish();
		// ������� ��������� �� �������� ����������
		//MessageBox(NULL, L"Done", L"Success", MB_OK); 

		// ���������� ������������ �����������
		queue.enqueueReadBuffer(bOut, CL_TRUE, 0, datasize, out);
	} 
	catch(exception e)
	{
		// ������� ��������� �� ������ ���� ���-�� ����� �� ���
		MessageBox(NULL, (wchar_t*)e.what(), L"ERROR", MB_OK); 
	}

	return 0;
}