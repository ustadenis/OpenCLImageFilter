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
		int minElIndex = k;
        for(int s = k; s < n; s++)
		{     
            if(tmp[minElIndex] > tmp[s])
			{
				minElIndex = s;
            }
        }
		p = tmp[k];
        tmp[k] = tmp[minElIndex];
        tmp[minElIndex] = p;
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
	unsigned int pixel = 0x000000;

	// Берем окно размером edge x edge
	for(int l = -edge/2; l < edge/2; l++)
	{
		int line = l;
		if(l + y >= height)
		{
			line = height - (l + y);
		}
		else if(y + l < 0)
		{
			line = -(y + l);
		}
		for(int r = -edge/2; r < edge/2; r++)
		{
			int raw = r;
			if(r + x >= width)
			{
				raw = width - (r + x);
			}
			else if(r + x < 0)
			{
				raw = -(r + x);
			}

			tmp[(l + edge/2) * edge + (r + edge/2)] = in[(width * (y + line)) + (x + raw)];
		}
	}
	
	// Красный
	for(int i = 0; i < tmpSize; i++)
	{
		colorTmp[i] = RED(tmp[i]);
	}

    sort(&colorTmp, tmpSize);

	pixel = pixel + OUTRED(colorTmp[(edge * edge - 1) / 2]);

	// Зеленый
	for(int i = 0; i < tmpSize; i++)
	{
		colorTmp[i] = GREEN(tmp[i]);
	}

    sort(&colorTmp, tmpSize);

	pixel = pixel + OUTGREEN(colorTmp[(edge * edge - 1) / 2]);

	// Синий
	for(int i = 0; i < tmpSize; i++)
	{
		colorTmp[i] = BLUE(tmp[i]);
	}

    sort(&colorTmp, tmpSize);

	pixel = pixel + OUTBLUE(colorTmp[(edge * edge - 1) / 2]);

	// Записываем в пиксель медиану (центральный пиксель)
	out[width * y + x] = pixel;
}