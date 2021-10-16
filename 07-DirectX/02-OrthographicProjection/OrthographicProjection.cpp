#include<windows.h>
#include<stdio.h>	//For file I/O

#include<d3d11.h>
#include<d3dcompiler.h>

#pragma warning(disable : 4838)
#include "XNAMath_204/xnamath.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define WIN_NAME "DirectX : Orthographic Projection"

//Global Function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global Variable Declaration
FILE *gpFile_rvg = NULL;
char gszLogFileName_rvg[] = "LogFile.log";
int x_rvg;
int y_rvg;

HWND ghwnd_rvg = NULL;

DWORD dwStyle_rvg;
WINDOWPLACEMENT wpPrev_rvg = {sizeof(WINDOWPLACEMENT)};

bool gbActiveWindow_rvg = false;
bool gbEscapeKeyIsPressed_rvg = false;
bool gbFullscreen_rvg = false;

float gClearColor_rvg[4];	//RGBA
IDXGISwapChain *gpIDXGISwapChain_rvg = NULL;
ID3D11Device *gpID3D11Device_rvg = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext_rvg = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView_rvg = NULL;

ID3D11VertexShader *gpID3D11VertexShader_rvg = NULL;
ID3D11PixelShader *gpID3D11PixelShader_rvg = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_rvg = NULL;
ID3D11InputLayout *gpID3D11InputLayout_rvg = NULL;
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_rvg = NULL;

struct CBUFFER
{
	XMMATRIX WorldViewProjectionMatrix;
};
XMMATRIX gOrthographicProjectionMatrix_rvg;


//WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function Declaration
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);
	void ToggleFullscreen(void);

	//Variable Declaration
	WNDCLASSEX wndClass_rvg;
	HWND hwnd_rvg;
	MSG msg_rvg;
	TCHAR szClassName_rvg[] = TEXT("Direct3D11");
	int XScreen_rvg;
	int YScreen_rvg;
	int midX1_rvg;
	int midY1_rvg;
	int midX2_rvg;
	int midY2_rvg;
	bool bDone_rvg = false;

	//Code
	//Centering of Window
	XScreen_rvg = GetSystemMetrics(SM_CXSCREEN);
	YScreen_rvg = GetSystemMetrics(SM_CYSCREEN);
	midX1_rvg = XScreen_rvg / 2;
	midY1_rvg = YScreen_rvg / 2;
	midX2_rvg = 800 / 2;
	midY2_rvg = 600 / 2;
	x_rvg = midX1_rvg - midX2_rvg;
	y_rvg = midY1_rvg - midY2_rvg;

	//Create Log File
	if (fopen_s(&gpFile_rvg, gszLogFileName_rvg, "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting..."), TEXT("ERROR"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile_rvg, "Log File Is Successfully Opened!\n");
		fclose(gpFile_rvg);
	}

	//Initialize WNDCLASSX Structure
	wndClass_rvg.cbSize = sizeof(WNDCLASSEX);
	wndClass_rvg.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass_rvg.cbClsExtra = 0;
	wndClass_rvg.cbWndExtra = 0;
	wndClass_rvg.lpfnWndProc = WndProc;
	wndClass_rvg.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass_rvg.hInstance = hInstance;
	wndClass_rvg.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass_rvg.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass_rvg.lpszClassName = szClassName_rvg;
	wndClass_rvg.lpszMenuName = NULL;
	wndClass_rvg.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//Register WNDCLASSX Structure
	RegisterClassEx(&wndClass_rvg);

	//CreateWindow
	hwnd_rvg = CreateWindow(szClassName_rvg,
							WIN_NAME,
							WS_OVERLAPPEDWINDOW,
							x_rvg,
							y_rvg,
							WIN_WIDTH,
							WIN_HEIGHT,
							NULL,
							NULL,
							hInstance,
							NULL);

	ghwnd_rvg = hwnd_rvg;

	ToggleFullscreen();
	ShowWindow(hwnd_rvg, iCmdShow);
	SetForegroundWindow(hwnd_rvg);
	SetFocus(hwnd_rvg);

	//Initialize D3D
	HRESULT hr_rvg;
	hr_rvg = initialize();
	if (FAILED(hr_rvg))
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "initialize() Failed.\nExitting Now ...\n");
		fclose(gpFile_rvg);
		DestroyWindow(hwnd_rvg);
		hwnd_rvg = NULL;
	}
	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "initialize() Succeeded.\n");
		fclose(gpFile_rvg);
	}

	

	//Message Loop
	while(bDone_rvg == false)
	{
		if (PeekMessage(&msg_rvg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg_rvg.message == WM_QUIT)
			{
				bDone_rvg = true;
			}
			else
			{
				TranslateMessage(&msg_rvg);
				DispatchMessage(&msg_rvg);
			}
		}
		else
		{
			//Render
			display();

			if (gbActiveWindow_rvg == true)
			{
				if (gbEscapeKeyIsPressed_rvg == true)
				{
					bDone_rvg = true;
				}
			}
		}
	}

	//Clean-Up
	uninitialize();

	return((int)msg_rvg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Function Declaration
	HRESULT resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//Variable Declaration
	HRESULT hr_rvg;

	//Code
	switch(iMsg)
	{
		case WM_ACTIVATE:
			if (HIWORD(wParam) == 0)	//if 0, window is active
			{
				gbActiveWindow_rvg = true;
			}
			else 						//if non-zero, window is not active
			{
				gbActiveWindow_rvg = false;
			}
			break;

		case WM_ERASEBKGND:
			return(0);

		case WM_SIZE:
			if (gpID3D11DeviceContext_rvg)
			{
				hr_rvg = resize(LOWORD(lParam), HIWORD(lParam));
				if (FAILED(hr_rvg))
				{
					fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
					fprintf_s(gpFile_rvg, "resize() Failed.\n");
					fclose(gpFile_rvg);
					return(hr_rvg);
				}
				else
				{
					fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
					fprintf_s(gpFile_rvg, "resize() Succeeded.\n");
					fclose(gpFile_rvg);				
				}
			}
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_ESCAPE:	//Case 27
					if (gbEscapeKeyIsPressed_rvg == false)
					{
						gbEscapeKeyIsPressed_rvg = true;
					}
					break;

				case 0x46:	//'F' or 'f'
					if (gbFullscreen_rvg == false)
					{
						ToggleFullscreen();
						gbFullscreen_rvg = true;
					}
					else
					{
						ToggleFullscreen();
						gbFullscreen_rvg = false;
					}
					break;

				default:
					break;
			}
			break;

		case WM_LBUTTONDOWN:
			break;

		case WM_CLOSE:
			uninitialize();
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			break;
	}
	
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	//Variable Declaration
	MONITORINFO mi_rvg;

	//Code
	if (gbFullscreen_rvg == false)
	{
		dwStyle_rvg = GetWindowLong(ghwnd_rvg, GWL_STYLE);
		if (dwStyle_rvg & WS_OVERLAPPEDWINDOW)
		{
			mi_rvg = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd_rvg, &wpPrev_rvg) && GetMonitorInfo(MonitorFromWindow(ghwnd_rvg, MONITORINFOF_PRIMARY), &mi_rvg))
			{
				SetWindowLong(ghwnd_rvg, GWL_STYLE, dwStyle_rvg & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd_rvg, HWND_TOP, mi_rvg.rcMonitor.left,
													mi_rvg.rcMonitor.top,
													mi_rvg.rcMonitor.right - mi_rvg.rcMonitor.left,
													mi_rvg.rcMonitor.bottom - mi_rvg.rcMonitor.top,
													SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd_rvg, GWL_STYLE, dwStyle_rvg | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd_rvg, &wpPrev_rvg);
		SetWindowPos(ghwnd_rvg, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

HRESULT initialize(void)
{
	//Function Declaration
	void uninitialize(void);
	HRESULT resize(int, int);

	//Variable Declaration
	HRESULT hr_rvg;
	D3D_DRIVER_TYPE d3dDriverType_rvg;
	D3D_DRIVER_TYPE d3dDriverTypes_rvg[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE,};
	D3D_FEATURE_LEVEL d3dFeatureLevel_required_rvg = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_aquired_rvg = D3D_FEATURE_LEVEL_10_0;		//default, lowest
	UINT createDeviceFlags_rvg = 0;
	UINT numDriverTypes_rvg = 0;
	UINT numFeatureLevels_rvg = 1;	//based upon d3dFeatureLevel_required_rvg

	//Code
	numDriverTypes_rvg = sizeof(d3dDriverTypes_rvg) / sizeof(d3dDriverTypes_rvg[0]);	//Calculating size of Array

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc_rvg;
	ZeroMemory((void *)&dxgiSwapChainDesc_rvg, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc_rvg.BufferCount = 1;
	dxgiSwapChainDesc_rvg.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc_rvg.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc_rvg.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc_rvg.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc_rvg.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc_rvg.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc_rvg.OutputWindow = ghwnd_rvg;
	dxgiSwapChainDesc_rvg.SampleDesc.Count = 1;
	dxgiSwapChainDesc_rvg.SampleDesc.Quality = 0;
	dxgiSwapChainDesc_rvg.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes_rvg; driverTypeIndex++)
	{
		d3dDriverType_rvg = d3dDriverTypes_rvg[driverTypeIndex];
		hr_rvg = D3D11CreateDeviceAndSwapChain(
			NULL,								//Adapter
			d3dDriverType_rvg,					//Driver Type
			NULL,								//Software
			createDeviceFlags_rvg,				//Flags
			&d3dFeatureLevel_required_rvg,		//Feature Levels
			numFeatureLevels_rvg,				//Num Feature Levels
			D3D11_SDK_VERSION,					//SDK Version
			&dxgiSwapChainDesc_rvg,				//Swap Chain Desc
			&gpIDXGISwapChain_rvg,				//Swap Chain
			&gpID3D11Device_rvg,				//Device
			&d3dFeatureLevel_aquired_rvg,		//Feature Level
			&gpID3D11DeviceContext_rvg);		//Device Context

		if (SUCCEEDED(hr_rvg))
		{
			break;
		}
	}

	if (FAILED(hr_rvg))
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "D3D11CreateDeviceAndSwapChain() Failed.\n");
		fclose(gpFile_rvg);
		return(hr_rvg);
	}

	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "D3D11CreateDeviceAndSwapChain() Succeeded.\n");
		fprintf_s(gpFile_rvg, "The Chosen Driver is Of ");
		if (d3dDriverType_rvg == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFile_rvg, "Hardware Type.\n");
		}
		else if (d3dDriverType_rvg == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFile_rvg, "Warp Type.\n");
		}

		else if (d3dDriverType_rvg == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFile_rvg, "Reference Type.\n");
		}
		else
		{
			fprintf_s(gpFile_rvg, "Unknown Type.\n");
		}

		fprintf_s(gpFile_rvg, "The Supported Highest Feature Level is ");
		if (d3dFeatureLevel_aquired_rvg == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf_s(gpFile_rvg, "11.0.\n\n");
		}
		else if (d3dFeatureLevel_aquired_rvg == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf_s(gpFile_rvg, "10.1.\n\n");	
		}
		else if (d3dFeatureLevel_aquired_rvg == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpFile_rvg, "10.0.\n\n");	
		}
		else
		{
			fprintf_s(gpFile_rvg, "Unknown.\n\n");
		}
		fclose(gpFile_rvg);
	}

	//Initialize Shader, Input Layout, Constant Buffer, etc.
	 
	// ****************** Vertex Shader ******************
	//Create Shader Source Code
	const char* VertexShaderSourceCode_rvg =
											"cbuffer ConstantBuffer" \
											"{" \
												"float4x4 worldViewProjectionMatrix;" \
											"}" \
											"float4 main(float4 pos : POSITION) : SV_POSITION" \
											"{" \
												"float4 position = mul(worldViewProjectionMatrix, pos);" \
												"return(position);" \
											"}";

	//Compile Shader Source Code
	ID3DBlob *pID3DBlob_VertexShaderCode_rvg = NULL;
	ID3DBlob *pID3DBlob_Error_rvg = NULL;

	hr_rvg = D3DCompile(VertexShaderSourceCode_rvg,
						lstrlen(VertexShaderSourceCode_rvg) + 1,
						"VS",
						NULL,
						D3D_COMPILE_STANDARD_FILE_INCLUDE,
						"main",
						"vs_5_0",
						0,
						0,
						&pID3DBlob_VertexShaderCode_rvg,
						&pID3DBlob_Error_rvg);

	if (FAILED(hr_rvg))
	{
		if (pID3DBlob_Error_rvg != NULL)
		{
			fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
			fprintf_s(gpFile_rvg, "D3DCompile() Failed for Vertex Shader : %s.\n", (char *)pID3DBlob_Error_rvg->GetBufferPointer());
			fclose(gpFile_rvg);
			pID3DBlob_Error_rvg->Release();
			pID3DBlob_Error_rvg = NULL;
			return(hr_rvg);
		}
		else
		{
			fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
			fprintf_s(gpFile_rvg, "Some COM Error in Vertex Shader.\n");
			fclose(gpFile_rvg);
			return(hr_rvg);
		}
	}
	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "D3DCompile() Succeeded for Vertex Shader.\n");
		fclose(gpFile_rvg);
	}

	//Create Actual Shader and Feed Compiled Vertex Shader Code
	hr_rvg = gpID3D11Device_rvg->CreateVertexShader(pID3DBlob_VertexShaderCode_rvg->GetBufferPointer(),
													pID3DBlob_VertexShaderCode_rvg->GetBufferSize(),
													NULL,
													&gpID3D11VertexShader_rvg);

	if (FAILED(hr_rvg))
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateVertexShader() Failed!\n");
		fclose(gpFile_rvg);
		return(hr_rvg);
	}
	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateVertexShader() Succeeded!\n\n");
		fclose(gpFile_rvg);
	}

	//Set Vertex Shader as Active
	gpID3D11DeviceContext_rvg->VSSetShader(gpID3D11VertexShader_rvg, 0, 0);

	// ****************** Pixel Shader ******************
	//Create Shader Source Code
	const char* PixelShaderSourceCode_rvg =
										"float4 main(void) : SV_TARGET" \
										"{" \
											"return(float4(1.0f, 1.0f, 1.0f, 1.0f));" \
										"}";

	//Compile Shader Source Code
	ID3DBlob *pID3DBlob_PixelShaderCode_rvg = NULL;
	pID3DBlob_Error_rvg = NULL;

	hr_rvg = D3DCompile(PixelShaderSourceCode_rvg,
		lstrlen(PixelShaderSourceCode_rvg) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode_rvg,
		&pID3DBlob_Error_rvg);

	if (FAILED(hr_rvg))
	{
		if (pID3DBlob_Error_rvg != NULL)
		{
			fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
			fprintf_s(gpFile_rvg, "D3DCompile() Failed for Pixel Shader : %s.\n", (char*)pID3DBlob_Error_rvg->GetBufferPointer());
			fclose(gpFile_rvg);
			pID3DBlob_Error_rvg->Release();
			pID3DBlob_Error_rvg = NULL;
			return(hr_rvg);
		}
		else
		{
			fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
			fprintf_s(gpFile_rvg, "Some COM Error in Pixel Shader.\n");
			fclose(gpFile_rvg);
			return(hr_rvg);
		}
	}
	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "D3DCompile() Succeeded for Pixel Shader.\n");
		fclose(gpFile_rvg);
	}

	//Create Actual Shader and Feed Compiled Pixel Shader Code
	hr_rvg = gpID3D11Device_rvg->CreatePixelShader(pID3DBlob_PixelShaderCode_rvg->GetBufferPointer(),
													pID3DBlob_PixelShaderCode_rvg->GetBufferSize(),
													NULL,
													&gpID3D11PixelShader_rvg);

	if (FAILED(hr_rvg))
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreatePixelShader() Failed!\n");
		fclose(gpFile_rvg);
		return(hr_rvg);
	}
	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreatePixelShader() Succeeded!\n\n");
		fclose(gpFile_rvg);
	}

	//Set Pixel Shader as Active
	gpID3D11DeviceContext_rvg->PSSetShader(gpID3D11PixelShader_rvg, 0, 0);

	pID3DBlob_PixelShaderCode_rvg->Release();
	pID3DBlob_PixelShaderCode_rvg = NULL;

	//Initialize and Create Input Layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc_rvg;
	ZeroMemory((void*)&inputElementDesc_rvg, sizeof(D3D11_INPUT_ELEMENT_DESC));
	inputElementDesc_rvg.SemanticName = "POSITION";
	inputElementDesc_rvg.SemanticIndex = 0;
	inputElementDesc_rvg.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc_rvg.AlignedByteOffset = 0;
	inputElementDesc_rvg.InputSlot = 0;
	inputElementDesc_rvg.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc_rvg.InstanceDataStepRate = 0;

	hr_rvg = gpID3D11Device_rvg->CreateInputLayout(&inputElementDesc_rvg,
													1,
													pID3DBlob_VertexShaderCode_rvg->GetBufferPointer(),
													pID3DBlob_VertexShaderCode_rvg->GetBufferSize(),
													&gpID3D11InputLayout_rvg);

	if (FAILED(hr_rvg))
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateInputLayout() Failed!\n");
		fclose(gpFile_rvg);
		if (pID3DBlob_VertexShaderCode_rvg)
		{
			pID3DBlob_VertexShaderCode_rvg->Release();
			pID3DBlob_VertexShaderCode_rvg = NULL;
		}
		return(hr_rvg);
	}

	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateInputLayout() Succeeded!\n");
		fclose(gpFile_rvg);
		if (pID3DBlob_VertexShaderCode_rvg)
		{
			pID3DBlob_VertexShaderCode_rvg->Release();
			pID3DBlob_VertexShaderCode_rvg = NULL;
		}
	}

	//Set Created Input Layout in Pipeline
	gpID3D11DeviceContext_rvg->IASetInputLayout(gpID3D11InputLayout_rvg);

	const float vertices[] =
	{
		0.0f, 50.0f, 0.0f,		//Apex
		50.0f, -50.0f, 0.0f,	//Right
		-50.0f, -50.0f, 0.0f	//Left
	};

	//Initialize and Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDesc_rvg;
	ZeroMemory((void *)&bufferDesc_rvg, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_rvg.ByteWidth = sizeof(float) * ARRAYSIZE(vertices);
	bufferDesc_rvg.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc_rvg.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc_rvg.Usage = D3D11_USAGE_DYNAMIC;

	hr_rvg = gpID3D11Device_rvg->CreateBuffer(&bufferDesc_rvg,
												NULL,
												&gpID3D11Buffer_VertexBuffer_rvg);

	if (FAILED(hr_rvg))
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateBuffer() Failed for Vertex Buffer!\n");
		fclose(gpFile_rvg);
		return(hr_rvg);
	}

	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateBuffer() Succeeded for Vertex Buffer!\n");
		fclose(gpFile_rvg);
	}

	//Copy Vertices into Buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresources_rvg;
	ZeroMemory((void *)&mappedSubresources_rvg, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext_rvg->Map(gpID3D11Buffer_VertexBuffer_rvg,
									0,
									D3D11_MAP_WRITE_DISCARD,
									0,
									&mappedSubresources_rvg);

	memcpy(mappedSubresources_rvg.pData, vertices, sizeof(vertices));
	gpID3D11DeviceContext_rvg->Unmap(gpID3D11Buffer_VertexBuffer_rvg, 0);

	//Initialize, Create Another Buffer to Hold Another Buffer
	ZeroMemory((void*)&bufferDesc_rvg, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_rvg.ByteWidth = sizeof(CBUFFER);
	bufferDesc_rvg.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc_rvg.CPUAccessFlags = 0;
	bufferDesc_rvg.Usage = D3D11_USAGE_DEFAULT;

	hr_rvg = gpID3D11Device_rvg->CreateBuffer(&bufferDesc_rvg,
												NULL,
												&gpID3D11Buffer_ConstantBuffer_rvg);

	if (FAILED(hr_rvg))
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateBuffer() Failed for Constant Buffer!\n");
		fclose(gpFile_rvg);
		return(hr_rvg);
	}

	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateBuffer() Succeeded for Constant Buffer!\n");
		fclose(gpFile_rvg);
	}

	//Set Constant Buffer into Pipeline
	gpID3D11DeviceContext_rvg->VSSetConstantBuffers(0,
													1,
													&gpID3D11Buffer_ConstantBuffer_rvg);

	//D3D Clear Color
	gClearColor_rvg[0] = 0.0f;
	gClearColor_rvg[1] = 0.0f;
	gClearColor_rvg[2] = 1.0f;
	gClearColor_rvg[3] = 1.0f;

	//Set Projection Matrix as Identity Matrix
	gOrthographicProjectionMatrix_rvg = XMMatrixIdentity();

	//Call Resize for First Time
	hr_rvg = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr_rvg))
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "\nresize() Failed.\n");
		fclose(gpFile_rvg);
		return(hr_rvg);
	}
	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "\nresize() Succeeded.\n");
		fclose(gpFile_rvg);
	}
	return(S_OK);
}

