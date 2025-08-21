// Public engine api.
// To use/compile define the respective macros:
// TE_DX11
// TE_WINDOWING
// TE_MATH etc.
// TODO add better documentation here

#include <wincodec.h>
#include <map>
#include <comdef.h>
#include <cassert>
#include <ppltasks.h>
#include <vector>
#include <iostream>
#include <string>
#include <d3d11_1.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

/// Represents a 3D API abstraction, 
/// eg. for OpenGL, DirectX, Vulkan etc.
namespace tiny_engine {

    struct Texture 
    {
        uint32_t id;
    };

    struct Event 
    {
        std::string type;

    };

    struct Context3D 
    {
        std::string api;
    };

  
    /// Cross platform window. 
    /// Where you render your game into
    struct Window 
    {
        int width;
        int height;
        void* nativeWindowHandle;   // e.g. HWND on win32, X11 window on Linux etc.

    };


    /// Creates a window with the client area having the desired dimension.
    Window createWindow(int width, int height, bool fullscreen);

    /// Poll the window for messages. 
    /// Necessary to have the window responsible, react to mouse movement, clicks,
    /// is closable etc.
    /// TODO return a list of frame events, including input events etc.
    std::vector<Event> pollWindowMessages(Window window);


    /// api: 
    /// dx11
    /// dx12
    /// vulkan
    /// opengl
    Context3D* init3D(const std::string& api, Window window);


    /// Clears the backbuffer to the given RGB color, 
    /// and the depthbuffer to 1.
    void clearBackBuffer(Context3D* context, float r, float g, float b, float a);


    /// 
    /// Flips the backbuffer, presenting its contents to the main window.
    void presentBackBuffer(Context3D* context);



    /// Binds the default (window) backbuffer. 
    /// Must be done before any clear or draw operation.
    void bindBackBuffer(Context3D* context, int x, int y, int width, int height);

    Texture createTextureFromFile(Context3D* context, const std::string& fileName);



    /// This is the private part of the API, 
    /// should only be used internally. 
    /// Call at your own risk, but better do not call at all!
    namespace detail {


        



        namespace dx11 {
            bool init(Window window);
            void printDXGIError(HRESULT hr);
            bool resizeSwapChain(HWND hwnd, int width, int height);
            bool createDefaultDepthStencilBuffer(int width, int height);
            void setViewport(int originX, int originY, int width, int height);
            void clearBackBuffer(float r, float g, float b, float a);
            void presentBackBuffer();
            void bindBackBuffer(int x, int y, int width, int height);
            void createTextureFromFile(const std::string& fileName);


            ComPtr<ID3D11Device> dx11Device;
            ComPtr<ID3D11DeviceContext> dx11Context;
            ComPtr<ID3D11Debug> dx11Debug;
            ComPtr<IDXGISwapChain> dx11SwapChain;
            ComPtr<ID3D11Texture2D> dx11BackBuffer;
            ComPtr<ID3D11Texture2D> dx11DepthStencilBuffer;
            ComPtr<ID3D11DepthStencilView> dx11DepthStencilView;
            ComPtr<ID3D11RenderTargetView> dx11rtv;
            ComPtr<ID3D11DepthStencilState> dx11DepthStencilState;
            ComPtr<ID3D11RasterizerState> dx11RasterState;
            ComPtr<ID3D11SamplerState> dx11SamplerState;
            ComPtr<ID3D11BlendState> dx11BlendState;


            struct DX11Texture {
                ComPtr<ID3D11Texture2D> texture;
                ComPtr<ID3D11RenderTargetView> rtv;
            };


            std::map<uint32_t, DX11Texture> textureStorage;

        }

        
    }

}




tiny_engine::Context3D* tiny_engine::init3D(const std::string& api, tiny_engine::Window window) 
{

    namespace ted = tiny_engine::detail;
    if (api != "dx11") return nullptr;

    if (tiny_engine::detail::dx11::init(window)) 
    {

        auto dx11Context = new tiny_engine::Context3D();
        dx11Context->api = api;
        return dx11Context;
    }
    else 
    {

        return nullptr;
    }


}


// Private API, not callable from outside
    

void tiny_engine::clearBackBuffer(Context3D* context, float r, float g, float b, float a)
{
   if (context->api == "dx11") {
       tiny_engine::detail::dx11::clearBackBuffer(r, g, b, a);
   }
}

