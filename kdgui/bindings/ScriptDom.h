
#ifndef ScriptDom_h
#define ScriptDom_h

#include "Script/include/squirrel.h"
#include "Script/scripthelp/SquirrelBindingsUtils.h"

SQInteger Sq$$(HSQUIRRELVM v);
SQInteger Sq$(HSQUIRRELVM v);
SQInteger SqSvgRoot(HSQUIRRELVM v);
SQInteger SqSvgDoc(HSQUIRRELVM v);

_DECL_CLASS(KSqNode);
_DECL_CLASS(KqEvt);
_DECL_CLASS(KqAnim);

#endif // ScriptDom_h