HRESULT resize(int width, int height)
{
	//Code
	HRESULT hr_rvg = S_OK;

	if (height <= 0)
	{
		height = 1;
	}

	//Free any size-dependant resources
	if (gpID3D11RenderTargetView_rvg)
	{
		gpID3D11RenderTargetView_rvg->Release();
		gpID3D11RenderTargetView_rvg = NULL;
	}

	//Resize swap chain buffers accordingly
	gpIDXGISwapChain_rvg->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM,0);

	//Again get back buffer from swap chain
	ID3D11Texture2D *pID3D11Texture2D_BackBuffer_rvg;
	gpIDXGISwapChain_rvg->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pID3D11Texture2D_BackBuffer_rvg);

	//Again get Render target view from d3d11 device using above back buffer
	hr_rvg = gpID3D11Device_rvg->CreateRenderTargetView(pID3D11Texture2D_BackBuffer_rvg, NULL, &gpID3D11RenderTargetView_rvg);

	if (FAILED(hr_rvg))
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateRenderTargetView() Failed.\n");
		fclose(gpFile_rvg);
		return(hr_rvg);
	}

	else
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "ID3D11Device::CreateRenderTargetView() Succeeded.\n");
		fclose(gpFile_rvg);
	}

	pID3D11Texture2D_BackBuffer_rvg->Release();
	pID3D11Texture2D_BackBuffer_rvg = NULL;

	//Set Render Target Viw as Render Target
	gpID3D11DeviceContext_rvg->OMSetRenderTargets(1, &gpID3D11RenderTargetView_rvg, NULL);

	//Set Viewport
	D3D11_VIEWPORT d3dViewPort_rvg;
	d3dViewPort_rvg.TopLeftX = 0;
	d3dViewPort_rvg.TopLeftY = 0;
	d3dViewPort_rvg.Width = (float)width;
	d3dViewPort_rvg.Height = (float)height;
	d3dViewPort_rvg.MinDepth = 0.0f;
	d3dViewPort_rvg.MaxDepth = 1.0f;
	gpID3D11DeviceContext_rvg->RSSetViewports(1, &d3dViewPort_rvg);

	//Set Orthographic Projection
	if (width <= height )
	{
		gOrthographicProjectionMatrix_rvg = XMMatrixOrthographicOffCenterLH(-100.0f,
																			100.0f,
																			-100.0f * ((float)height / (float)width),
																			100.0f * ((float)height / (float)width),
																			-100.0f,
																			100.0f);
	}
	else
	{
		gOrthographicProjectionMatrix_rvg = XMMatrixOrthographicOffCenterLH(-100.0f * ((float)width / (float)height),
																			100.0f * ((float)width / (float)height),
																			-100.0f,
																			100.0f,
																			-100.0f,
																			100.0f);
	}

	return(hr_rvg);
}

