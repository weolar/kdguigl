/*	see copyright notice in squirrel.h */
#ifndef _SQSTD_AUXLIB_H_
#define _SQSTD_AUXLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

SQUIRREL_API void SQUIRREL_CALL sqstd_seterrorhandlers(HSQUIRRELVM v);
SQUIRREL_API void SQUIRREL_CALL sqstd_printcallstack(HSQUIRRELVM v);
SQUIRREL_API void SQUIRREL_CALL sqstd_errorcallstack(HSQUIRRELVM v);

void sqstd_record_error_for_raise_exception(HSQUIRRELVM v);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* _SQSTD_AUXLIB_H_ */
