// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_TTSModuleApi_h
#define incl_TTSModuleApi_h

#if defined (_WINDOWS)
#if defined(TTS_MODULE_EXPORTS) 
#define TTS_MODULE_API __declspec(dllexport)
#else
#define TTS_MODULE_API __declspec(dllimport) 
#endif
#else
#define TTS_MODULE_API
#endif

#endif // incl_TTSModuleApi_h
