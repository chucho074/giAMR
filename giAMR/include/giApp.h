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

using namespace giAMRSDK;

class App final
{
 public:
  
  //Constrructor.
  App() = default;

  //Destructor
  ~App() = default;

  /**
   * @brief      Runs the app. 
   * @return     Returns a 0 if everything it's ok.
   */
  int32
  run();
  
  /**
   * @brief       
   * @return     Returns a 0 if everything it's ok.
   */
  int32
  onCreate();

  /**
   * @brief      Destroy's everything. 
   */
  void
  onDestroy();

  /**
   * @brief      Updates
   * @param      inDT          The delta Time.
   */
  void 
  onUpdate(float inDT);

  /**
   * @brief      Renders the information. 
   */
  void
  onRender();

  /**
   * @brief      Process the events.
   */
  void
  onEvent();

 private:

  /**
   * @brief 
   * @return     Returns a 0 if everything it's ok.
   */
  int32
  createWindow();
  
  /**
   * @brief      Initialize the systems of the project. 
   */
  void
  initSystems();
  
  /**
   * @brief      Destroys the systems of the project.
   */
  void
  destroySystems();

  /**
   * @brief      Reads the "configs.json" file.
   * @return     Returns false if there are no configs to read.
   */
  bool
  readConfigs();

  /**
   * @brief      Set's the information of the configs of the project. 
  */
  void
  setConfigs();

  /**
   * @brief      Function in charge to verify if every saved data on the setup file is 
                  update and valid.
   */
  void
  checkoutSetup();


 private:

   /**
    * @brief	   Method that set the events and messages for the game.
    * @param	   inHw          The HWND
    * @param	   msg		       The message
    * @param	   wParam	       The W Parameter
    * @param	   lParam	       The L Parameter
    * @return	   Returns the result of the handle event.
    */
   static LRESULT 
   CALLBACK handleWindowEvent(HWND inHw, UINT msg, WPARAM wParam, LPARAM lParam);


  uint32 m_width = 1280;
  uint32 m_height = 720;
  HWND m_hWnd = nullptr;
  void* m_window = nullptr;



  ///Reference to the device
  ID3D11Device* m_device = nullptr;

  ///Reference to the device Context
  ID3D11DeviceContext* m_devContext = nullptr;

  ///Reference to the Swap Chain
  IDXGISwapChain* m_swapChain = nullptr;

  ///Reference to the default Render Target
  ID3D11RenderTargetView * m_backBuffer = nullptr;

};