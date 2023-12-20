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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <pmp/surface_mesh.h>
#include <pmp/algorithms/subdivision.h>
#include <pmp/io/write_giAMR.h>

#include "giApp.h"

#include "giConfigs.h"
#include "giAMR.h"
#include "giTexture.h"
#include "giExporter.h"

#include <cmath>
#include <shlobj_core.h>
#include <json.hpp>
#include <json_fwd.hpp>
#include <yaml-cpp/yaml.h>

#include <d3d11.h>

using namespace giAMRSDK;
using namespace ImGui;
using namespace pmp;

namespace giAMRSDK {

#define MIN_SPHERE_SECTOR 3
#define MIN_SPHERE_STACK 2

#define PI static_cast<float>(4.0f * std::atan(1.0f))

  int32
  App::run() {
    auto & configs = g_Configs();

    //Create the main window.
    createWindow();

    //Create the information of the application and other systems.
    onCreate();

    //Initialize every system.
    initSystems();

    //Verify the existance of the configs.
    if(exists("giAMRconfigs.json")) {
      readConfigs();
      if(configs.s_anacondaPath.empty()) {
        m_askAnaconda = true;
      }
    }
    else {
      //Create the configs file and ask for anaconda's path.
      setConfigs();
      m_askAnaconda = true;
    }

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
                                                     D3D_FEATURE_LEVEL_10_0 };
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
    io.DisplaySize = ImVec2(m_width, m_height);
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
    UNREFERENCED_PARAMETER(inDT);
    //
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
  }

  void
  App::onRender() {

    auto& amr = g_AMR();
    auto& configs = g_Configs();

    //Menu bar
    ImGui::BeginMainMenuBar(); {
      if (ImGui::BeginMenu("Config")) {
        if (ImGui::MenuItem("Change Anaconda Path")) {
          configs.s_anacondaPath = selectFolderDialog();
          writeConfigs();
        }

        if (ImGui::MenuItem("Re-create configs")) {
          setConfigs();
        }
        ImGui::EndMenu();
      }

      ImGui::EndMainMenuBar();
    }

    //Imgui docking space for windows.
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    //Render other windows.
    
    
    //Render AMR menu.
    ImGui::Begin("Details", nullptr, ImGuiWindowFlags_NoNav
                                     | ImGuiWindowFlags_NoCollapse
                                     | ImGuiWindowFlags_NoScrollbar); {
      ImGui::BeginTable("giAMR Table", 1, ImGuiTableFlags_ScrollY); {
        ImGui::TableNextColumn();

        ImGui::Text("Generate from a UV Sphere");
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

        //Number of triangles
        ImGui::SliderInt("Triangles",
                         &amr.finalTriang,
                         amr.minimunTriang,
                         amr.m_refInfo.totalTriangles);

        ImGui::Separator();

        ImGui::Text("Generate from a Quad Sphere");

        //Number of subdivisions
        ImGui::SliderInt("Subdivisions",
                         &m_subdiv,
                         1,
                         6);

        ImGui::Text("Total Triangles: ");

        ImGui::SameLine();

        ImGui::Text(toString(pow(4, m_subdiv + 1) * 6 / 2).c_str());

        ImGui::Separator();

        //Ref mesh
        ImGui::Text("Ref Mesh:\t");
        ImGui::SameLine();
        ImGui::Text(amr.m_savedData.m_refMesh.filename().string().c_str());
        ImGui::SameLine();
        if (ImGui::Button("Select", { 50, 20 })) {
          amr.setRefMesh(openFileDialog());
          //Verify if is a fbx and transform to obj.
          if (L".fbx" == amr.m_savedData.m_refMesh.extension()) {
            Path tmpNewPath = Exporter::ExportAsObj(amr.m_savedData.m_refMesh, "obj");
            amr.setRefMesh(tmpNewPath);
          }
          createData(amr.m_savedData.m_refMesh);
          //Create data
          Path tmpPathData = amr.m_savedData.m_refMesh.string() + ".giData";
          amr.m_refInfo = decodeData(tmpPathData);
          
          if (amr.minimunTriang < amr.m_refInfo.totalTriangles) {
            amr.minimunTriang = amr.m_refInfo.totalTriangles * 0.25f;

            amr.halfTriang = amr.m_refInfo.totalTriangles * 0.50f;

            amr.partialTriang = amr.m_refInfo.totalTriangles * 0.75f;

            amr.finalTriang = amr.minimunTriang;
          }
        }
        ImGui::SameLine();

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

        ImGui::Text("Generation process");

        ImGui::Text("The sphere output path is: ");
        
        ImGui::Text(configs.s_generatedPath.string().c_str());
        

        //Generate by uv sphere
        if (ImGui::Button("Generate (UV Sphere)")) {
          if(!m_askAnaconda 
             && !configs.s_generatedPath.empty() 
             && !amr.m_savedData.m_refMesh.empty()) {
            createSphere(amr.finalTriang);

            amr.run();
            amr.m_processWindow = true;
          }
        }

        ImGui::SameLine();

        //Generate by quad sphere
        if (ImGui::Button("Generate (Quad Sphere)")) {
          if(!m_askAnaconda 
             && !configs.s_generatedPath.empty() 
             && !amr.m_savedData.m_refMesh.empty()) {
            createQuadSphere(m_subdiv);

            amr.run();
            amr.m_processWindow = true;
          
          }
        }

        ImGui::EndTable();
      }

      //Modal for asking about the anaconda path.
      if(m_askAnaconda) { 
        ImGui::OpenPopup("Anaconda Selection");
      }
      if(m_askAnaconda) { 
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize({ 450, 100 });
        bool* tmpValue = &m_askAnaconda;
        if (ImGui::BeginPopupModal("Anaconda Selection", tmpValue, ImGuiWindowFlags_AlwaysAutoResize
                                                              | ImGuiWindowFlags_NoMove)) { //0
          ImGui::Text("Please, select the folder of the anaconda files");
          ImGui::Text("The correct path to search is: ../anaconda3/Scripts/");
          if(ImGui::Button("Select")) {
            configs.s_anacondaPath = selectFolderDialog();
            configs.s_anacondaPath = configs.s_anacondaPath.string() + "/";
            configs.s_anacondaPath.make_preferred();
            ImGui::CloseCurrentPopup();
            m_askAnaconda = false;
            writeConfigs();
          }
          ImGui::EndPopup();
      
        }
      }

      //Modal for the process window
      if (amr.m_processWindow) {
        ImGui::OpenPopup("giAMRprocess");
      }
      renderAMRprocess();



      ImGui::End();
    }


    //
    ImGuiIO& io = ImGui::GetIO();
    m_devContext->OMSetRenderTargets(1, &m_backBuffer, NULL);

    float clear_color_with_alpha[4] = { 0.45f * 1.00f, 0.55f * 1.00f, 0.60f * 1.00f, 1.00f };
    m_devContext->ClearRenderTargetView(m_backBuffer, clear_color_with_alpha);

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

  }

  void
  App::destroySystems() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
  }

  bool
  App::readConfigs() {
    auto& configs = g_Configs();

    ifstream configFile("giAMRconfigs.json");
    
    json data = json::parse(configFile);
    configs.s_projectPath = data.value("/projectPath"_json_pointer, "/");
    configs.s_anacondaPath = data.value("/anacondaPath"_json_pointer, "/");
    configs.s_nvdiffmodPath = data.value("/nvdiffmodPath"_json_pointer, "/");
    configs.s_binPath = data.value("/binPath"_json_pointer, "/");
    configs.s_generatedPath = data.value("/generatedPath"_json_pointer, "/");

    return true;
  }

  void
  App::setConfigs() {
    auto& configs = g_Configs();

    configs.s_projectPath = current_path().parent_path();
    configs.s_projectPath = configs.s_projectPath.string() + "/";
    configs.s_binPath = configs.s_projectPath.string() + "/bin/";
    configs.s_nvdiffmodPath = configs.s_projectPath.string() + "/nvdiffmodeling/";
    configs.s_generatedPath = configs.s_binPath.string() + "/Resources/Generated/";
    writeConfigs();
  }

  void 
  App::writeConfigs() {
    auto & configs = g_Configs();
    json tmpFile;

    tmpFile["projectPath"] = configs.s_projectPath.make_preferred();
    tmpFile["anacondaPath"] = configs.s_anacondaPath.make_preferred();
    tmpFile["nvdiffmodPath"] = configs.s_nvdiffmodPath.make_preferred();
    tmpFile["binPath"] = configs.s_binPath.make_preferred();
    tmpFile["generatedPath"] = configs.s_generatedPath.make_preferred();

    ofstream o("giAMRconfigs.json");
    o << std::setw(4) << tmpFile << ConsoleLine;
  }

  void
  App::checkoutSetup() {

  }

  void
  App::renderAMRprocess() {

    auto& amr = g_AMR();

    amr.m_AMRprocess.reserve(25);

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize({450, 450});
    bool* tmpValue = &amr.m_processWindow;
    if(ImGui::BeginPopupModal("giAMRprocess", tmpValue, ImGuiWindowFlags_AlwaysAutoResize 
                                                        | ImGuiWindowFlags_NoMove)) { //0
      if (ImGui::BeginTable("giAMRProcessTable", 1, ImGuiTableFlags_Resizable)) { //1
        ImGui::TableNextColumn();

        //Read every new image, if exist any new, load it and present it.
        SharedPtr<Texture> tmpTexture;
        String tmpImgName = ("/img" + toString(amr.m_processImg) + ".png");
        String tmpNextImgName = ("/img" + toString(amr.m_processImg + 1) + ".png");
        //The path for the actual image
        //String tmpOut = amr.m_savedData.m_outputDir.parent_path().string() + "/" + amr.m_savedData.m_outputDir.stem().string();
        String tmpOut = amr.m_savedData.m_outputDir.string();
        Path tmpImgPath = tmpOut + tmpImgName;
        //Path for the next image
        Path tmpNextImgPath = tmpOut + tmpNextImgName;
        //Path 
        Path tmpPath = tmpOut + "/mesh/mesh.obj";
        if (fsys::exists(tmpNextImgPath)) {
          amr.m_AMRprocess.push_back(textureFromPath(tmpImgPath));
          ++amr.m_processImg;
          amr.m_showingImg = amr.m_processImg;
        }

        if (!amr.m_AMRprocess.empty()) {
          if (nullptr != amr.m_AMRprocess.at(amr.m_showingImg - 1)->m_texture) {
            tmpTexture = static_pointer_cast<Texture>(amr.m_AMRprocess.at(amr.m_showingImg-1));
            //tmpTexture = make_shared<Texture>(amr.m_AMRprocess.at(amr.m_showingImg - 1));
            if (nullptr != tmpTexture->m_texture) {
              ImGui::Image(tmpTexture->m_subResourceData, { ImGui::GetWindowContentRegionMax().x / 1,
                                                    ImGui::GetWindowContentRegionMax().y / 2 });
            }
          }
        }
        ImGui::SliderInt("", &amr.m_showingImg, 1, amr.m_processImg);
        ImGui::Text(String("Image: " + toString(amr.m_showingImg)).c_str());
        ImGui::Separator();
        
        if (ImGui::BeginTable("ComparasionTable", 2, ImGuiTableFlags_None)) {//2
          ImGui::TableNextColumn();
        
          if (0 == amr.m_outInfo.totalTriangles) {
            //Verify if the material exist, cause is created after the model file.
            Path tmpMaterialPath = tmpPath;
            tmpMaterialPath.replace_extension(".mtl");
            if(fsys::exists(tmpMaterialPath.string())) {
              if (!fsys::exists(tmpPath.string() + ".giData")) {
                createData(tmpPath);
              }
              else {
                Path tmpOutFile(tmpPath.string() + ".giData");
                amr.m_outInfo = decodeData(tmpOutFile);
              }
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
        ImGui::EndTable();
      }//1
      ImGui::EndPopup();
    }//0
  }

  void 
  App::createData(Path inFile) {
    
    YAML::Emitter tmpOut;

    tmpOut << YAML::BeginMap;
    tmpOut << YAML::Key << "Model data" << YAML::Value << YAML::BeginSeq;
    tmpOut << YAML::BeginMap;

    //Search for data.
    ModelInfo tmpData; 
    readBasicModel(inFile, tmpData);

    //Triangles data
    tmpOut << YAML::Key << "Triangles";
    tmpOut << YAML::Value << tmpData.totalTriangles;

    tmpOut << YAML::Key << "Vertices";
    tmpOut << YAML::Value << tmpData.totalVertices;
    
    tmpOut << YAML::Key << "Index";
    tmpOut << YAML::Value << tmpData.totalIndex;
    
    tmpOut << YAML::Key << "Faces";
    tmpOut << YAML::Value << tmpData.totalFaces;
    
    tmpOut << YAML::Key << "Meshes";
    tmpOut << YAML::Value << tmpData.totalMeshes;
    
    tmpOut << YAML::Key << "Materials";
    tmpOut << YAML::Value << tmpData.totalMaterials;
    
    tmpOut << YAML::Key << "Animations";
    tmpOut << YAML::Value << tmpData.totalAnimations;

    tmpOut << YAML::EndMap;
    tmpOut << YAML::EndSeq;
    tmpOut << YAML::EndMap;
    

    ofstream fout(inFile.string()+ ".giData");
    fout << tmpOut.c_str();

  }

  void
  App::processData(ModelInfo& inInfo, 
                   aiNode* node,
                   const aiScene* inScene) {
    for (uint32 i = 0; i < node->mNumMeshes; ++i) {
      aiMesh* mesh = inScene->mMeshes[node->mMeshes[i]];
      inInfo.totalVertices += mesh->mNumVertices;
      inInfo.totalFaces += mesh->mNumFaces;
      for (uint32 j = 0; j < mesh->mNumFaces; ++j) {
        aiFace face = mesh->mFaces[j];
        inInfo.totalIndex += face.mNumIndices;
      }
    }
    // then do the same for each of its children
    for (uint32 i = 0; i < node->mNumChildren; i++) {
      processData(inInfo, node->mChildren[i], inScene);
    }
  }


  void
  App::readBasicModel(Path inFile, ModelInfo& inInfo) {
    Assimp::Importer importer;

    importer.ReadFile(inFile.string(),
                      aiProcessPreset_TargetRealtime_MaxQuality |
                      aiProcess_TransformUVCoords |
                      aiProcess_ConvertToLeftHanded |
                      aiProcess_Triangulate);

    const aiScene* tmpScene = importer.GetOrphanedScene();

    if (!tmpScene
      || tmpScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
      || !tmpScene->mRootNode) {
      //g_logger().SetError(ERROR_TYPE::kModelLoading, "Failed to load a model");
    }

    inInfo.totalMeshes = tmpScene->mRootNode->mNumMeshes;
    inInfo.totalAnimations = tmpScene->mNumAnimations;
    inInfo.totalMaterials = tmpScene->mNumMaterials;

    processData(inInfo, tmpScene->mRootNode, tmpScene);

    inInfo.totalTriangles = inInfo.totalIndex / 3;
  }

  ModelInfo 
  App::decodeData(Path inFile) {
    ifstream tmpStream(inFile);
    ModelInfo tmpInfo;
    std::stringstream tmpStr;
    tmpStr << tmpStream.rdbuf();

    YAML::Node tmpData = YAML::Load(tmpStr.str());
    if (tmpData["Model data"]) {
      auto tmpModelData = tmpData["Model data"];
      for (auto iterData : tmpModelData) {
        if (iterData["Triangles"]) {
          tmpInfo.totalTriangles = iterData["Triangles"].as<uint32>();
        }
        if (iterData["Vertices"]) {
          tmpInfo.totalVertices = iterData["Vertices"].as<uint32>();
        }
        if (iterData["Index"]) {
          tmpInfo.totalIndex = iterData["Index"].as<uint32>();
        }
        if (iterData["Faces"]) {
          tmpInfo.totalFaces = iterData["Faces"].as<uint32>();
        }
        if (iterData["Meshes"]) {
          tmpInfo.totalMeshes = iterData["Meshes"].as<uint32>();
        }
        if (iterData["Materials"]) {
          tmpInfo.totalMaterials = iterData["Materials"].as<uint32>();
        }
        if (iterData["Animations"]) {
          tmpInfo.totalAnimations = iterData["Animations"].as<uint32>();
        }
      }
    }
    return tmpInfo;
  }

  void
  App::createSphere(int32 numTriangles) {
    auto& configs = g_Configs();

    float radius = 1;
    uint32 sectors = MIN_SPHERE_SECTOR;
    for (int32 i = MIN_SPHERE_SECTOR; (((numTriangles) % i) == 0); i += 3) {
      sectors = i;
    }

    uint32 stacks = ((numTriangles) / sectors) / 2;

    uint32 sphereSectors = sectors < MIN_SPHERE_SECTOR ? MIN_SPHERE_SECTOR : sectors;
    uint32 sphereStacks = stacks < MIN_SPHERE_STACK ? MIN_SPHERE_STACK : stacks;

    float x, y, z;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorAngle;

    Vector<SimpleVertex> sphereVertices;
    Vector<uint32> sphereIndices;
    SimpleVertex vertex;
    vertex.Tang = Vector3(1.0f, 1.0f, 1.0f);
    vertex.BiNor = Vector3(1.0f, 1.0f, 1.0f);

    sphereVertices.resize((sphereSectors * (sphereStacks - 1)) + 2);
    int32 tmpIter = 0;
    ++tmpIter;

    //First Vertex data.
    SimpleVertex tmpVertex;
    tmpVertex.Pos = { 0, 1, 0 };
    tmpVertex.Nor = { 0, 1, 0 };
    tmpVertex.Tex = { 0.f, 1.f };
    sphereVertices[0] = tmpVertex;

    //Vertices
    for (uint32 i = 0; i < sphereStacks - 1; ++i) {
      float phi = PI * double(i + 1) / double(sphereStacks);
      for (uint32 j = 0; j < sphereSectors; ++j) {
        sectorAngle = 2.0 * PI * double(j) / double(sphereSectors);

        //Vertex
        x = std::sin(phi) * std::cos(sectorAngle);
        y = std::cos(phi);
        z = std::sin(phi) * std::sin(sectorAngle);
        vertex.Pos = Vector3(x, y, z);
        //Normal
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;
        vertex.Nor = Vector3(nx, ny, nz);
        //Texcoords
        s = (float)j / sphereSectors;
        t = (float)i / sphereStacks;
        vertex.Tex = Vector2(s, t);

        vertex.Tex.normalize();
        if (tmpIter >= ((sphereSectors * (sphereStacks - 1)) + 2)) {
          __debugbreak();
          tmpIter = tmpIter - 1;
          break;
        }
        else {
          sphereVertices[tmpIter] = vertex;
          ++tmpIter;
        }
      }
    }
    //Last vertex data.
    tmpVertex.Pos = { 0, -1, 0 };
    tmpVertex.Nor = { 0, -1, 0 };
    tmpVertex.Tex = { 0.5f, 0.5f };
    sphereVertices[tmpIter] = tmpVertex;

    //Indices

    //Triangles with the first Vertex.
    for (int32 i = 0; i < sphereSectors; ++i) {
      auto i0 = i + 1;
      auto i1 = (i + 1) % sphereSectors + 1;
      sphereIndices.push_back(0);
      sphereIndices.push_back(i0);
      sphereIndices.push_back(i1);
      i0 = i + sphereSectors * (sphereStacks - 2) + 1;
      i1 = (i + 1) % sphereSectors + sphereSectors * (sphereStacks - 2) + 1;
      sphereIndices.push_back(tmpIter);
      sphereIndices.push_back(i0);
      sphereIndices.push_back(i1);
    }

    //Middle triangles
    for (int32 i = 0; i < sphereStacks - 2; ++i) {
      auto i0 = i * sphereSectors + 1;
      auto i1 = (i + 1) * sphereSectors + 1;
      for (int32 j = 0; j < sphereSectors; ++j) {
        auto j0 = i0 + j;
        auto j1 = i0 + (j + 1) % sphereSectors;
        auto j2 = i1 + (j + 1) % sphereSectors;
        auto j3 = i1 + j;
        sphereIndices.push_back(j0);
        sphereIndices.push_back(j1);
        sphereIndices.push_back(j2);

        sphereIndices.push_back(j0);
        sphereIndices.push_back(j2);
        sphereIndices.push_back(j3);

      }
    }

    auto tmpMesh = make_shared<Mesh>(sphereVertices, sphereIndices);
    SharedPtr<Model> tmpModel = make_shared<Model>();
    tmpModel->m_meshes.push_back(tmpMesh);
    Exporter::ExportObj(configs.s_generatedPath.string() + "sphere.obj", tmpModel);
  }

  void
  App::createQuadSphere(int32 inSubdiv) {
    auto& configs = g_Configs();

    float a = 1.0f / sqrt(3.0f);

    pmp::SurfaceMesh tmpPMPMesh;

    // add the 8 vertices
    auto v0 = tmpPMPMesh.add_vertex(pmp::Point(-a, -a, -a));
    tmpPMPMesh.add_vertex_property("v:normal", pmp::Normal(1.f, 1.f, 1.f));
    tmpPMPMesh.add_halfedge_property("h:tex", pmp::TexCoord(1.f, 1.f));

    auto v1 = tmpPMPMesh.add_vertex(pmp::Point(a, -a, -a));
    auto v2 = tmpPMPMesh.add_vertex(pmp::Point(a, a, -a));
    auto v3 = tmpPMPMesh.add_vertex(pmp::Point(-a, a, -a));
    auto v4 = tmpPMPMesh.add_vertex(pmp::Point(-a, -a, a));
    auto v5 = tmpPMPMesh.add_vertex(pmp::Point(a, -a, a));
    auto v6 = tmpPMPMesh.add_vertex(pmp::Point(a, a, a));
    auto v7 = tmpPMPMesh.add_vertex(pmp::Point(-a, a, a));

    tmpPMPMesh.add_quad(v3, v2, v1, v0);
    tmpPMPMesh.add_quad(v2, v6, v5, v1);
    tmpPMPMesh.add_quad(v5, v6, v7, v4);
    tmpPMPMesh.add_quad(v0, v4, v7, v3);
    tmpPMPMesh.add_quad(v3, v7, v6, v2);
    tmpPMPMesh.add_quad(v1, v5, v4, v0);


    for (int32 i = 1; i <= inSubdiv; i++) {
      pmp::catmull_clark_subdivision(tmpPMPMesh);
    }

    pmp::IOFlags tmpFlags;
    tmpFlags.use_vertex_texcoords = true;
    tmpFlags.use_vertex_normals = true;
    tmpFlags.use_halfedge_texcoords = true;

    Path tmpPath = configs.s_generatedPath.string() + "sphere.obj";

    pmp::write_giAMR(tmpPMPMesh, tmpPath, tmpFlags);
    Exporter::ExportMtl(tmpPath);
  }

  SharedPtr<Texture>
  App::textureFromPath(Path inPath) {
    int32 w = 0, h = 0, comp = 0;

    comp = 4;

    SharedPtr<Texture> tmpTexture = make_shared<Texture>();

    //Get the information of the image loadead.
    uint8 * tmpImg = stbi_load(inPath.string().c_str(),
                               &w, 
                               &h, 
                               &comp, 4);

    //Create the texture with the information.
    CD3D11_TEXTURE2D_DESC tempDesc;
    memset(&tempDesc, 0, sizeof(tempDesc));
    tempDesc.Width = w;
    tempDesc.Height = h;
    tempDesc.MipLevels = 1;
    tempDesc.ArraySize = 1;
    tempDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tempDesc.SampleDesc.Count = 1;
    tempDesc.SampleDesc.Quality = 0;
    tempDesc.Usage = D3D11_USAGE_DEFAULT;
    tempDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    tempDesc.CPUAccessFlags = 0;
    tempDesc.MiscFlags = 0;
    
    if (FAILED(m_device->CreateTexture2D(&tempDesc, nullptr, &tmpTexture->m_texture))) {
    
      //Send error message
      //g_logger().SetError(ERROR_TYPE::kTextureCreation, "Texture can't be created");
      //Pone un breakpoint cuando llegue aqui
      __debugbreak();
      return SharedPtr<Texture>();
    }
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    memset(&srvDesc, 0, sizeof(srvDesc));
    srvDesc.Format = tempDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    
    if (FAILED(m_device->CreateShaderResourceView(tmpTexture->m_texture, 
                                                  &srvDesc, 
                                                  &tmpTexture->m_subResourceData))) {
    
      //Send error message
      /*g_logger().SetError(ERROR_TYPE::kTextureCreation, 
                          "Texture can't be created as a Shader Resource View");*/
      __debugbreak();
      return SharedPtr<Texture>();
    }
    
    m_devContext->UpdateSubresource(tmpTexture->m_texture,
                                    0,
                                    NULL,
                                    tmpImg,
                                    w * 4,
                                    0);
    
    //Unload Data
    stbi_image_free(tmpImg);
    tmpTexture->m_path = inPath;
    tmpTexture->m_name = inPath.filename().string();
    
    //Sampler
    ID3D11SamplerState* tmpSampler;
    
    D3D11_SAMPLER_DESC sampDesc;
    memset(&sampDesc, 0, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = 3.402823466e+38f;
    sampDesc.MaxAnisotropy = 0;
    sampDesc.MipLODBias = 0;
    
    if (FAILED(m_device->CreateSamplerState(&sampDesc,
                                            &tmpSampler))) {
      /*g_logger().SetError(ERROR_TYPE::kSamplerCreation, 
                          "A sampler can't be created");*/
      __debugbreak();
      return SharedPtr<Texture>();
    }
    
    //Unload Data
    //stbi_image_free(tmpImg);

    return tmpTexture;
  }

  String
  App::openFileDialog() {
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = (HWND)m_window;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = m_fileFilters;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE) {
      return ofn.lpstrFile;
    }
    return String();
  }

  Path 
  App::selectFolderDialog() {
    BROWSEINFO bi = {0};
    bi.lpszTitle = L"Browse folder";
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != NULL)
    {
      TCHAR tszPath[MAX_PATH] = L"\0";

      if (SHGetPathFromIDList(pidl, tszPath) == TRUE) {
        //AfxMessageBox(tszPath);
        Path tmpPath = tszPath;
        return tmpPath;
      }

      // — Free pidl
      CoTaskMemFree(pidl);
    }
    
    return Path();
  }



}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

LRESULT
App::handleWindowEvent(HWND inHw,
  UINT inMsg,
  WPARAM inwParam,
  LPARAM inlParam) {

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

  return false;
}