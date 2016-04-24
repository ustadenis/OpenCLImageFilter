__kernel void Filter(
	__global __read_only unsigned char *in1,
	__global __write_only unsigned char *in2,
	__global __read_only unsigned char *tmp,
	int stride,
	int edge)
{
	const int x = get_global_id(0);
	const int y = get_global_id(1);
	const int width = get_global_size(0);
	const int height = get_global_size(1);

	//if ((x >= width) || (y >= height)) return;

	in1 = in1 + stride * y;
	in2 = in2 + stride * y;

	//in2[x * 4] = 255; //blue
    //in2[x * 4 + 1] = 50; //green
    //in2[x * 4 + 2] = 20; //red
    //in2[x * 4 + 3] = 150; //alpha

	int size = stride * height;
	
	for(int l = 0; l < edge; l++)
	{
		for(int r = 0; r < edge; r++)
		{
			if((x + r) * 4 < size && (x + r) * 4 > 0)
			{
				tmp[l * edge + r] = (in1 + stride * l)[(x + r) * 4];
			}
			else
			{
				tmp[l * edge + r] = (in1 + stride * l)[x * 4];
			}
		}
	}

	unsigned char p;
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

	in2[x * 4] = tmp[(edge * edge - 1) / 2];

	for(int l = 0; l < edge; l++)
	{
		for(int r = 0; r < edge; r++)
		{
			if((x + r) * 4 + 1 < size && (x + r) * 4 + 1 > 0)
			{
				tmp[l * edge + r] = (in1 + stride * l)[(x + r) * 4 + 1];
			}
			else
			{
				tmp[l * edge + r] = (in1 + stride * l)[x * 4 + 1];
			}
		}
	}

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

	in2[x * 4 + 1] = tmp[(edge * edge - 1) / 2];

	for(int l = 0; l < edge; l++)
	{
		for(int r = 0; r < edge; r++)
		{
			if((x + r) * 4 + 2 < size && (x + r) * 4 + 2 > 0)
			{
				tmp[l * edge + r] = (in1 + stride * l)[(x + r) * 4 + 2];
			}
			else
			{
				tmp[l * edge + r] = (in1 + stride * l)[x * 4 + 2];
			}
		}
	}

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

	in2[x * 4 + 2] = tmp[(edge * edge - 1) / 2];
	in2[x * 4 + 3] = in1[x * 4 + 3];
}

__kernel void AddNoize(
	__global __read_write unsigned char *in1,
	__global __read_only unsigned char *noize,
	int stride,
	int noizeLevel)
{
	const int x = get_global_id(0);
	const int y = get_global_id(1);
	const int width = get_global_size(0);
	const int height = get_global_size(1);

	in1 = in1 + stride * y;

	if(noize[y * width + x] > 0)
	{
		in1[x * 4] = 255;
		in1[x * 4 + 1] = 255;
		in1[x * 4 + 2] = 255;
		in1[x * 4 + 3] = 255;
	}
}