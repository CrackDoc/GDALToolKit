#ifndef RasterCliper_h__
#define RasterCliper_h__
#include "GDALToolKitExport.h"
#include "gdal.h"
#include "ogr_geometry.h"
#include <string>
#include <sstream>

int __stdcall ALGTermProgress(double dfComplete, const char *pszMessage, void *pProgressArg);

class GDALToolKit_EXPORT CRasterCliper
{
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
	CRasterCliper(void);
	~CRasterCliper(void);

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