void display(void)
{
	//Local Variable Declaration
	UINT stride_rvg = sizeof(float) * 3;
	UINT offset_rvg = 0;

	//Code
	//Clear render target view to a chosen color
	gpID3D11DeviceContext_rvg->ClearRenderTargetView(gpID3D11RenderTargetView_rvg, gClearColor_rvg);

	//Select which Vertex Buffer to Display
	gpID3D11DeviceContext_rvg->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_rvg, &stride_rvg, &offset_rvg);

	//Select Geometry Primitive
	gpID3D11DeviceContext_rvg->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Matrix Multiplication
	XMMATRIX worldMatrix_rvg = XMMatrixIdentity();
	XMMATRIX viewMatrix_rvg = XMMatrixIdentity();
	XMMATRIX wvpMatrix_rvg = XMMatrixIdentity();

	wvpMatrix_rvg = worldMatrix_rvg * viewMatrix_rvg * gOrthographicProjectionMatrix_rvg;

	//Load data into Constant Buffer
	CBUFFER constantBuffer_rvg;
	ZeroMemory((void*)&constantBuffer_rvg, sizeof(CBUFFER));
	constantBuffer_rvg.WorldViewProjectionMatrix = wvpMatrix_rvg;

	//Send Uniform to Shader
	gpID3D11DeviceContext_rvg->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_rvg, 0, NULL, &constantBuffer_rvg, 0, 0);

	//Draw Vertex Buffer
	gpID3D11DeviceContext_rvg->Draw(3, 0);

	//Switch between front and Back Buffers
	gpIDXGISwapChain_rvg->Present(0, 0);
}

