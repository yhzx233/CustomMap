﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#pragma comment(lib, "../SDK/Lib/bedrock_server_api.lib")
#pragma comment(lib, "../SDK/Lib/bedrock_server_var.lib")
#pragma comment(lib, "../SDK/Lib/SymDBHelper.lib")
#pragma comment(lib, "../SDK/Lib/LiteLoader.lib")

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern void PluginInit();

extern "C" {
    _declspec(dllexport) void onPostInit() {
        std::ios::sync_with_stdio(false);
        PluginInit();
    }
}
