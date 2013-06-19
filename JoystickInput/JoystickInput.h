// JoystickInput.h

#pragma once

#include <Windows.h>
extern "C"{
#include <Hidsdi.h>
}

#define MAX_BUTTONS		128
#define CHECK(exp)		{ if(!(exp)) goto Error; }
#define SAFE_FREE(p)	{ if(p) { HeapFree(hHeap, 0, p); (p) = NULL; } }
