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

cl_int COpenCL::CreateContext(bool useAllDevices)
{
	if(devices.size() > 0)
	{
		Device &dev = devices[m_nSelectedDevice];
		if(!useAllDevices)
		{
			ctx = Context(dev); // �������� ��������� ��� ���������� ����������
		}
		else
		{
			ctx = Context(devices);
		}
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

cl_int COpenCL::RunFilterKernel(UINT* in, UINT* out, int width, int height, int edge)
{
	try
	{
		int imagesize = width * height; // ���-�� ��������
		std::size_t datasize = imagesize * sizeof(UINT); // ������ ������ � ������������

		Buffer bIn(ctx, CL_MEM_READ_ONLY, datasize); // ������� ����� ��� �����������
		Buffer bOut(ctx, CL_MEM_WRITE_ONLY, datasize); // ������� ����� ��� ���������������� �����������

		queue.enqueueWriteBuffer(bIn, CL_TRUE, 0, datasize, in); // ���������� ����������� � �����

		// ���������� ������� � ����
		int arg = 0;
		kernel.setArg(arg++, bIn);
		kernel.setArg(arg++, bOut);
		kernel.setArg(arg++, edge);

		// ��������� ���� � ������� � ���� ����� ����������
		queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height), NullRange);
		queue.finish();

		// ���������� ������������ �����������
		queue.enqueueReadBuffer(bOut, CL_TRUE, 0, datasize, out);
	} 
	catch(Error &e)
	{
		// ������� ��������� �� ������ ���� ���-�� ����� �� ���
		MessageBoxA(NULL, e.what(), "������ ���������� ����", MB_OK);
		return e.err();
	}

	return CL_SUCCESS;
}