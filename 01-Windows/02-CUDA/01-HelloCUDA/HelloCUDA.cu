//Header
#include<stdio.h>
#include<cuda.h>

//Global Variable
int inputLength_rvg = 5;

float *hostInput1_rvg = NULL;
float *hostInput2_rvg = NULL;
float *hostOutput_rvg = NULL;

float *deviceInput1_rvg = NULL;
float *deviceInput2_rvg = NULL;
float *deviceOutput_rvg = NULL;

//Global kernel Function definition
__global__ void vecAdd(float *in1, float *in2, float *out, int len)
{
	//Variable Declaration
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	
	//Code
	if(i <= len)
	{
		out[i] = in1[i] + in2[i];
	}
}

int main(int argc, char *argv[])
{
	//Function Declaration
	void cleanup(void);

	//Code
	//Allocate Host-Memory
	hostInput1_rvg = (float *)malloc(inputLength_rvg * sizeof(float));
	if(hostInput1_rvg == NULL)
	{
		printf("CPU Memory Fatal Error = Can Not Allocate Enough Memory For Host Input Array 1.\nExiting ... \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostInput2_rvg = (float *)malloc(inputLength_rvg * sizeof(float));
	if(hostInput2_rvg == NULL)
	{
		printf("CPU Memory Fatal Error = Can Not Allocate Enough Memory For Host Input Array 2.\nExiting ... \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostOutput_rvg = (float *)malloc(inputLength_rvg * sizeof(float));
	if(hostOutput_rvg == NULL)
	{
		printf("CPU Memory Fatal Error = Can Not Allocate Enough Memory For Host Output Array.\nExiting ... \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	//Fill above input host vectors with arbitary but Hard-Coded Data
	hostInput1_rvg[0] = 101.0;
	hostInput1_rvg[1] = 102.0;
	hostInput1_rvg[2] = 103.0;
	hostInput1_rvg[3] = 104.0;
	hostInput1_rvg[4] = 105.0;

	hostInput2_rvg[0] = 201.0;
	hostInput2_rvg[1] = 202.0;
	hostInput2_rvg[2] = 203.0;
	hostInput2_rvg[3] = 204.0;
	hostInput2_rvg[4] = 205.0;

	//Allocate Device Memory
	int size = inputLength_rvg * sizeof(float);
	cudaError_t err = cudaSuccess;

	err = cudaMalloc((void **)&deviceInput1_rvg,size); 
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s at Line No. %d\nExiting ...\n", cudaGetErrorString(err), __FILE__, __LINE__ );
		cleanup();
		exit(EXIT_FAILURE);
	}

	err = cudaMalloc((void **)&deviceInput2_rvg,size); 
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s at Line No. %d\nExiting ...\n", cudaGetErrorString(err), __FILE__, __LINE__ );
		cudaFree(deviceInput1_rvg);
		cleanup();
		exit(EXIT_FAILURE);
	}

	err = cudaMalloc((void **)&deviceOutput_rvg,size); 
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s at Line No. %d\nExiting ...\n", cudaGetErrorString(err), __FILE__, __LINE__ );
		cleanup();
		exit(EXIT_FAILURE);
	}


	//Copy Host Memory Contents to Device Memory
	err  = cudaMemcpy(deviceInput1_rvg, hostInput1_rvg, size, cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s at Line No. %d\nExiting ...\n", cudaGetErrorString(err), __FILE__, __LINE__ );
		cleanup();
		exit(EXIT_FAILURE);
	}

	err  = cudaMemcpy(deviceInput2_rvg, hostInput2_rvg, size, cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s at Line No. %d\nExiting ...\n", cudaGetErrorString(err), __FILE__, __LINE__ );
		cleanup();
		exit(EXIT_FAILURE);
	}

	//Cuda Kernel Configuration
	dim3 DimGrid = dim3(ceil(inputLength_rvg/256.0),1,1);
	dim3 DimBlock = dim3(256, 1,1);
	vecAdd<<<DimGrid, DimBlock>>>(deviceInput1_rvg, deviceInput2_rvg, deviceOutput_rvg, inputLength_rvg);

	//Copy Device Memory to Host Memory
	err  = cudaMemcpy(hostOutput_rvg, deviceOutput_rvg, size, cudaMemcpyDeviceToHost);
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s at Line No. %d\nExiting ...\n", cudaGetErrorString(err), __FILE__, __LINE__ );
		cleanup();
		exit(EXIT_FAILURE);
	}

	//Results
	int i;
	for (i = 0; i < inputLength_rvg; i++)
	{
		printf("%f + %f = %f\n", hostInput1_rvg[i], hostInput2_rvg[i], hostOutput_rvg[i]);
	}

	//Total Cleanup
	cleanup();

	return(0);

}

void cleanup(void)
{

	//Code

	//Free Allocated Device Memory
	if (deviceInput1_rvg)
	{
		cudaFree(deviceInput1_rvg);
		deviceInput1_rvg = NULL;
	}


	if (deviceInput2_rvg)
	{
		cudaFree(deviceInput2_rvg);
		deviceInput2_rvg = NULL;
	}

	if (deviceOutput_rvg)
	{
		cudaFree(deviceOutput_rvg);
		deviceOutput_rvg = NULL;
	}

	//Free Allocated Host Memory
	if (hostInput1_rvg)
	{
		cudaFree(hostInput1_rvg);
		hostInput1_rvg = NULL;
	}


	if (hostInput2_rvg)
	{
		cudaFree(hostInput2_rvg);
		hostInput2_rvg = NULL;
	}

	if (hostOutput_rvg)
	{
		cudaFree(hostOutput_rvg);
		hostOutput_rvg = NULL;
	}
	
}






