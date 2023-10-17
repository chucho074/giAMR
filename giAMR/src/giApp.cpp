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
#include "giAMR.h"
#include "giTexture.h"

using namespace giAMRSDK;
using namespace ImGui;

int32 
App::run() {


  //Create the main window.
  createWindow();

  //Create the information of the application and other systems.
  onCreate();

  

  //Initialize every system.
  initSystems();

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

  ShowWindow(m_hWnd, 10);
  UpdateWindow(m_hWnd);

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
  //
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();  
}

void
App::onRender() {
  //
  ImGui::BeginMainMenuBar(); {
    if (ImGui::BeginMenu("Config")) {
      if (ImGui::MenuItem("Change Project Path")) {
        
      }

      if (ImGui::MenuItem("Change Anaconda Path")) {
        
      }

      if (ImGui::MenuItem("Change Output Path")) {

      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Exit")) {
      
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  //Imgui docking space for windows
  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

  //Render other windows
  

  auto& amr = g_AMR();
  //Render the details window.
  ImGui::Begin("Details", nullptr, ImGuiWindowFlags_NoNav
                                    | ImGuiWindowFlags_NoCollapse
                                    | ImGuiWindowFlags_NoScrollbar); {
    ImGui::BeginTable("giAMR Table", 1, ImGuiTableFlags_ScrollY); {
      ImGui::TableNextColumn();

      ImGui::Text("Generate from a Sphere");
      ImGui::SameLine();

      if (ImGui::Button(toString(amr.minimunTriang).c_str())) {
        amr.finalTriang = amr.minimunTriang;
      }

      ImGui::SameLine();

      if (ImGui::Button(toString(amr.halfTriang).c_str())) {
        amr.finalTriang = amr.halfTriang;
      }

      ImGui::SameLine();

      if (ImGui::Button(toString(amr.partialTriang).c_str())) {
        amr.finalTriang = amr.partialTriang;
      }

      ImGui::SliderInt(" ",
        &amr.finalTriang,
        amr.minimunTriang,
        amr.m_refInfo.totalTriangles);

      ImGui::Separator();

      ImGui::Text("Generate form a base model");

      ImGui::Separator();

      //Ref mesh
      ImGui::Text("Ref Mesh:\t");
      ImGui::SameLine();
      ImGui::Text(amr.m_savedData.m_refMesh.filename().string().c_str());
      ImGui::SameLine();

      //ImGui::Separator();
      ImGui::Text(" | ");
      ImGui::SameLine();

      //Base mesh
      ImGui::Text("Base Mesh:\t");
      ImGui::SameLine();
      ImGui::Text(amr.m_savedData.m_baseMesh.filename().string().c_str());

      ImGui::Separator();

      ImGui::Text("AMR configuration");
      ImGui::Separator();

      //Train resolution
      ImGui::SliderInt("Train resolution", &amr.m_savedData.m_trainResolution, 64, 2048);

      ImGui::Separator();

      //Learning rate
      ImGui::SliderFloat("Learning Rate", &amr.m_savedData.m_learningRate, 0.003, 0.10);

      ImGui::Separator();

      //Iterations
      ImGui::SliderInt("Iterations", &amr.m_savedData.m_iterations, 4000, 25000);

      ImGui::Separator();

      //Batch
      ImGui::SliderInt("Batch", &amr.m_savedData.m_batch, 2, 10);

      ImGui::Separator();

      //Generate Button
      if (ImGui::Button("Generate")) {
        //if (useSphere) {
        //RM.exportModel({ configs.s_generatedPath.string() + "sphere.obj" },
          //createSphere(amr.finalTriang));
        //}
        amr.run();
        amr.m_renderWindow = false;
        amr.m_processWindow = true;
      }

      ImGui::EndTable();
    }
    ImGui::End();
  }





  //
  ImGuiIO& io = ImGui::GetIO();
  ImGui::Render(); 
  m_devContext->OMSetRenderTargets(1, &m_backBuffer, NULL);
  float clear_color_with_alpha[4] = { 0.45f * 1.00f, 0.55f * 1.00f, 0.60f * 1.00f, 1.00f };
  m_devContext->ClearRenderTargetView(m_backBuffer, clear_color_with_alpha);
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
  
  return 0;
}

void 
App::initSystems() {
  //AMR
  AMR::startUp();
  AMR* amr = new AMR();
  g_AMR().setObject(amr);
  
  //Configs
  Configs::startUp();
  Configs* conf = new Configs();
  g_Configs().setObject(conf);

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

void 
App::renderAMRprocess() {
  
  auto& amr = g_AMR();
  auto& configs = g_Configs();

  amr.m_AMRprocess.reserve(25);


  

  bool* tmpValue = &amr.m_processWindow;
  ImGui::Begin("giAMRprocess", tmpValue, ImGuiWindowFlags_NoScrollbar
                                         | ImGuiWindowFlags_NoDocking
                                         | ImGuiWindowFlags_NoCollapse);
  if(ImGui::BeginTable("giAMRProcessTable", 1, ImGuiTableFlags_ScrollY)) { //1
    ImGui::TableNextColumn();

    //Read every new image, if exist any new, load it and present it.
    SharedPtr<Texture> tmpTexture;
    String tmpImgName = ("/img" + toString(amr.m_processImg) + ".png");
    String tmpNextImgName = ("/img" + toString(amr.m_processImg+1) + ".png");
    //The path for the actual image
    Path tmpImgPath = configs.s_outPath.string() + "/giAMR/"
                       + amr.m_savedData.m_refMesh.filename().string() + tmpImgName;
    //Path for the next image
    Path tmpNextImgPath = configs.s_outPath.string() + "/giAMR/"
                       + amr.m_savedData.m_refMesh.filename().string() + tmpNextImgName;
    //Path 
    Path tmpPath = amr.m_savedData.m_outputDir.string() + "/mesh/mesh.obj";
    if (fsys::exists(tmpNextImgPath)) {
      Path tmpFile(tmpImgPath);
      amr.m_AMRprocess.push_back(textureFromPath(tmpFile));
      ++amr.m_processImg;
      amr.m_showingImg = amr.m_processImg;
    }

    if(!amr.m_AMRprocess.empty()) {
      if (nullptr != amr.m_AMRprocess.at(amr.m_showingImg-1).m_texture) {
        //tmpTexture = static_pointer_cast<Texture>(amr.m_AMRprocess.at(amr.m_showingImg-1));
        tmpTexture = make_shared<Texture>(amr.m_AMRprocess.at(amr.m_showingImg-1));
        if(nullptr != tmpTexture->m_texture) {
          ImGui::Image(tmpTexture->m_texture, { ImGui::GetWindowContentRegionMax().x / 1,
                                                ImGui::GetWindowContentRegionMax().y / 2 });
        }
      }
    }
    ImGui::SliderInt("", &amr.m_showingImg, 1, amr.m_processImg);
    ImGui::Text(String("Image: "+ toString(amr.m_showingImg)).c_str());
    ImGui::Separator();

    if(ImGui::BeginTable("ComparasionTable", 2, ImGuiTableFlags_ScrollY)) {//2
      ImGui::TableNextColumn();
      
      if (0 == amr.m_outInfo.totalTriangles) {
        if (fsys::exists(tmpPath.string()+ ".giData")) {
          Path tmpOutFile(tmpPath.string() + ".giData");
          //amr.m_outInfo = RM.getFromFile(tmpOutFile);
        }
      }

      ImGui::Text("Output Model: ");
      ImGui::Text(String("Triangles: " + toString(amr.m_outInfo.totalTriangles)).c_str());
      ImGui::Text(String("Vertex: " + toString(amr.m_outInfo.totalVertices)).c_str());
      ImGui::Text(String("Index: " + toString(amr.m_outInfo.totalIndex)).c_str());

      ImGui::TableNextColumn();

      ImGui::Text("Reference Model: ");
      ImGui::Text(String("Triangles: " + toString(amr.m_refInfo.totalTriangles)).c_str());
      ImGui::Text(String("Vertex: " + toString(amr.m_refInfo.totalVertices)).c_str());
      ImGui::Text(String("Index: " + toString(amr.m_refInfo.totalIndex)).c_str());

      ImGui::EndTable();//2

    }
    ImGui::EndTable();//1
  }
  ImGui::End();
}

Texture
App::textureFromPath(Path inPath) {
  return Texture();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT 
App::handleWindowEvent(HWND inHw, UINT inMsg, WPARAM inwParam, LPARAM inlParam) {

  if (ImGui_ImplWin32_WndProcHandler(inHw, inMsg, inwParam, inlParam))
    return true;

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