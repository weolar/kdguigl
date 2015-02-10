#ifndef SqErrorHandling_h
#define SqErrorHandling_h

extern int g_sq_error_result;
extern int g_sq_error_handling; // 错误处理方式。0:表示不抛异常，也不报告外部。1:报告外部

bool isThrowSqException();

int 
#ifdef _MSC_VER
WINAPI 
#endif
KdPageError(void* arg, const SQChar* s, ...);

#endif // SqErrorHandling_h
