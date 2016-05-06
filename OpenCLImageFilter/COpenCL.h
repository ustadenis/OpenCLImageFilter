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
	/**
	 * Конструктор по умолчанию
	 */
	COpenCL();

	/**
	 * Деструктор по умолчанию
	 */
	~COpenCL();
	
	/**
	 * Получить платформы
	 *
	 * @return вектор платформ
	 */
	VECTOR_CLASS<Platform> GetPlatforms();

	/**
	 * Получить устройства
	 *
	 * @return вектор устрйоств
	 */
	VECTOR_CLASS<Device> GetDevices();

	/**
	 * Создать контекст и очередь
	 *
	 * @return код ошибки
	 */
	cl_int CreateContext(bool useAllDevices);

	/**
	 * Загрузить код и скомпилировать программу
	 *
	 * @param name - имя функции
	 * @param code - код
	 *
	 * @return код ошибки
	 */
	cl_int LoadKernel(char* name, char* code);

	/**
	 * Запустить фильтрацию
	 *
	 * @param in - неотфильтрованное изображение
	 * @param out - отфильтрованное изображение
	 * @param width - ширина изображение
	 * @param height - высота изображение
	 * @param edge - глубина фильтрации
	 *
	 * @return код ошибки
	 */
	cl_int RunFilterKernel(UINT* in, UINT* out, int width, int height, int edge);

	/**
	 * Запустить добавление шума на изображение
	 *
	 * @param in - входное изображение
	 * @param out - зашумленное изображение
	 * @param noiseLevel - уровень желаемого шума
	 * @param width - ширина изображение
	 * @param height - высота изображение
	 *
	 * @return код ошибки
	 */
	cl_int RunAddNoizeKernel(UINT* in, UINT* out, int noiseLevel, int width, int height);

	/**
	 * Выбрать платформу
	 *
	 * @param num - номер платформы
	 */
	void SetSelectedPlatform(int num);

	/**
	 * Выбрать устройство
	 *
	 * @param num - номер устройства
	 */
	void SetSelectedDevice(int num);

public:
	static const int MAX_SOURCE_SIZE = 1024; // Максимальный размер входного кода

private:
	VECTOR_CLASS<Platform> platforms; // Вектор платформ
	VECTOR_CLASS<Device> devices; // Вектор устройств
	
	Context ctx; // Контекст
	CommandQueue queue; // Очередь
	Program program; // Программа
	Kernel kernel; // Ядро

	int m_nSelectedPlatform; // Индекс выбранной платформы
	int m_nSelectedDevice; // Индекс выбранного устройства
};