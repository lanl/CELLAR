#ifndef ABI_EXTERN_H
#define ABI_EXTERN_H

#ifdef __cplusplus
#define EXTERN_C extern "C"
#define EXTERN_C_BEGIN EXTERN_C {
#define EXTERN_C_END }
#else
#define EXTERN_C
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif

#endif // ABI_EXTERN_H