// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_NotificationTTSApi_h
#define incl_NotificationTTSApi_h

#if defined (_WINDOWS)
#if defined(NOTIFICATIONTTS_EXPORTS) 
#define NOTIFICATIONTTS_API __declspec(dllexport)
#else
#define NOTIFICATIONTTS_API __declspec(dllimport) 
#endif
#else
#define NOTIFICATIONTTS_API
#endif

#endif 
