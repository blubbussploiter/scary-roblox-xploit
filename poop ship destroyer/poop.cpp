
#include <thread>
#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>

#define version 1.0f

#include "output.h"

#include "exploit/datamodelSendStats.h"
#include "exploit/datamodel.h"
#include "exploit/execution.h"
#include "exploit/communication.h"
#include "exploit/util/static_addresses.h"
#include "exploit/util/security/bypasses.h"
#include "exploit/coolluaenv.h"

std::string getModuleName()
{
    char moduleName[MAX_PATH];
    GetModuleFileNameA(NULL, moduleName, MAX_PATH);

    char* baseName = strrchr(moduleName, '\\');
    if (baseName != NULL) {
        baseName++; 
    }
    else {
        baseName = moduleName;
    }
    return std::string(baseName);
}

static int poopMain()
{
    output o(RBX::random_string(6));

    output::printf("hi\n");

    output::printf("Cyllabus version %f initialized!\nCurrent module : '%s'\n", version, getModuleName().c_str());
    output::printf("-------------------\n");

    if (!RBX::FamilyGuy::scoutAddresses())
    {
        output::printf("Cyllabus cannot work without all addresses!\nPress any key to close this window...\n");
        cin.get();
        o.close();
        return -1;
    }

    uintptr_t taskscheduler = RBX::FamilyGuy::getTaskScheduler();
    uintptr_t scriptContext = RBX::FamilyGuy::getScriptContext();
    uintptr_t luaState = RBX::FamilyGuy::getLuaState();

    output::printf("taskscheduler = 0x%08X\n", taskscheduler);
    output::printf("scriptContext = 0x%08X\n", scriptContext);
    output::printf("luaState = 0x%08X\n", luaState);
    output::printf("luavm_secure = %d\n", RBX::FamilyGuy::luavm_secure_enabled);

    if (RBX::FamilyGuy::luavm_secure_enabled)
    {
        //RBX::placeBypasses();
    }

    doDatamodelDetours();
    pushAll();

    output::printf("-------------------\nAll loaded! :D\n");
    output::printf("-------------------\n");
    
    output::printf("Cyllabus version %f loaded\n", version);

  //ExploitHandler::get()->writeScript("warn(game)");
    static_addresses::nested::setThreadIdentity(luaState, 9);

    std::string script = "";
    while (1)
    {
        if (std::getline(std::cin, script))
        {
            if (!script.empty())
            {
                output::printf("script = %s\n", script.c_str());
                ExploitHandler::get()->writeScript(script);
                script = "";
            }
        }
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    unsigned long  ul_reason_for_call,
    LPVOID lpReserved
)
{
    DisableThreadLibraryCalls(hModule);

    if (ul_reason_for_call != DLL_PROCESS_ATTACH) return 1;
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&poopMain, NULL, NULL, NULL);

    return 1;
}