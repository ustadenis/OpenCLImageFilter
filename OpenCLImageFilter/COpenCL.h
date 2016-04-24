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

// Таймер
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

// Динамический массив
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
	COpenCL();
	~COpenCL();
	VECTOR_CLASS<Platform> GetPlatforms();
	VECTOR_CLASS<Device> GetDevices();
	cl_int CreateContext();
	cl_int LoadKernel(char* name, char* code);
	cl_int RunFilterKernel(BYTE* m1, BYTE* m2, int width, int height, int stride, int edge);
	cl_int RunAddNoizeKernel(BYTE* m1, BYTE* m2, int noizeLevel, int width, int height, int stride);
	void SetSelectedPlatform(int num);
	void SetSelectedDevice(int num);

private:
	void GetDevicesInfo();

public:
	static const int MAX_SOURCE_SIZE = 1024;

private:
	VECTOR_CLASS<Platform> platforms;
	VECTOR_CLASS<Device> devices;
	Context* ctx;
	CommandQueue* queue;
	Program* program;
	Kernel* kernel;

	int m_nSelectedPlatform;
	int m_nSelectedDevice;
};