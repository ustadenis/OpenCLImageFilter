#define RED(x) ((x & 0x00FF0000) >> 16)
#define GREEN(x) ((x & 0x0000FF00) >> 8)
#define BLUE(x) ((x & 0x000000FF) >> 0)

#define OUTRED(x) (x << 16)
#define OUTGREEN(x) (x << 8)
#define OUTBLUE(x) (x << 0)

/**
 *
 * ������� ����������
 *
 * @param ��������� �� ������
 * @param ������ �������
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
 * ���� ���������� �����
 *
 * @param in - ����������� �����������
 * @param out - ��������������� �����������
 * @param edge - ������� ����������
 */
__kernel void Filter(
	__global unsigned int *in,
	__global unsigned int *out,
	int edge)
{
	const int x = get_global_id(0); // �������� ������ � 0 ���������
	const int y = get_global_id(1); // �������� ������ � 1 ���������
	const int width = get_global_size(0); // �������� ����������� 0 ���������
	const int height = get_global_size(1); // �������� ����������� 1 ���������

	if ((x >= width) || (y >= height)) return; // ��������� ��� ������� �� ����� �� ���������

	unsigned int tmp[1000]; // �������� ������ ��� ������������ ����
	unsigned char colorTmp[1000]; // ������ ��� ������
	int tmpSize = edge * edge;
	unsigned int pixel = 0x000000;

	// ����� ���� �������� edge x edge
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
	
	// �������
	for(int i = 0; i < tmpSize; i++)
	{
		colorTmp[i] = RED(tmp[i]);
	}

    sort(&colorTmp, tmpSize);

	pixel = pixel + OUTRED(colorTmp[(edge * edge - 1) / 2]);

	// �������
	for(int i = 0; i < tmpSize; i++)
	{
		colorTmp[i] = GREEN(tmp[i]);
	}

    sort(&colorTmp, tmpSize);

	pixel = pixel + OUTGREEN(colorTmp[(edge * edge - 1) / 2]);

	// �����
	for(int i = 0; i < tmpSize; i++)
	{
		colorTmp[i] = BLUE(tmp[i]);
	}

    sort(&colorTmp, tmpSize);

	pixel = pixel + OUTBLUE(colorTmp[(edge * edge - 1) / 2]);

	// ���������� � ������� ������� (����������� �������)
	out[width * y + x] = pixel;
}