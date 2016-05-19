#include "stdafx.h"
#include "COpenCL.h"
#include <time.h>

using namespace std;
using namespace cl;

COpenCL::COpenCL() // �����������
{
	m_nSelectedPlatform = 0;
	m_nSelectedDevice = 0;
	m_bUseAllDevices = false;
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
	m_bUseAllDevices = useAllDevices;
	if(devices.size() > 0)
	{
		if(!m_bUseAllDevices)
		{
			Device &dev = devices[m_nSelectedDevice];
			ctx = Context(dev); // �������� ��������� ��� ���������� ����������
			queue = CommandQueue(ctx, dev); // �������� �������
		}
		else
		{
			if(devices.size() > 1)
			{
				ctx = Context(devices);
				Device &dev1 = devices[0];
				queue = CommandQueue(ctx, dev1); // �������� ������ �������
				Device &dev2 = devices[1];
				second_queue = CommandQueue(ctx, dev2); // �������� ������ �������
			}
			else
			{
				Device &dev = devices[m_nSelectedDevice];
				ctx = Context(dev); // �������� ��������� ��� ���������� ����������
				queue = CommandQueue(ctx, dev); // �������� �������
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
		if(!m_bUseAllDevices || devices.size() == 1)
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
		else
		{
			int imagesize = width * height / 2; // ���-�� ��������
			std::size_t datasize = imagesize * sizeof(UINT); // ������ ������ � ������������

			Buffer bIn1(ctx, CL_MEM_READ_ONLY, datasize); // ������� ����� ��� �����������
			Buffer bIn2(ctx, CL_MEM_READ_ONLY, datasize); // ������� ����� ��� �����������
		
			Buffer bOut1(ctx, CL_MEM_WRITE_ONLY, datasize); // ������� ����� ��� ���������������� �����������
			Buffer bOut2(ctx, CL_MEM_WRITE_ONLY, datasize); // ������� ����� ��� ���������������� �����������

			queue.enqueueWriteBuffer(bIn1, CL_TRUE, 0, datasize, in); // ���������� ����������� � �����
			second_queue.enqueueWriteBuffer(bIn2, CL_TRUE, 0, datasize, in + imagesize); // ���������� ����������� � �����

			// ���������� ������� � ����
			int arg = 0;
			kernel.setArg(arg++, bIn1);
			kernel.setArg(arg++, bOut1);
			kernel.setArg(arg++, edge);

			// ��������� ���� � ������� � ���� ����� ����������
			queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height / 2), NullRange);

			// ���������� ������� � ����
			arg = 0;
			kernel.setArg(arg++, bIn2);
			kernel.setArg(arg++, bOut2);
			kernel.setArg(arg++, edge);

			// ��������� ���� � ������� � ���� ����� ����������
			second_queue.enqueueNDRangeKernel(kernel, NullRange, NDRange(width, height / 2), NullRange);

			queue.finish();
			second_queue.finish();

			// ���������� ������������ �����������
			queue.enqueueReadBuffer(bOut1, CL_TRUE, 0, datasize, out);

			// ���������� ������������ �����������
			second_queue.enqueueReadBuffer(bOut2, CL_TRUE, 0, datasize, out + imagesize);
		}
	} 
	catch(Error &e)
	{
		// ������� ��������� �� ������ ���� ���-�� ����� �� ���
		MessageBoxA(NULL, e.what(), "������ ���������� ����", MB_ICONERROR);
		return e.err();
	}

	return CL_SUCCESS;
}