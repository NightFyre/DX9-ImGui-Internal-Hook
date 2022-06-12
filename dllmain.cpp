#include "helper.h"
#include "Console.hpp"
#include "Game.hpp"
#include "D3D9Window.hpp"
#include "Hooking.hpp"
#include "Menu.hpp"

/// <summary>
/// Background Thread
/// - We utilize this to execute instructions constantantly without interfering with our render thread
/// </summary>
int COUNTER = 0;
void BackgroundWorker()
{
    using namespace DX9Base;

    //  This is a forward declaration for our exit strategy
    //  Our thread will never join (end) if we don't first declare a parameter to run by
    if (g_Running) {

        //  Branch for 3s timer without creating another background worker
        if (COUNTER >= 3000) {
            /// </summary>
            // Everything below this point will execute every 3s
            //  This section is for code we want to execute but not at the same time as everything else
            COUNTER = NULL;     //  RESET OUR COUNTER, otherwise this block will be executed each loop from this point on as COUNTER will always be >= 3000
        }

        /// </summary>
        // Everything below this point will execute every 1ms
        g_Menu->Loops();
        
        //  SETUP FOR NEXT LOOP
        COUNTER++;  //  Increase our counter by 1
        std::this_thread::sleep_for(1ms);   //  This thread will sleep after each execution for 1ms (1000ms = 1s)
    }
    return;
}

DWORD WINAPI MainThread()
{
    using namespace DX9Base;
    g_Console = std::make_unique<Console>();

    /// Pre-Processor check, really useful tool for toggling things per build.
    //  Perhaps we don't want to include console output. We can declare that simply with a 1 or 0 before compiling
    //  refer to "helper.h" 
#if DEVELOPER
    g_Console->InitializeConsole("DX9 ImGui Base - DEBUG CONSOLE");
    g_Console->printdbg("DX9 IMGUI BASE :: initializing . . .\n", g_Console->color.DEFAULT);
#endif

    g_GameVariables = std::make_unique<GameVariables>();
    g_GameData = std::make_unique<GameData>();
    g_D3D9Window = std::make_unique<D3D9Window>();
    g_Hooks = std::make_unique<Hooking>();
    g_Menu = std::make_unique<Menu>();
    
    //  With everything declared and in scope , we can begin creating our cheat
    g_Hooks->Hook();

#if DEVELOPER
    g_Console->printdbg("Main::Initialized\n", g_Console->color.green);
#endif

    //  
    std::thread UPDATE(BackgroundWorker);

    //  RENDER LOOP
    g_Running = TRUE;
    while (g_Running) {
        if (GetAsyncKeyState(VK_INSERT) & 1) g_GameVariables->m_ShowMenu = !g_GameVariables->m_ShowMenu;
    }
    
    //  EXIT PROCESS
    UPDATE.join();
    FreeLibraryAndExitThread(g_hModule, EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

/// <summary>
/// 
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call,  LPVOID lpReserved )
{
    using namespace DX9Base;
    g_hModule = hModule;
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            CreateThread(NULL , NULL, (LPTHREAD_START_ROUTINE)MainThread, g_hModule, NULL, NULL);
            break;
        }
        case DLL_PROCESS_DETACH: {
            // We trigger a boolean to initialize the detach sequence, otherwise we could face critical errors
            g_Killswitch = TRUE;
            break;
        }
    }
    return TRUE;
}

