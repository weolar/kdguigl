/*	see copyright notice in squirrel.h */
#ifndef _SQSTDIO_H_
#define _SQSTDIO_H_

#ifdef __cplusplus

#define SQSTD_STREAM_TYPE_TAG 0x80000000

struct SQStream {
	virtual SQInteger Read(void *buffer, SQInteger size) = 0;
	virtual SQInteger Write(void *buffer, SQInteger size) = 0;
	virtual SQInteger Flush() = 0;
	virtual SQInteger Tell() = 0;
	virtual SQInteger Len() = 0;
	virtual SQInteger Seek(SQInteger offset, SQInteger origin) = 0;
	virtual bool IsValid() = 0;
	virtual bool EOS() = 0;
};

extern "C" {
#endif

#define SQ_SEEK_CUR 0
#define SQ_SEEK_END 1
#define SQ_SEEK_SET 2

typedef void* SQFILE;

SQUIRREL_API SQFILE SQUIRREL_CALL sqstd_fopen(const SQChar *,const SQChar *);
SQUIRREL_API SQInteger SQUIRREL_CALL sqstd_fread(SQUserPointer, SQInteger, SQInteger, SQFILE);
SQUIRREL_API SQInteger SQUIRREL_CALL sqstd_fwrite(const SQUserPointer, SQInteger, SQInteger, SQFILE);
SQUIRREL_API SQInteger SQUIRREL_CALL sqstd_fseek(SQFILE , SQInteger , SQInteger);
SQUIRREL_API SQInteger SQUIRREL_CALL sqstd_ftell(SQFILE);
SQUIRREL_API SQInteger SQUIRREL_CALL sqstd_fflush(SQFILE);
SQUIRREL_API SQInteger SQUIRREL_CALL sqstd_fclose(SQFILE);
SQUIRREL_API SQInteger SQUIRREL_CALL sqstd_feof(SQFILE);

SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_createfile(HSQUIRRELVM v, SQFILE file,SQBool own);
SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_getfile(HSQUIRRELVM v, SQInteger idx, SQFILE *file);

//compiler helpers
SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_loadfile(HSQUIRRELVM v,const SQChar *filename,SQBool printerror);
SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_dofile(HSQUIRRELVM v,const SQChar *filename,SQBool retval,SQBool printerror);
SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_writeclosuretofile(HSQUIRRELVM v,const SQChar *filename);

SQUIRREL_API SQRESULT SQUIRREL_CALL sqstd_register_iolib(HSQUIRRELVM v);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*_SQSTDIO_H_*/

