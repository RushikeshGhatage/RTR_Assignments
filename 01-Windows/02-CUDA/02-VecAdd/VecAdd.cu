//Header
#include <stdio.h>
#include <cuda.h>
#include "helper_timer.h"

//Global Variables
//Odd number 11444777 is deliberate illustration (nVidia OpenCL Samples)
int iNumberOfArrayElements = 5;

float *hostInput1 = NULL;
float *hostInput2 = NULL;
float *hostOutput = NULL;
float *gold = NULL;

float *deviceInput1 = NULL;
float *deviceInput2 = NULL;
float *deviceOutput = NULL;

float timeOnCPU;
float timeOnGPU;

//CUDA KERNEL DEFINITION
// Global Kernel Function Definition
__global__ void vecAdd(float *in1, float *in2, float *out, int len)
{
	//Variable Declaration
	int i = blockIdx.x * blockDim.x + threadIdx.x;

	//Code
	if (i < len)
	{
		out[i] = in1[i] + in2[i];
	}
}

int main(int argc, char *argv[])
{
	//Function Declaration
	void fillFloatArrayWithRandomNumbers(float *, int);
	void vecAddHost(const float *, const float *, float *, int);
	void cleanup(void);

	//Code
	//Allocate Host-Memory
	hostInput1 = (float *)malloc(sizeof(float) * iNumberOfArrayElements);
	if (hostInput1 == NULL)
	{
		printf("CPU Memory fatal Error = Cannot Allocate Enough emory For Host Input Array 1.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostInput2 = (float *)malloc(sizeof(float) * iNumberOfArrayElements);
	if (hostInput2 == NULL)
	{
		printf("CPU Memory fatal Error = Cannot Allocate Enough emory For Host Input Array 2.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostOutput = (float *)malloc(sizeof(float) * iNumberOfArrayElements);
	if (hostOutput == NULL)
	{
		printf("CPU Memory fatal Error = Cannot Allocate Enough emory For Host Output Array 1.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	gold = (float *)malloc(sizeof(float) * iNumberOfArrayElements);
	if (gold == NULL)
	{
		printf("CPU Memory fatal Error = Cannot Allocate Enough emory For Gold Output Array 1.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	//Fill Above input host vectors with Random but Hard-Coded data
	fillFloatArrayWithRandomNumbers(hostInput1, iNumberOfArrayElements);
	fillFloatArrayWithRandomNumbers(hostInput2, iNumberOfArrayElements);

	//Allocate Device Memory
	cudaError_t err = cudaSuccess;
	err = cudaMalloc((void **)&deviceInput1, sizeof(float) * iNumberOfArrayElements);
	if (err != cudaSuccess)
	{
		printf("GPU Memory fatal Error = %s in file name %s at Line No. %d.\n",cudaGetErrorString(err), __FILE__, __LINE__);
		cleanup();
		exit(EXIT_FAILURE);
	}

	err = cudaMalloc((void **)&deviceInput2, sizeof(float) * iNumberOfArrayElements);
	if (err != cudaSuccess)
	{
		printf("GPU Memory fatal Error = %s in file name %s at Line No. %d.\n",cudaGetErrorString(err), __FILE__, __LINE__);
		cleanup();
		exit(EXIT_FAILURE);
	}

	err = cudaMalloc((void **)&deviceOutput, sizeof(float) * iNumberOfArrayElements);
	if (err != cudaSuccess)
	{
		printf("GPU Memory fatal Error = %s in file name %s at Line No. %d.\n",cudaGetErrorString(err), __FILE__, __LINE__);
		cleanup();
		exit(EXIT_FAILURE);
	}

	//Copy Host Memory Contents to device memory
	err = cudaMemcpy(deviceInput1, hostInput1, sizeof(float) * iNumberOfArrayElements, cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		printf("GPU Memory fatal Error = %s in file name %s at Line No. %d.\n",cudaGetErrorString(err), __FILE__, __LINE__);
		cleanup();
		exit(EXIT_FAILURE);
	}

	err = cudaMemcpy(deviceInput2, hostInput2, sizeof(float) * iNumberOfArrayElements, cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		printf("GPU Memory fatal Error = %s in file name %s at Line No. %d.\n",cudaGetErrorString(err), __FILE__, __LINE__);
		cleanup();
		exit(EXIT_FAILURE);
	}

	//Cuda Kernel Configuration
	dim3 dimGrid = dim3(ceil(iNumberOfArrayElements / 256.0), 1, 1);
	dim3 dimBlock = dim3(256, 1, 1);

	//Start Timer
	StopWatchInterface *timer = NULL;
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);

	vecAdd<<<dimGrid,dimBlock>>>(deviceInput1, deviceInput2, deviceOutput, iNumberOfArrayElements);

	//Stop Timer
	sdkStopTimer(&timer);
	timeOnGPU = sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);

	// Copy devce memory to host memory
	err = cudaMemcpy(hostOutput, deviceOutput, sizeof(float) * iNumberOfArrayElements, cudaMemcpyDeviceToHost);
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s in File name %s at Line number %d\n",cudaGetErrorString(err), __FILE__, __LINE__);
		cleanup();
		exit(EXIT_FAILURE);
	}

	//Results
	vecAddHost(hostInput1, hostInput2, gold, iNumberOfArrayElements);

	//Compare results for Golden-Host
	const float epsilon = 0.000001f;
	bool bAccuracy = true;
	int breakValue = 0;
	int i;
	for(i = 0; i < iNumberOfArrayElements; i++)
	{
		float val1 = gold[i];
		float val2 = hostOutput[i];
		if (fabs(val1 - val2) > epsilon)
		{
			bAccuracy = false;
			breakValue = i;
			break;
		}
	}

	if (bAccuracy == false)
	{
		printf("Break Value = %d\n", breakValue);
	}

	char str[125];
	if (bAccuracy == true)
	{
		sprintf(str, "%s", "Comparison of Output Arrays on CPU and GPU are Accurate within the limit of 0.000001");
	}
	else
	{
		sprintf(str, "%s", "Not All Comparison of Output Arrays on CPU and GPU are Accurate within the limit of 0.000001");	
	}

	printf("1st Array from 0th Element %.6f to %dth element %.6f\n",hostInput1[0], iNumberOfArrayElements - 1 ,hostInput1[iNumberOfArrayElements - 1]);
	printf("2nd Array from 0th Element %.6f to %dth element %.6f\n",hostInput2[0], iNumberOfArrayElements - 1 ,hostInput2[iNumberOfArrayElements - 1]);
	printf("Grid Dimension = (%d, 1, 1) and Block Dimension = (%d, 1, 1)\n", dimGrid.x, dimBlock.x);
	printf("Sum of Each Element from Above 2 Arrays creates 3rd Arrays :\n");
	printf("3rd Array is From 0th Element %.6f to %dth element %.6f\n", hostOutput[0], iNumberOfArrayElements - 1, hostOutput[iNumberOfArrayElements - 1]);
	printf("The Time taken to do Above Addition on CPU = %.6f (ms)\n", timeOnCPU);
	printf("The Time taken to do Above Addition on GPU = %.6f (ms)\n", timeOnGPU);
	printf("%s\n", str);

	//Total Cleanup
	cleanup();
	return(0);
}

void cleanup(void)
{

	//Code

	//Free Allocated Device Memory
	if (deviceInput1)
	{
		cudaFree(deviceInput1);
		deviceInput1 = NULL;
	}


	if (deviceInput2)
	{
		cudaFree(deviceInput2);
		deviceInput2 = NULL;
	}

	if (deviceOutput)
	{
		cudaFree(deviceOutput);
		deviceOutput = NULL;
	}

	//Free Allocated Host Memory
	if (hostInput1)
	{
		cudaFree(hostInput1);
		hostInput1 = NULL;
	}


	if (hostInput2)
	{
		cudaFree(hostInput2);
		hostInput2 = NULL;
	}

	if (hostOutput)
	{
		cudaFree(hostOutput);
		hostOutput = NULL;
	}

	if (gold)
	{
		free(gold);
		gold = NULL;
	}
	
}

void fillFloatArrayWithRandomNumbers(float *pFloatArray, int iSize)
{
	//Code
	int i;
	const float fScale = 1.0f / (float)RAND_MAX;
	for (i = 0; i < iSize; ++i)
	{
		pFloatArray[i] = fScale * rand();
	}
}

//"Golden" Host Processing vector addition function for comparison purposes
void vecAddHost(const float* pFloatData1, const float* pFloatData2, float* pFloatResult, int iNumElement)
{
	int i;
	StopWatchInterface *timer = NULL;
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);

	for (i = 0; i < iNumberOfArrayElements; i++)
	{
		pFloatResult[i] = pFloatData1[i] + pFloatData2[i];
	}

	sdkStopTimer(&timer);
	timeOnCPU = sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);
}
