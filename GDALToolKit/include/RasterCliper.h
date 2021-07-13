#ifndef RasterCliper_h__
#define RasterCliper_h__
#include "GDALToolKitExport.h"
#include "gdal.h"
#include "ogr_geometry.h"
#include "gdal_priv.h"
#include "gdalwarper.h"
#include "ogr_spatialref.h"
#include "cpl_conv.h"
#include "gdal_alg.h"
#include <string>
#include <sstream>

int __stdcall ALGTermProgress(double dfComplete, const char *pszMessage, void *pProgressArg);

class CutlineTransformer : public OGRCoordinateTransformation
{
public:

	void* hSrcImageTransformer;

	virtual OGRSpatialReference* GetSourceCS() { return NULL; }
	virtual OGRSpatialReference* GetTargetCS() { return NULL; }

	virtual int Transform(int nCount,
		double* x, double* y, double* z = NULL) {
		int nResult;

		int* pabSuccess = (int*)CPLCalloc(sizeof(int), nCount);
		nResult = TransformEx(nCount, x, y, z, pabSuccess);
		CPLFree(pabSuccess);

		return nResult;
	}

	virtual int TransformEx(int nCount,
		double* x, double* y, double* z = NULL,
		int* pabSuccess = NULL) {
		return GDALGenImgProjTransform(hSrcImageTransformer, TRUE,
			nCount, x, y, z, pabSuccess
		);
	}
};
class GDALToolKit_EXPORT CRasterCliper
{
	friend class CutlineTransformer;
public:
	class IProgressCallback
	{
	public:
		virtual ~IProgressCallback(){}
		/**
		 * @fn   OnProgress   
		 * @brief 返回裁剪进度结果
		 * @date 2021/1/25 11:04
		 * @param int nValue
		 * @param int nTotal
		 * @return bool
		*/
		virtual bool OnProgress(int nValue, int nTotal) = 0;
	};

public:
	CRasterCliper();
	~CRasterCliper();

	/**
	 * @fn   Clip   
	 * @brief 裁剪大图
	 * @date 2021/1/25 11:08
	 * @param 
	 * @param 
	 * @return bool
	*/
	bool Clip(const char *szFilePath,OGRGeometry *CliperGeometry );


	bool Projection2ImageRowCol(double *adfGeoTransform, double dProjX, double dProjY, int &iCol, int &iRow);

	bool ImageRowCol2Projection(double *adfGeoTransform, int iCol, int iRow, double &dProjX, double &dProjY);


	void LoadCutline( const char *pszCutlineDSName, const char *pszCLayer,
		const char *pszCWHERE, const char *pszCSQL,
		void **phCutlineRet );

	void TransformCutlineToSource( GDALDatasetH hSrcDS, void *hCutline,
		char ***ppapszWarpOptions, char **papszTO_In );

};
#endif // RasterCliper_h__
