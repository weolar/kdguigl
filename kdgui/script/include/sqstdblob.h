/*	see copyright notice in squirrel.h */
#ifndef _SQSTDBLOB_H_
#define _SQSTDBLOB_H_

#ifdef __cplusplus
extern "C" {
#endif

SQUIRREL_API SQUserPointer SQUIRREL_CALL sqstd_createblob(HSQUIRRELVM v, SQInteger size);
SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_getblob(HSQUIRRELVM v,SQInteger idx,SQUserPointer *ptr);
SQUIRREL_API SQInteger SQUIRREL_CALL sqstd_getblobsize(HSQUIRRELVM v,SQInteger idx);

SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_register_bloblib(HSQUIRRELVM v);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*_SQSTDBLOB_H_*/

