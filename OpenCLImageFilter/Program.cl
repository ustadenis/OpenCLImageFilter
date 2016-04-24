__kernel void Filter(
	__global __read_only unsigned char *in1,
	__global __write_only unsigned char *in2,
	int edge)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);
	const int width = get_global_size(0);
	const int height = get_global_size(1);

	if ((i >= width) || (j >= height)) return;

	unsigned char tmp[100];
	
	for(int l = 0; l < edge; l++)
	{
		for(int r = 0; r < edge; r++)
		{
			if(i + r < width || j + l < height)
				tmp[l * edge + r] = in1[(j + l) * width + (i + r)];
			else
				tmp[l * edge + r] = 0;
		}
	}

    unsigned char x;
    for(int k = 0; k < 100; k++)
	{            
        for(int s = 100 - 1; s > k; s--)
		{     
            if(tmp[s - 1] > tmp[s])
			{
                x = tmp[s - 1];
                tmp[s - 1] = tmp[s];
                tmp[s] = x;
            }
        }
    }

	in2[j * width + i] = tmp[100 - 1 - edge * edge / 2];
}

__kernel void AddNoize(
	__global __read_write unsigned char *in1,
	__global __read_only unsigned char *noize,
	int noizeLevel)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);
	const int width = get_global_size(0);
	const int height = get_global_size(1);

	if(noize[j * width + i] > 0)
		in1[j * width + i] = 255;
}