#ifndef RasterReprojection_h__
#define RasterReprojection_h__
#include "ogr_spatialref.h"
#include <string>
#include "GDALToolKitExport.h"
class GDALToolKit_EXPORT CRasterReprojection
{
public:
	CRasterReprojection(void);
	~CRasterReprojection(void);
	/**
	 * @fn      
	 * @brief brief
	 * @date 2021/1/25 14:05
	 * @param 
	 * @param 
	 * @return 
	*/
	bool Reprojection(const std::string & strTifPath,OGRSpatialReference exportSRS);
};
#endif // RasterReprojection_h__

