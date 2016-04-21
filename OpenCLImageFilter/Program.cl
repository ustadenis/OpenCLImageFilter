__kernel void Filter(
	__global __read_only unsigned char *in1,
	__global __write_only unsigned char *in2,
	__global __read_only unsigned char *tmp,
	int width,
	int height,
	int edge)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);

	if ((i >= width - edge) || (j >= height - edge)) return;
	//in2[j * width + i] = in1[j * width + i];
	for(int l = 0; l < edge; l++)
	{
		for(int r = 0; r <= edge; r++)
		{
			tmp[l * edge + r] = in1[(j + l) * width + (i + (4*r+1))];
		}
	}

    unsigned char x;
    for(int k = 0; k < edge * edge; k++)
	{            
        for(int s = edge * edge - 1; s > k; s--)
		{     
            if(tmp[s - 1] > tmp[s])
			{
                x = tmp[s - 1];
                tmp[s - 1] = tmp[s];
                tmp[s] = x;
            }
        }
    }

	in2[j * width + i] = tmp[edge * edge / 2];
}

__kernel void AddNoize(
	__global __read_only unsigned char *in1,
	__global __write_only unsigned char *in2,
	int counter,
	int noizeLevel,
	int width,
	int height)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);

	if ((i >= width) || (j >= height)) return;

	counter = counter + 1;

	if(counter < (noizeLevel * width * height / 100))
	{
		in2[j * width + i] = 0;
	}
	else 
	{
		in2[j * width + i] = in1[j * width + i];
	}
}