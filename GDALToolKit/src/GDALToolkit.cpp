#include "GDALToolkit.h"
#include "cpl_conv.h"

namespace IxGDALCoordinateToolKit
{
	bool IxGDALCoordinateToolKit::GDALCoordinateConvert( OGRGeometry *srcGeometry,OGRSpatialReference dSR )
	{
		OGRSpatialReference *pddSR = dSR.Clone();
		OGRErr error = srcGeometry->transformTo(pddSR);
		CPLFree(pddSR);
		return error==OGRERR_NONE;
	}
}