void uninitialize(void)
{
	//Code
	if (gpID3D11Buffer_ConstantBuffer_rvg)
	{
		gpID3D11Buffer_ConstantBuffer_rvg->Release();
		gpID3D11Buffer_ConstantBuffer_rvg = NULL;
	}

	if (gpID3D11InputLayout_rvg)
	{
		gpID3D11InputLayout_rvg->Release();
		gpID3D11InputLayout_rvg = NULL;
	}

	if (gpID3D11Buffer_VertexBuffer_rvg)
	{
		gpID3D11Buffer_VertexBuffer_rvg->Release();
		gpID3D11Buffer_VertexBuffer_rvg = NULL;
	}

	if (gpID3D11PixelShader_rvg)
	{
		gpID3D11PixelShader_rvg->Release();
		gpID3D11PixelShader_rvg = NULL;
	}

	if (gpID3D11VertexShader_rvg)
	{
		gpID3D11VertexShader_rvg->Release();
		gpID3D11VertexShader_rvg = NULL;
	}

	if (gpID3D11RenderTargetView_rvg)
	{
		gpID3D11RenderTargetView_rvg->Release();
		gpID3D11RenderTargetView_rvg = NULL;
	}

	if (gpIDXGISwapChain_rvg)
	{
		gpIDXGISwapChain_rvg->Release();
		gpIDXGISwapChain_rvg = NULL;
	}

	if (gpID3D11DeviceContext_rvg)
	{
		gpID3D11DeviceContext_rvg->Release();
		gpID3D11DeviceContext_rvg = NULL;
	}

	if (gpID3D11Device_rvg)
	{
		gpID3D11Device_rvg->Release();
		gpID3D11Device_rvg = NULL;
	}

	if (gpFile_rvg)
	{
		fopen_s(&gpFile_rvg, gszLogFileName_rvg, "a+");
		fprintf_s(gpFile_rvg, "uninitialize() Succeeded.\n");
		fprintf_s(gpFile_rvg,  "Log File is Successfully Closed!\n");
		fclose(gpFile_rvg);
	}
}
