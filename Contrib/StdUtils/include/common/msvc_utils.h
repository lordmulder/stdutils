#ifndef __MSVC_FUNC_H__
#define __MSVC_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

long time(long *time);
int _snwprintf(wchar_t *buffer, size_t count, const wchar_t *format, ...);
int _snprintf(char *buffer, size_t count, const char *format, ...);
int sscanf(const char *input, const char * format, ...);
int swscanf(const wchar_t *input, const wchar_t * format, ...);

#ifdef __cplusplus
}
#endif

#endif //__MSVC_FUNC_H__
