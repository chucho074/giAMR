/**
 * @file    giApp.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    09/05/2023
 * @brief   A basic aplication.
 */
 
/**
 * @include
 */

//#include <imgui_impl_dx11.cpp>
//#include <imgui_impl_win32.cpp>

#include "giApp.h"
#include "giConfigs.h"

using namespace ImGui;

int32 
App::run() {
  //Create the main window.
  createWindow();

  //Initialize every system.
  initSystems();

  //Create the information of the application and other systems.
  onCreate();

  //App Loop.
  MSG msg = { 0 };
  while (WM_QUIT != msg.message) {
    if (PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      //Own events.
      onEvent();
    }
    else {
      //Update Time
      static float t = 0.0f;
      static int64 dwTimeStart = 0;
      int64 dwTimeCur = GetTickCount64();
      if (dwTimeStart == 0) {
        dwTimeStart = dwTimeCur;
      }
      t = (dwTimeCur - dwTimeStart) / 1000.0f;
      //Update Game Logic.
      onUpdate(t);

      //Render Frame
      onRender();
    }
  }


  //Destroy the resources
  onDestroy();

  return 0;
}

int32 
App::onCreate() {
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = m_hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
#ifdef _DEBUG
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, 
                                                   D3D_FEATURE_LEVEL_10_0};
  if (D3D11CreateDeviceAndSwapChain(NULL, 
                                    D3D_DRIVER_TYPE_HARDWARE, 
                                    NULL, 
                                    createDeviceFlags, 
                                    featureLevelArray, 
                                    2, 
                                    D3D11_SDK_VERSION, 
                                    &sd, 
                                    &m_swapChain,
                                    &m_device,
                                    &featureLevel, 
                                    &m_devContext) != S_OK) {
    __debugbreak();
    return 4;
  }



  //CreateRenderTarget
  ID3D11Texture2D* pBackBuffer;
  m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  m_device->CreateRenderTargetView(pBackBuffer, NULL, &m_backBuffer);
  pBackBuffer->Release();



  //IMGUI

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.DisplaySize = ImVec2(m_width,m_height);
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init(m_hWnd);
  ImGui_ImplDX11_Init(m_device, m_devContext);

  //Change font path to engine settings
  io.Fonts->AddFontFromFileTTF("Resources/Fonts/Inter-Regular.ttf", 15.0f);


  return 0;
}

void 
App::onDestroy() {
  if (m_backBuffer) {
    m_backBuffer->Release();
    m_backBuffer = NULL;
  }

  if (m_swapChain) {
    m_swapChain->Release();
    m_swapChain = NULL;
  }
  if (m_devContext) {
    m_devContext->Release();
    m_devContext = NULL;
  }

  if (m_device) {
    m_device->Release();
    m_device = NULL;
  }
}

void
App::onUpdate(float inDT) {

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
}

void
App::onRender() {
 


  ImGuiIO& io = ImGui::GetIO();

  ImGui::ShowDemoWindow();
  
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  // Update and Render additional Platform Windows
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
  }
  m_swapChain->Present(0, 0);
}

void 
App::onEvent() {
  
}

int32 
App::createWindow() {
  //Create the instance for the window
  auto hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(nullptr));
  
  // Register class
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = handleWindowEvent;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = L"giAMR";
  wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_APPLICATION);
  if (!RegisterClassEx(&wcex)) {
    __debugbreak();
    return 1;
  }

  RECT rc = { 0, 0, m_width, m_height };
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

  m_hWnd = CreateWindow(L"giAMR",
                     L"giAMR app",
                     WS_OVERLAPPEDWINDOW,
                     CW_USEDEFAULT,
                     CW_USEDEFAULT,
                     rc.right - rc.left,
                     rc.bottom - rc.top,
                     nullptr,
                     nullptr,
                     hInstance,
                     nullptr);

  //Check if the window was created correctly
  if (!m_hWnd) {
    __debugbreak();
    return 2;
  }

  RECT clientRect;
  GetClientRect(m_hWnd, &clientRect);
  m_width = clientRect.right - clientRect.left;
  m_height = clientRect.bottom - clientRect.top;
  m_window = reinterpret_cast<void*>(m_hWnd);
  ShowWindow(m_hWnd, 10);

  return 0;
}

void 
App::initSystems() {
  checkoutSetup();
  if(!readConfigs()) {
    setConfigs();
  }
}

void 
App::destroySystems() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

bool 
App::readConfigs() {
  
  return true;
}

void 
App::setConfigs() {
  
}

void 
App::checkoutSetup() {
  
}

LRESULT 
App::handleWindowEvent(HWND inHw, UINT inMsg, WPARAM inwParam, LPARAM inlParam) {

  PAINTSTRUCT ps;
  HDC hdc;
  switch (inMsg) {
  case WM_PAINT: {
    hdc = BeginPaint(inHw, &ps);
    EndPaint(inHw, &ps);
    break;
  }
  case WM_DESTROY: {
    PostQuitMessage(0);
    break;
  }


  default: {

    return DefWindowProc(inHw, inMsg, inwParam, inlParam);
    break;
  }
  }


}