void tiny_engine::presentBackBuffer(Context3D* context)
{
    if (context->api == "dx11") {
        tiny_engine::detail::dx11::presentBackBuffer();
    }
}

void tiny_engine::bindBackBuffer(Context3D* context, int x, int y, int width, int height) 
{
    if (context->api == "dx11") {
        tiny_engine::detail::dx11::bindBackBuffer(x, y, width, height);
    }

}



#ifdef TE_DX11
void tiny_engine::detail::dx11::presentBackBuffer()
{
    auto result = dx11SwapChain->Present(0, 0);
    if (FAILED(result)) {
        _com_error err(result);
        std::wcerr << L"Present failed: " << _com_error(result).ErrorMessage() << std::endl;
        auto reason = dx11Device->GetDeviceRemovedReason();
        std::cerr << "Device removed reason: " << reason << std::endl;
        // TODO further error handling
    }
}

void tiny_engine::detail::dx11::printDXGIError(HRESULT hr) {
    LPWSTR errorText = nullptr;
    DWORD result = FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&errorText), 0, nullptr );
    if (result > 0)
    {
        // errorText contains the description of the error code hr
        std::wcout << "DXGI Error: " << errorText << std::endl;
        LocalFree( errorText );
    }
    else
    {
        // Error not known by the OS
    }
}


tiny_engine::detail::dx11::DX11Texture createTextureFromFile(const std::wstring& fileName)
{

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    Microsoft::WRL::ComPtr<IWICImagingFactory> wic;
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(&wic));

    Microsoft::WRL::ComPtr<IWICBitmapDecoder> dec;
    if (FAILED(wic->CreateDecoderFromFilename(fileName.c_str(), nullptr, GENERIC_READ,
                                              WICDecodeMetadataCacheOnDemand, &dec)))
    return {};

    ComPtr<IWICBitmapFrameDecode> frame;
    dec->GetFrame(0, &frame);

    // Convert to straight RGBA (not pre-multiplied)
    Microsoft::WRL::ComPtr<IWICFormatConverter> conv;
    wic->CreateFormatConverter(&conv);
    conv->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA,
                     WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);

    UINT w = 0, h = 0;
    conv->GetSize(&w, &h);
    std::vector<uint8_t> buffer(w * h * 4);
    conv->CopyPixels(nullptr, w * 4, (UINT)buffer.size(), buffer.data());
}
    

