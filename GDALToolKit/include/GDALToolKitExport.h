#ifndef GDALToolKitExport_h__
#define GDALToolKitExport_h__

#if defined(WIN32)
#ifdef GDALToolKit_EXPORTS
#define GDALToolKit_EXPORT __declspec(dllexport)
#else
#define  GDALToolKit_EXPORT __declspec(dllimport)
#endif

#pragma warning( disable: 4251 )

#elif __linux__
#define GDALToolKit_EXPORT

#else
#define GDALToolKit_EXPORT 
#endif
#endif // ExtendStructureExport_h__
