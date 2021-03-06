#include "Game/Game.h"

using namespace Microsoft::WRL;
namespace library
{
    

     
    /*--------------------------------------------------------------------
      Global Variables
    --------------------------------------------------------------------*/
    HINSTANCE               g_hInst = nullptr;
    HWND                    g_hWnd = nullptr;
    D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
    Microsoft::WRL::ComPtr<ID3D11Device> g_pd3dDevice;
    Microsoft::WRL::ComPtr<ID3D11Device> g_pd3dDevice1;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> g_pImmediateContext;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> g_pImmediateContext1;
    Microsoft::WRL::ComPtr<IDXGISwapChain> g_pSwapChain;
    Microsoft::WRL::ComPtr<IDXGISwapChain1> g_pSwapChain1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> g_pRenderTargetView;

    /*--------------------------------------------------------------------
      Forward declarations
    --------------------------------------------------------------------*/
   
    /*F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      Function: WindowProc
      Summary:  Defines the behavior of the window—its appearance, how
                it interacts with the user, and so forth
      Args:     HWND hWnd
                  Handle to the window
                UINT uMsg
                  Message code
                WPARAM wParam
                  Additional data that pertains to the message
                LPARAM lParam
                  Additional data that pertains to the message
      Returns:  LRESULT
                  Integer value that your program returns to Windows
    -----------------------------------------------------------------F-F*/
    //--------------------------------------------------------------------------------------
    // Window Procedure
    //--------------------------------------------------------------------------------------
    
   
    LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc;


        switch (message)
        {
        case WM_CLOSE:
            if (MessageBox(hWnd,
                L"Really quit?",
                L"Game Graphics Programming",
                MB_OKCANCEL) == IDOK)
            {
                DestroyWindow(hWnd);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;


        default: // 알아서 해줄 것
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }

    //--------------------------------------------------------------------------------------
    // Register class, create window and show window
    //--------------------------------------------------------------------------------------
    HRESULT InitWindow(_In_ HINSTANCE hInstance, _In_ INT nCmdShow)
    {
        // Register class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"TutorialWindowClass";
        wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
        if (!RegisterClassEx(&wcex))
            return E_FAIL;

        // Create window
        g_hInst = hInstance;
        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        g_hWnd = CreateWindow(L"TutorialWindowClass", L"Direct3D 11 Tutorial 1: Direct3D 11 Basics",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
            nullptr);
        if (!g_hWnd)
            return E_FAIL;

        ShowWindow(g_hWnd, nCmdShow);

        return S_OK;
    }

   


    //--------------------------------------------------------------------------------------
    // Create Direct3D device and context
    //--------------------------------------------------------------------------------------
    HRESULT InitDevice()
    {
       
        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(g_hWnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            g_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, g_pd3dDevice.GetAddressOf() , &g_featureLevel, g_pImmediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, g_pd3dDevice.GetAddressOf(), &g_featureLevel, g_pImmediateContext.GetAddressOf());
            }

            if (SUCCEEDED(hr))
                break;
        }
        if (FAILED(hr))
            return hr;

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
        {
            Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
            if (SUCCEEDED(g_pd3dDevice.As(&dxgiDevice)))
            {
                Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
                hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());
                if (SUCCEEDED(hr))
                {
                    adapter->GetParent(__uuidof(IDXGIFactory1), (&dxgiFactory));
                   
                }
                
            }
        }
        if (FAILED(hr))
            return hr;

        // Create swap chain
        ComPtr<IDXGIFactory2>           dxgiFactory2(nullptr);
        hr = dxgiFactory.As(&dxgiFactory2);

        if (dxgiFactory2)
        {
            // DirectX 11.1 or later
            
            if (SUCCEEDED(g_pd3dDevice.As(&g_pd3dDevice1)))
            {
               hr= g_pImmediateContext.As(&g_pImmediateContext1);
            }

           
           
            DXGI_SWAP_CHAIN_DESC1 sd = {};
            sd.Width = width;
            sd.Height = height;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;

            hr = dxgiFactory2->CreateSwapChainForHwnd(g_pd3dDevice.Get(), g_hWnd, &sd, nullptr, nullptr, g_pSwapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
               hr= g_pSwapChain1.As(&g_pSwapChain);
            }

            
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd = {};
            sd.BufferCount = 1;
            sd.BufferDesc.Width = width;
            sd.BufferDesc.Height = height;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = g_hWnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;

            hr = dxgiFactory->CreateSwapChain(g_pd3dDevice.Get(), &sd, g_pSwapChain.GetAddressOf());
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);

        

        if (FAILED(hr))
            return hr;

        // Create a render target view
        Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
        hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (&pBackBuffer));
        if (FAILED(hr))
            return hr;

        hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, g_pRenderTargetView.GetAddressOf());
        
        if (FAILED(hr))
            return hr;

        g_pImmediateContext->OMSetRenderTargets(1, g_pRenderTargetView.GetAddressOf(), nullptr);

        // Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        g_pImmediateContext->RSSetViewports(1, &vp);

        return S_OK;


    }



    void Render()
    {
        // Just clear the backbuffer
        g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView.Get(), Colors::MidnightBlue);
        g_pSwapChain->Present(0, 0);
    }

    // 윈도우에서 오는 메시지에 대응하는 함수를 줘 ~~~

    void CleanupDevice()
    {
        if (g_pImmediateContext) g_pImmediateContext->ClearState();
    }
}