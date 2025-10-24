#ifndef _CFB_HTTP_CLIENT_DEF_
#define _CFB_HTTP_CLIENT_DEF_ 

#ifdef BUILD_STATIC
#define CFBHTTPCLIENT_EXPORTS
#else
#ifdef CFBHTTPCLIENT_LIB
#define CFBHTTPCLIENT_EXPORTS __declspec(dllexport)
#else
#define CFBHTTPCLIENT_EXPORTS __declspec(dllimport)
#endif
#endif

//#define BUILDING_CURLPP

#endif
