__global__ void sinewave_kernel(float4 *pos, unsigned int width, unsigned int height, float animationTime)
{
	//Code
	unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;

	float u = x / (float)width;
	float v = y / (float)height;

	u = u * 2.0f - 1.0f;
	v = v * 2.0f - 1.0f;
	float freq = 4.0f;

	float w = sinf(u * freq + animationTime) * cosf(v * freq + animationTime) * 0.5f;

	pos[y * width + x] = make_float4(u, w, v, 1.0f);
}

void launchCUDAKernel(float4 *pos, unsigned int width, unsigned int height, float animationTime)
{
	//Code
	dim3 block(8, 8, 1);
	dim3 grid(width / block.x, height / block.y, 1);

	sinewave_kernel<<<grid,block>>>(pos, width, height, animationTime);

}