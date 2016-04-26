/**
 * ���� ���������� �����
 *
 * @param in - ����������� �����������
 * @param out - ��������������� �����������
 * @param edge - ������� ����������
 */
__kernel void Filter(
	__global __read_only unsigned int *in,
	__global __write_only unsigned int *out,
	int edge)
{
	const int x = get_global_id(0); // �������� ������ � 0 ���������
	const int y = get_global_id(1); // �������� ������ � 1 ���������
	const int width = get_global_size(0); // �������� ����������� 0 ���������
	const int height = get_global_size(1); // �������� ����������� 1 ���������

	if ((x >= width) || (y >= height)) return; // ��������� ��� ������� �� ����� �� ���������

	unsigned int tmp[1000]; // �������� ������ ��� ���������������������� ����
	
	// ����� ���� �������� edge x edge
	for(int l = 0; l < edge; l++)
	{
		for(int r = 0; r < edge; r++)
		{
			if(x + r < width && l + y < height)
				tmp[l * edge + r] = in[(width * (y + l)) + (x + r)];
		}
	}

	// ��������� ������� (���� �������, ���� ������������, ����� ��� ��� �������)
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

	// ���������� � ������� ������� (����������� �������)
	out[width * y + x] = tmp[(edge * edge - 1) / 2];
}

/**
 * ���� ���������� �����
 *
 * @param in - ����������� �����������
 * @param noize - ����� ����
 */
__kernel void AddNoize(
	__global __read_write unsigned int *in,
	__global __read_only unsigned int *noize)
{
	const int x = get_global_id(0); // �������� ������ � 0 ���������
	const int y = get_global_id(1); // �������� ������ � 1 ���������
	const int width = get_global_size(0); // �������� ����������� 0 ���������

	// ����������� ����� ���� �� �����������
	if(noize[width * y + x] > 0)
	{
		in[width * y + x] = noize[width * y + x];
	}
}