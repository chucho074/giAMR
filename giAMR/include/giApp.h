/**
 * @file    giApp.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    08/05/2023
 * @brief   A basic description of the what do the doc.
 */
 
/**
 * @include
 */
#pragma once

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

//Windows
#include "windows.h"
//ImGUI
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

//dx11
#include <d3d11.h>
//Own includes
#include "giPrerequisitesAMR.h"
#include "giTexture.h"
#include "giModel.h"


//using namespace giAMRSDK;

namespace giAMRSDK {
  class App final
  {
  public:

    //Constrructor.
    App() = default;

    //Destructor
    ~App() = default;

    /**
     * @brief    Runs the app.
     * @return   Returns a 0 if everything it's ok.
     */
    int32
    run();

    /**
     * @brief    .
     * @return   Returns a 0 if everything it's ok.
     */
    int32
    onCreate();

    /**
     * @brief    Destroy's everything.
     */
    void
    onDestroy();

    /**
     * @brief    Updates
     * @param    inDT          The delta Time.
     */
    void
    onUpdate(float inDT);

    /**
     * @brief    Renders the information.
     */
    void
    onRender();

    /**
     * @brief    Process the events.
     */
    void
    onEvent();

  private:

    /**
     * @brief    Create the window.
     * @return   Returns a 0 if everything it's ok.
     */
    int32
    createWindow();

    /**
     * @brief    Initialize the systems of the project.
     */
    void
    initSystems();

    /**
     * @brief    Destroys the systems of the project.
     */
    void
    destroySystems();

    /**
     * @brief    Reads the "configs.json" file.
     * @return   Returns false if there are no configs to read.
     */
    bool
    readConfigs();

    /**
     * @brief    Set's the information of the configs of the project.
     */
    void
    setConfigs();

    /**
     * @brief    Write the configs file.
     */
    void
    writeConfigs();

    /**
     * @brief    Function in charge to verify if every saved data on the setup file is
                  update and valid.
     */
    void
    checkoutSetup();

    /**
     * @brief    In charge to render the window for the process of the tool.
     */
    void
    renderAMRprocess();

    /**
     * @brief    Creates the giData files.
     */
    void
    createData(Path inFile);

    /**
     * @brief    Process the data of the model.
     * @param    inInfo        The information structure to save data.
     * @param    node          The node verify.
     * @param    inScene       The assimp scene from the readed file.
     */
    void 
    processData(ModelInfo & inInfo, 
                aiNode * node, 
                const aiScene * inScene);

    /**
     * @brief    Reads a model and get the information of it.
     * @param    inFile        The path of the model.
     * @param    inInfo        The info structure.
     */
    void
    readBasicModel(Path inFile, ModelInfo& inInfo);

    /**
     * @brief    Decode the data of the model.
     * @param    inFile        The path of the model.
     * @return   Returns the basic info of the model.
     */
    ModelInfo
    decodeData(Path inFile);

    /**
     * @brief    Creates a UV Sphere by a given triangle number.
     * @param    numTriangles  The number of triangles for create the sphere.
     */
    void
    createSphere(int32 numTriangles);

    /**
     * @brief    Create a Quad Sphere.
     * @param    inSubdiv      The number of subdivisions for create the sphere.
     */
    void
    createQuadSphere(int32 inSubdiv);

  private:

    /**
     * @brief    Create a texture from a given Path.
     * @param    inPath        The path for create the texture.
     * @return   Returns the texture.
     */
    SharedPtr<Texture>
    textureFromPath(Path inPath);

    /**
     * @brief	   Method that set the events and messages for the game.
     * @param	   inHw          The HWND
     * @param	   msg		       The message
     * @param	   wParam	       The W Parameter
     * @param	   lParam	       The L Parameter
     * @return	 Returns the result of the handle event.
     */
    static LRESULT
    CALLBACK handleWindowEvent(HWND inHw,
                               UINT msg,
                               WPARAM wParam,
                               LPARAM lParam);

    /**
     * @brief    Opens a dialog for select a file.
     * @return   Returns the path of the file to open.
     */
    String
    openFileDialog();

    /**
     * @brief    Opens a dialog for select a folder.
     * @return   Returns the path of the folder to save.
     */
    Path
    selectFolderDialog();


    int32 m_subdiv = 1;

    float m_width = 500;
    float m_height = 720;
    HWND m_hWnd = nullptr;
    void* m_window = nullptr;


    bool m_askAnaconda = false;

    static inline const char* m_fileFilters = "obj files\0*.obj\0fbx files\0*.fbx\0";

    //Configs

    ///Reference to the device
    ID3D11Device* m_device = nullptr;

    ///Reference to the device Context
    ID3D11DeviceContext* m_devContext = nullptr;

    ///Reference to the Swap Chain
    IDXGISwapChain* m_swapChain = nullptr;

    ///Reference to the default Render Target
    ID3D11RenderTargetView* m_backBuffer = nullptr;

  };
}