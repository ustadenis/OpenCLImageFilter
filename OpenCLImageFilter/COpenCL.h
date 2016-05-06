#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define __CL_ENABLE_EXCEPTIONS
#include <cassert>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL\cl.hpp>
#endif

using namespace std;
using namespace cl;

// ������
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

// ������������ ������
template<class T> class Array
{
	T *_array;
	std::size_t _rows;
	std::size_t _cols;

public:

	Array(std::size_t size) : _array(nullptr), _rows(1), _cols(size)
	{
		_array = new T[size];
	}

	Array(std::size_t rows, std::size_t cols) : _array(nullptr), _rows(rows), _cols(cols)
	{
		_array = new T[rows * cols];
	}

	~Array()
	{
		delete _array;
	}

	T& operator()(std::size_t index)
	{
		return _array[index];
	}

	T& operator()(std::size_t row, std::size_t col)
	{
		return _array[row * _cols + col];
	}

	operator T*()
	{
		return _array;
	}

	std::size_t Size()
	{
		return _rows * _cols * ValueSize();
	}

	std::size_t ValueSize()
	{
		return sizeof(T);
	}
};

class COpenCL
{
public:
	/**
	 * ����������� �� ���������
	 */
	COpenCL();

	/**
	 * ���������� �� ���������
	 */
	~COpenCL();
	
	/**
	 * �������� ���������
	 *
	 * @return ������ ��������
	 */
	VECTOR_CLASS<Platform> GetPlatforms();

	/**
	 * �������� ����������
	 *
	 * @return ������ ���������
	 */
	VECTOR_CLASS<Device> GetDevices();

	/**
	 * ������� �������� � �������
	 *
	 * @return ��� ������
	 */
	cl_int CreateContext(bool useAllDevices);

	/**
	 * ��������� ��� � �������������� ���������
	 *
	 * @param name - ��� �������
	 * @param code - ���
	 *
	 * @return ��� ������
	 */
	cl_int LoadKernel(char* name, char* code);

	/**
	 * ��������� ����������
	 *
	 * @param in - ����������������� �����������
	 * @param out - ��������������� �����������
	 * @param width - ������ �����������
	 * @param height - ������ �����������
	 * @param edge - ������� ����������
	 *
	 * @return ��� ������
	 */
	cl_int RunFilterKernel(UINT* in, UINT* out, int width, int height, int edge);

	/**
	 * ��������� ���������� ���� �� �����������
	 *
	 * @param in - ������� �����������
	 * @param out - ����������� �����������
	 * @param noiseLevel - ������� ��������� ����
	 * @param width - ������ �����������
	 * @param height - ������ �����������
	 *
	 * @return ��� ������
	 */
	cl_int RunAddNoizeKernel(UINT* in, UINT* out, int noiseLevel, int width, int height);

	/**
	 * ������� ���������
	 *
	 * @param num - ����� ���������
	 */
	void SetSelectedPlatform(int num);

	/**
	 * ������� ����������
	 *
	 * @param num - ����� ����������
	 */
	void SetSelectedDevice(int num);

public:
	static const int MAX_SOURCE_SIZE = 1024; // ������������ ������ �������� ����

private:
	VECTOR_CLASS<Platform> platforms; // ������ ��������
	VECTOR_CLASS<Device> devices; // ������ ���������
	
	Context ctx; // ��������
	CommandQueue queue; // �������
	Program program; // ���������
	Kernel kernel; // ����

	int m_nSelectedPlatform; // ������ ��������� ���������
	int m_nSelectedDevice; // ������ ���������� ����������
};