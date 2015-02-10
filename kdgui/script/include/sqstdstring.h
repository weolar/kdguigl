/*	see copyright notice in squirrel.h */
#ifndef _SQSTD_STRING_H_
#define _SQSTD_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int SQRexBool;
typedef struct SQRex SQRex;

typedef struct {
	const SQChar *begin;
	SQInteger len;
} SQRexMatch;

SQUIRREL_API SQRex* SQUIRREL_CALL sqstd_rex_compile(const SQChar *pattern,const SQChar **error);
SQUIRREL_API void SQUIRREL_CALL sqstd_rex_free(SQRex *exp);
SQUIRREL_API SQBool SQUIRREL_CALL sqstd_rex_match(SQRex* exp,const SQChar* text);
SQUIRREL_API SQBool SQUIRREL_CALL sqstd_rex_search(SQRex* exp,const SQChar* text, const SQChar** out_begin, const SQChar** out_end);
SQUIRREL_API SQBool SQUIRREL_CALL sqstd_rex_searchrange(SQRex* exp,const SQChar* text_begin,const SQChar* text_end,const SQChar** out_begin, const SQChar** out_end);
SQUIRREL_API SQInteger SQUIRREL_CALL sqstd_rex_getsubexpcount(SQRex* exp);
SQUIRREL_API SQBool SQUIRREL_CALL sqstd_rex_getsubexp(SQRex* exp, SQInteger n, SQRexMatch *subexp);

SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_format(HSQUIRRELVM v,SQInteger nformatstringidx,SQInteger *outlen,SQChar **output);

SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_register_stringlib(HSQUIRRELVM v);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*_SQSTD_STRING_H_*/
