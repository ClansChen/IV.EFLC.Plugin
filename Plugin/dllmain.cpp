#include <windows.h>
#include "plugin.h"

#if 0
#pragma comment(linker, "/EXPORT:_BinkSetMemory@8=binkw32o._BinkSetMemory@8")
#pragma comment(linker, "/EXPORT:_BinkOpen@8=binkw32o._BinkOpen@8")
#pragma comment(linker, "/EXPORT:_BinkClose@4=binkw32o._BinkClose@4")
#pragma comment(linker, "/EXPORT:_BinkGetFrameBuffersInfo@8=binkw32o._BinkGetFrameBuffersInfo@8")
#pragma comment(linker, "/EXPORT:_BinkShouldSkip@4=binkw32o._BinkShouldSkip@4")
#pragma comment(linker, "/EXPORT:_BinkOpenDirectSound@4=binkw32o._BinkOpenDirectSound@4")
#pragma comment(linker, "/EXPORT:_BinkGetKeyFrame@12=binkw32o._BinkGetKeyFrame@12")
#pragma comment(linker, "/EXPORT:_BinkPause@8=binkw32o._BinkPause@8")
#pragma comment(linker, "/EXPORT:_BinkGoto@12=binkw32o._BinkGoto@12")
#pragma comment(linker, "/EXPORT:_BinkWait@4=binkw32o._BinkWait@4")
#pragma comment(linker, "/EXPORT:_BinkSetVolume@12=binkw32o._BinkSetVolume@12")
#pragma comment(linker, "/EXPORT:_BinkNextFrame@4=binkw32o._BinkNextFrame@4")
#pragma comment(linker, "/EXPORT:_BinkRegisterFrameBuffers@8=binkw32o._BinkRegisterFrameBuffers@8")
#pragma comment(linker, "/EXPORT:_BinkSetSoundSystem@8=binkw32o._BinkSetSoundSystem@8")
#pragma comment(linker, "/EXPORT:_BinkGetSummary@8=binkw32o._BinkGetSummary@8")
#pragma comment(linker, "/EXPORT:_BinkDoFrame@4=binkw32o._BinkDoFrame@4")
#endif

BOOL WINAPI DllMain(HMODULE module, DWORD reason, void *reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		Plugin::Init(module);
	}

	return TRUE;
}
