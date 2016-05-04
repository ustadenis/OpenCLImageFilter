#define RED(x) ((x & 0x00FF0000) >> 16)
#define GREEN(x) ((x & 0x0000FF00) >> 8)
#define BLUE(x) ((x & 0x000000FF) >> 0)

#define OUTRED(x) (x << 16)
#define OUTGREEN(x) (x << 8)
#define OUTBLUE(x) (x << 0)

/**
 *
 * Функция сортировки
 *
 * @param указатель на массив
 * @param размер массива
 */
void sort(unsigned char* tmp, int n)
{
	unsigned char p;
	for(int k = 0; k < n; k++)
	{            
        for(int s = n - 1; s > k; s--)
		{     
            if(tmp[s - 1] > tmp[s])
			{
                p = tmp[s - 1];
                tmp[s - 1] = tmp[s];
                tmp[s] = p;
            }
        }
    }
}

/**
 *
 * Очистка массива
 *
 * @param указатель на массив
 * @param размер массива
 */
void clear(unsigned char* tmp, int n)
{
	for(int i = 0; i < n; i++)
	{
		tmp[i] = 0;
	}
}

/**
 * Ядро фильтрации окном
 *
 * @param in - зашумленное изображение
 * @param out - отфильтрованное изображение
 * @param edge - глубина фильтрации
 */
__kernel void Filter(
	__global unsigned int *in,
	__global unsigned int *out,
	int edge)
{
	const int x = get_global_id(0); // Получаем индекс в 0 измерение
	const int y = get_global_id(1); // Получаем индекс в 1 измерение
	const int width = get_global_size(0); // Получаем размерность 0 измерения
	const int height = get_global_size(1); // Получаем размерность 1 измерения

	if ((x >= width) || (y >= height)) return; // Проверяем что индексы не вышли за диаппазон

	unsigned int tmp[1000]; // Создадим массив для фильтрующего окна
	unsigned char colorTmp[1000]; // Массив для цветов
	int tmpSize = edge * edge;
	unsigned int pixel = in[width * y + x] & 0xFF000000;

	// Берем окно размером edge x edge
	for(int l = 0; l < edge; l++)
	{
		for(int r = 0; r < edge; r++)
		{
			if(x + r < width && l + y < height)
				tmp[l * edge + r] = in[(width * (y + l)) + (x + r)];
		}
	}
	
	// Красный
	for(int i = 0; i < tmpSize; i++)
	{
		colorTmp[i] = RED(tmp[i]);
	}

    sort(&colorTmp, tmpSize);

	pixel = pixel + OUTRED(colorTmp[(edge * edge - 1) / 2]);

	clear(&colorTmp, tmpSize);

	// Зеленый
	for(int i = 0; i < tmpSize; i++)
	{
		colorTmp[i] = GREEN(tmp[i]);
	}

    sort(&colorTmp, tmpSize);

	pixel = pixel + OUTGREEN(colorTmp[(edge * edge - 1) / 2]);

	clear(&colorTmp, tmpSize);

	// Синий
	for(int i = 0; i < tmpSize; i++)
	{
		colorTmp[i] = BLUE(tmp[i]);
	}

    sort(&colorTmp, tmpSize);

	pixel = pixel + OUTBLUE(colorTmp[(edge * edge - 1) / 2]);

	clear(&colorTmp, tmpSize);

	// Записываем в пиксель медиану (центральный пиксель)
	out[width * y + x] = pixel;
}