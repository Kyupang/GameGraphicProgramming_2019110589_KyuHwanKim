/*+===================================================================
  File:      MAIN.CPP
  Summary:   This application demonstrates creating a Direct3D 11 device
  Origin:    http://msdn.microsoft.com/en-us/library/windows/apps/ff729718.aspx
  Originally created by Microsoft Corporation under MIT License
  © 2022 Kyung Hee University
===================================================================+*/
#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include <Windows.h>

#include "Common.h"

#include "Game/Game.h"



/*F+F+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  Function: wWinMain
  Summary:  Entry point to the program. Initializes everything and
            goes into a message processing loop. Idle time is used to
            render the scene.
  Args:     HINSTANCE hInstance
              Handle to an instance.
            HINSTANCE hPrevInstance
              Has no meaning.
            LPWSTR lpCmdLine
              Contains the command-line arguments as a Unicode
              string
            INT nCmdShow
              Flag that says whether the main application window
              will be minimized, maximized, or shown normally
  Returns:  INT
              Status code.
-----------------------------------------------------------------F-F*/


INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    if (FAILED(library::InitWindow(hInstance, nCmdShow))) {
        return 0;
    }
    if (FAILED(library::InitDevice())) {
        return 0;
    }

    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            library::Render();  // Do some rendering
        }
    }

    // Destroy
    library::CleanupDevice();

    return static_cast<INT>(msg.wParam);
}

