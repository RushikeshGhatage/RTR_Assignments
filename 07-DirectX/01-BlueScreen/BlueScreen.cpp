#include<windows.h>
#include<stdio.h>	//For file I/O

#include<d3d11.h>

#pragma comment(lib, "d3d11.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define WIN_NAME "DirectX : BlueScreen"

//Global Function Declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global Variable Declaration
FILE *gpFile_rvg = NULL;
char gszLogFileName_rvg[] = "Log.log";
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
			fprintf_s(gpFile_rvg, "11.0.\n");
		}
		else if (d3dFeatureLevel_aquired_rvg == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf_s(gpFile_rvg, "10.1.\n");	
		}
		else if (d3dFeatureLevel_aquired_rvg == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf_s(gpFile_rvg, "10.0.\n");	
		}
		else
		{
			fprintf_s(gpFile_rvg, "Unknown.\n");
		}
		fclose(gpFile_rvg);
	}

	//D3D Clear Color (Blue)
	gClearColor_rvg[0] = 0.0f;
	gClearColor_rvg[1] = 0.0f;
	gClearColor_rvg[2] = 1.0f;
	gClearColor_rvg[3] = 1.0f;

	//Call Resize for First Time
	hr_rvg = resize(WIN_WIDTH, WIN_HEIGHT);
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
	return(S_OK);
}

HRESULT resize(int width, int height)
{
	//Code
	HRESULT hr_rvg = S_OK;

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

	return(hr_rvg);
}

void display(void)
{
	//Code
	//Clear render target view to a chosen color
	gpID3D11DeviceContext_rvg->ClearRenderTargetView(gpID3D11RenderTargetView_rvg, gClearColor_rvg);

	//Switch between front and Back Buffers
	gpIDXGISwapChain_rvg->Present(0, 0);
}

void uninitialize(void)
{
	//Code
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