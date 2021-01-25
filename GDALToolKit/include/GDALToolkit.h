#ifndef GDALToolkit_h__
#define GDALToolkit_h__
#include "GDALToolKitExport.h"
#include "ogr_spatialref.h"
#include "ogr_geometry.h"

namespace IxGDALCoordinateToolKit
{
	extern GDALToolKit_EXPORT bool GDALCoordinateConvert(OGRGeometry *srcGeometry,OGRSpatialReference dSR);
}
#endif // GDALToolkit_h__
