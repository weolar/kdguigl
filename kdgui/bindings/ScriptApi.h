#ifndef ScriptApi_h
#define ScriptApi_h

#include "script/include/squirrel.h"

SQInteger SqSetTimeout(HSQUIRRELVM v);
SQInteger SqSetInterval(HSQUIRRELVM v);
SQInteger SqClearTimeout(HSQUIRRELVM v);
SQInteger SqClearInterval(HSQUIRRELVM v);

SQInteger SqGetTickCount(HSQUIRRELVM v);

SQInteger SqGetScreenCursorPos(HSQUIRRELVM v);
SQInteger SqGetClientCursorPos(HSQUIRRELVM v);
SQInteger SqGetAsyncKeyState(HSQUIRRELVM v);
SQInteger SqGetCursorPosX(HSQUIRRELVM v);
SQInteger SqGetCursorPosY(HSQUIRRELVM v);

SQInteger SqIsDraggableRegionNcHitTest(HSQUIRRELVM v);

#endif // ScriptApi_h