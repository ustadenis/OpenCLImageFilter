/**
 * Ядро фильтрации окном
 *
 * @param in - зашумленное изображение
 * @param out - отфильтрованное изображение
 * @param edge - глубина фильтрации
 */
__kernel void Filter(
	__global __read_only unsigned int *in,
	__global __write_only unsigned int *out,
	int edge)
{
	const int x = get_global_id(0); // Получаем индекс в 0 измерение
	const int y = get_global_id(1); // Получаем индекс в 1 измерение
	const int width = get_global_size(0); // Получаем размерность 0 измерения
	const int height = get_global_size(1); // Получаем размерность 1 измерения

	if ((x >= width) || (y >= height)) return; // Проверяем что индексы не вышли за диаппазон

	unsigned int tmp[1000]; // Создадим массив для сортировкифильтрующего окна
	
	// Берем окно размером edge x edge
	for(int l = 0; l < edge; l++)
	{
		for(int r = 0; r < edge; r++)
		{
			if(x + r < width && l + y < height)
				tmp[l * edge + r] = in[(width * (y + l)) + (x + r)];
		}
	}

	// Сортируем пиксели (пока пузырек, лень переписывать, время уже час доходит)
	unsigned int p;
    for(int k = 0; k < edge * edge; k++)
	{            
        for(int s = edge * edge - 1; s > k; s--)
		{     
            if(tmp[s - 1] > tmp[s])
			{
                p = tmp[s - 1];
                tmp[s - 1] = tmp[s];
                tmp[s] = p;
            }
        }
    }

	// Записываем в пиксель медиану (центральный пиксель)
	out[width * y + x] = tmp[(edge * edge - 1) / 2];
}

/**
 * Ядро фильтрации окном
 *
 * @param in - зашумленное изображение
 * @param noize - маска шума
 */
__kernel void AddNoize(
	__global __read_write unsigned int *in,
	__global __read_only unsigned int *noize)
{
	const int x = get_global_id(0); // Получаем индекс в 0 измерение
	const int y = get_global_id(1); // Получаем индекс в 1 измерение
	const int width = get_global_size(0); // Получаем размерность 0 измерения

	// Накладываем маску шума на изображение
	if(noize[width * y + x] > 0)
	{
		in[width * y + x] = noize[width * y + x];
	}
}