void tiny_engine::detail::dx11::clearBackBuffer(float r, float g, float b, float a) 
{

    float color[] = {r, g, b, a};
    dx11Context->ClearRenderTargetView(dx11rtv.Get(), color);
    dx11Context->ClearDepthStencilView(dx11DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

bool tiny_engine::detail::dx11::init(Window window)
{
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
    UINT flags = 0;

    #ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    auto result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 
                    flags, &featureLevel, 1, D3D11_SDK_VERSION, &dx11Device, nullptr, &dx11Context);

    if (FAILED(result)) return false;


    // Gather the debug interface:
    #ifdef _DEBUG
    if (flags == D3D11_CREATE_DEVICE_DEBUG) {
        result = dx11Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&dx11Debug);
        if (FAILED(result)) {
            OutputDebugString(L"debuger creation failed\n");
            return false;
        }
    }
    #endif

    // Creating our swapchain:
    IDXGIDevice * dxgiDevice = nullptr;
    result = dx11Device->QueryInterface(__uuidof(IDXGIDevice), 
        (void **)&dxgiDevice);

    if (FAILED(result)) {
        return false;
    }
    

    IDXGIAdapter * dxgiAdapter = nullptr;
    result = dxgiDevice->GetAdapter(&dxgiAdapter);
    if (FAILED(result)) {
        return false;
    }

    IDXGIOutput* output = nullptr;
    result = dxgiAdapter->EnumOutputs(0, &output);
    if (SUCCEEDED(result)) {
        UINT numModes = 0;
        // First, get the number of modes
        result = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0, &numModes, nullptr);
        if (SUCCEEDED(result) && numModes > 0) {
            std::vector<DXGI_MODE_DESC> modeList(numModes);
            // Retrieve the full list
            result = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0, &numModes, modeList.data());
            if (SUCCEEDED(result)) {
                std::cout << "Supported _SRGB modes found: " << numModes << std::endl;

                bool modeFound = false;
                for (const auto& mode : modeList) {

                    // Check for exact or close match
                    if (mode.Width == window.width && mode.Height == window.height &&
                        mode.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB &&
                        mode.RefreshRate.Numerator / (float)mode.RefreshRate.Denominator >= 59.0f &&
                        mode.RefreshRate.Numerator / (float)mode.RefreshRate.Denominator <= 61.0f) {
                        modeFound = true;
                        std::cout << "Matching mode found!" << std::endl;
                        std::cout << "Width: " << mode.Width << ", Height: " << mode.Height
                              << ", Refresh: " << mode.RefreshRate.Numerator << "/"
                              << mode.RefreshRate.Denominator << std::endl;
                        break;
                        }
                }
                if (!modeFound) {
                    std::cerr << "No exact match for " << window.width << "x" << window.height << " at ~60 Hz" << std::endl;
                }
            } else {
                std::cerr << "GetDisplayModeList failed: 0x" << std::hex << result << std::endl;
            }
        } else {
            std::cerr << "Format not supported or no modes: 0x" << std::hex << result << std::endl;
        }
        output->Release();
    }

    IDXGIFactory* factory = nullptr;
    result = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result)) {
        return false;
    }

    HWND hwnd = (HWND) window.nativeWindowHandle;

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Width  = window.width;
    sd.BufferDesc.Height = window.height;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.SampleDesc.Count   = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount  = 1;
    sd.OutputWindow = hwnd;
    sd.Windowed     = true;
    sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags        = 0;
    result = factory->CreateSwapChain(dx11Device.Get(), &sd, &dx11SwapChain );
    if (FAILED(result)) {
        printDXGIError(result);
        return false;
    }

    factory->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();

    resizeSwapChain(hwnd, window.width, window.height);

    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = FALSE; 
    rsDesc.DepthClipEnable = TRUE;
    dx11Device->CreateRasterizerState(&rsDesc, &dx11RasterState);
    dx11Context->RSSetState(dx11RasterState.Get());

    D3D11_SAMPLER_DESC samplerdesc;
    ZeroMemory(&sd, sizeof(sd));
    samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerdesc.MinLOD = 0;
    samplerdesc.MaxLOD = D3D11_FLOAT32_MAX;
    result = dx11Device->CreateSamplerState(&samplerdesc, &dx11SamplerState);
    if (FAILED(result)) {
        return false;
    }


    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable           = TRUE;
    blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = dx11Device->CreateBlendState(&blendDesc, &dx11BlendState);
    if (FAILED(result)) {
        return false;
    }
    float blendFactor[4] = { 0, 0, 0, 0 };
    dx11Context->OMSetBlendState(dx11BlendState.Get(), blendFactor, 0xffffffff);


    bindBackBuffer(0, 0, window.width, window.height);

    return true;

}


#endif




#ifdef TE_WINDOWING
#include <Windows.h>


static LRESULT CALLBACK engineWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
    switch (msg) 
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    

}


bool tiny_engine::detail::dx11::resizeSwapChain(HWND hwnd, int width, int height) {
    dx11Context->OMSetRenderTargets(0, nullptr, nullptr);
    ID3D11ShaderResourceView* nullsrvs[16] = {nullptr};
    dx11Context->VSSetShaderResources(0, 16, nullsrvs);
    dx11Context->PSSetShaderResources(0, 16, nullsrvs);
    dx11Context->CSSetShaderResources(0, 16, nullsrvs);
    ID3D11UnorderedAccessView* nullUAVs[8] = { nullptr };
    dx11Context->CSSetUnorderedAccessViews(0, 8, nullUAVs, nullptr);

    if (dx11rtv.Get()) {
        dx11rtv->Release();
        dx11rtv = nullptr;
    }
    if (dx11DepthStencilView.Get()) {
        dx11DepthStencilView->Release();
        dx11DepthStencilView = nullptr;
    }

    if (dx11DepthStencilBuffer.Get()) {
        dx11DepthStencilBuffer->Release();
        dx11DepthStencilBuffer = nullptr;
    }

    auto result = dx11SwapChain->Present(0, 0);
    if (FAILED(result)) {
        return false;
    }

    DXGI_SWAP_CHAIN_DESC desc;

    dx11SwapChain->GetDesc(&desc);
    std::cout << "Swap chain state: BufferCount=" << desc.BufferCount
              << ", Format=" << desc.BufferDesc.Format
              << ", Windowed=" << (desc.Windowed ? "Yes" : "No")
              << ", Flags=" << desc.Flags << std::endl;

    DXGI_MODE_DESC modeDesc = {};
    modeDesc.Width = width;
    modeDesc.Height = height;
    modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    result = dx11SwapChain->ResizeTarget(&modeDesc);
    if (FAILED(result)) {
        std::cout << "backbuffer target resizing failed" << std::to_string(result) << std::endl;
        return false;
    }

    result = dx11SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(result)) {
        std::cout << "backbuffer resizing on swapchain resizing failed" << std::to_string(result) << std::endl;
        return false;
    }

    result = dx11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&dx11BackBuffer);
    if (FAILED(result)) {
        std::cout << "backbuffer creation/retrieval on swapchain resizing failed" << std::endl;
        return false;
    }

    // Step 4: Create new render target view
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    result = dx11Device->CreateRenderTargetView(dx11BackBuffer.Get(), nullptr, &dx11rtv);
    dx11BackBuffer->Release(); // Release temporary back buffer pointer
    if (FAILED(result)) {
        std::cout << "Render target view creation failed" << std::endl;
        return false;
    }

    if (!dx11rtv) return false;

    createDefaultDepthStencilBuffer(width, height);
    setViewport(0, 0, width, height);

    return true;
}



