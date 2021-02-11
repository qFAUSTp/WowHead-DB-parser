#ifndef _ALL_DEFINITIONS_
#define _ALL_DEFINITIONS_
#endif

#define CURL_STATICLIB
#include "curl/curl.h"

#define LIBXL_STATICLIB
#include "libxl/libxl.h"

#	pragma comment (lib, "libxl/libxl.lib") 

#ifdef _DEBUG
#	pragma comment (lib, "curl/libcurl_a_debug.lib")
#else
#	pragma comment (lib, "curl/libcurl_a.lib")
#endif 

#define	CurlGlobalInit		curl_global_init
#define	CurlEasyInit		curl_easy_init
#define	CurlEasySetopt		curl_easy_setopt
#define	CurlEasyPerform		curl_easy_perform
#define	CurlEasyCleanup		curl_easy_cleanup
#define	CurlGlobalCleanup	curl_global_cleanup