__kernel void Filter(
	__global __read_only unsigned int *in1,
	__global __write_only unsigned int *in2,
	__global __read_only unsigned int *tmp,
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

    unsigned int x;
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
	__global __read_only unsigned int *in1,
	__global __write_only unsigned int *in2,
	int width,
	int height)
{
	const int i = get_global_id(0);
	const int j = get_global_id(1);

	if ((i >= width) || (j >= height)) return;

	in2[j * width + i] = in1[j * width + i];
}