void tiny_engine::detail::dx11::setViewport(int originX, int originY, int width, int height) {
    D3D11_VIEWPORT vp = {};
    vp.TopLeftX = originX;
    vp.TopLeftY = originY;
    vp.Width = (FLOAT) width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    dx11Context->RSSetViewports(1, &vp);
}


bool tiny_engine::detail::dx11::createDefaultDepthStencilBuffer(int width, int height) {

    // Create a depth/stencil buffer
    D3D11_TEXTURE2D_DESC td;
    td.Width = width;
    td.Height = height;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_D32_FLOAT;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    auto result = dx11Device->CreateTexture2D(&td, 0, &dx11DepthStencilBuffer);
    if (FAILED(result)) {
        std::cout << "DepthStencil buffer creation failed! width: " << std::to_string(width) 
                  << " height: " << std::to_string(height) << std::endl;
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dpd;
    ZeroMemory(&dpd, sizeof(dpd));
    dpd.Flags = 0;
    dpd.Format = DXGI_FORMAT_D32_FLOAT;
    dpd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    result = dx11Device->CreateDepthStencilView(dx11DepthStencilBuffer.Get(), &dpd, &dx11DepthStencilView);
    if (FAILED(result)) {
        OutputDebugString(L"D S view creation failed\n");
        return false;
    }

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = dx11Device->CreateDepthStencilState(&depthStencilDesc, &dx11DepthStencilState);
    if (FAILED(result)) {
        OutputDebugString(L"failed to create depth stencil state\n");
        return false;
    }

    dx11Context->OMSetDepthStencilState(dx11DepthStencilState.Get(), 0);

    return true;

}


void tiny_engine::detail::dx11::bindBackBuffer(int x, int y, int width, int height) {
    ID3D11RenderTargetView* const rtvs[] = { dx11rtv.Get() };
    dx11Context->OMSetRenderTargets(1, rtvs, dx11DepthStencilView.Get());
    setViewport(x, y, width, height);
}


std::vector<tiny_engine::Event> tiny_engine::pollWindowMessages(Window window)
{
    std::vector<Event> events;
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {

            events.emplace_back(tiny_engine::Event{"quit"});
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }


    return events;
}



tiny_engine::Window tiny_engine::createWindow(int width, int height, bool fullscreen)
{

    const std::wstring className = L"GameWindowClass";
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = engineWindowProc; 
    wc.hInstance = hInstance;
    wc.lpszClassName = className.c_str();
    wc.hCursor = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassExW(&wc);

    RECT clientRect = {0, 0, width, height};
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&clientRect, style, FALSE); // FALSE = no menu bar
    int adjustedWidth = clientRect.right - clientRect.left;
    int adjustedHeight = clientRect.bottom - clientRect.top;

    HWND hwnd = CreateWindowExW(
        0,
        className.c_str(),
        L"GameWindow",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, adjustedWidth, adjustedHeight,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
	
	
    if (hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }
#ifdef GAME_MAIN
    extern void GAME_MAIN();
    GAME_MAIN();
#endif
    
    return {width, height, hwnd};
}



#endif
