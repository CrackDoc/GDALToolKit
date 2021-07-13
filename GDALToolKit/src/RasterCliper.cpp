#include "RasterCliper.h"
#include "XFile.h"
#include "StlUtil.h"
CRasterCliper::CRasterCliper()
{
}


CRasterCliper::~CRasterCliper()
{
}

bool CRasterCliper::Clip( const char *szFilePath,OGRGeometry *CliperGeometry )
{
	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
	if (pDriver == NULL)
	{
		return false;
	}

	const char *pszOutFile = szFilePath;

	std::stringstream ss;

	IOx::XFile xfile(szFilePath);
	xfile.Normalize();

	std::string strAbsolutePath = "";//xfile.absoluteDir();

	ss<<strAbsolutePath<<"/ClipTmp.tif";

	std::string strReNameFilePath = ss.str();

	stlu::fileRename(pszOutFile,strReNameFilePath);

	void *hCutLine = NULL;

	GDALDataset * pSrcDS = (GDALDataset*)GDALOpen(strReNameFilePath.c_str(),GA_ReadOnly);

	if (pSrcDS == NULL)
	{
		GDALClose(pSrcDS);
		return false;
	}

	int iBandCount = pSrcDS->GetRasterCount();
	const char* pszWkt = pSrcDS->GetProjectionRef();
	GDALDataType dT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	double adfGeoTransform[6] = {0};
	double newGeoTransform[6] = {0};

	pSrcDS->GetGeoTransform(adfGeoTransform);
	memcpy(newGeoTransform, adfGeoTransform, sizeof(double)*6);

	int iNewBandCount = iBandCount;
	//输入输出波段对应
	int *pSrcBand = NULL;
	int *pDstBand = NULL;

	//if(pBandIndex != NULL && pnBandCount != NULL) {
	//	pSrcBand = new int[iBandCount];
	//	pDstBand = new int[iBandCount];

	//	for(int i = 0; i < iBandCount; i++) {
	//		pSrcBand[i] = pBandIndex[i];
	//		pDstBand[i] = i + 1;
	//	}
	//} 
	//else
	{
		pSrcBand = new int[iBandCount];
		pDstBand = new int[iBandCount];

		for(int i = 0; i < iBandCount; i++) {
			pSrcBand[i] = i + 1;
			pDstBand[i] = i + 1;
		}
	}
	const char *chSrcWKT = pSrcDS->GetProjectionRef();
	//源投影
	OGRSpatialReference src_oSRS(chSrcWKT);
	//目标投影
	OGRSpatialReference *pDest = CliperGeometry->getSpatialReference();

	if(false == (bool)src_oSRS.IsSame(pDest))
	{
		CliperGeometry->transformTo(&src_oSRS);
	}

	OGRGeometryH hGeometry = (OGRGeometryH) CliperGeometry;
	OGRGeometryH hMultiPoly = NULL;

	// 外扩
	//if (iBuffer > 0)
	//{
	//	double dDistance = ABS(adfGeoTransform[1]*iBuffer);
	//	hMultiPoly = OGR_G_Buffer(hGeometry, dDistance, 30);
	//	OGR_G_AssignSpatialReference(hMultiPoly, OGR_G_GetSpatialReference(hGeometry));
	//	OGR_G_DestroyGeometry(hGeometry);
	//}
	//else
	hMultiPoly = hGeometry;

	OGREnvelope eRect;
	OGR_G_GetEnvelope(hMultiPoly, &eRect);

	int iNewWidth = 0, iNewHeigh = 0;
	int iBeginRow = 0, iBeginCol = 0;

	newGeoTransform[0] = eRect.MinX;
	newGeoTransform[3] = eRect.MaxY;

	iNewWidth = static_cast<int>((eRect.MaxX -eRect.MinX) / ABS(adfGeoTransform[1]));
	iNewHeigh = static_cast<int>((eRect.MaxY -eRect.MinY) / ABS(adfGeoTransform[5]));

	Projection2ImageRowCol(adfGeoTransform, newGeoTransform[0], newGeoTransform[3], iBeginCol, iBeginRow);
	ImageRowCol2Projection(adfGeoTransform, iBeginCol, iBeginRow, newGeoTransform[0], newGeoTransform[3]);

	char** papszTO = NULL;
	papszTO = CSLSetNameValue( papszTO, "INIT_DEST","NO_DATA");
	papszTO = CSLSetNameValue( papszTO, "TILED","YES");
	papszTO = CSLSetNameValue( papszTO, "INTERLEAVE","PIXEL");
	papszTO = CSLSetNameValue( papszTO, "BIGTIFF","IF_NEEDED");

	GDALDataset* pDstDS = pDriver->Create(pszOutFile, iNewWidth, iNewHeigh, iNewBandCount, dT, papszTO);
	if (pDstDS == NULL)
	{
		GDALClose((GDALDatasetH) pSrcDS);
		return false;
	}

	pDstDS->SetGeoTransform(newGeoTransform);
	pDstDS->SetProjection(pszWkt);

	//GDALSetRasterNoDataValue(pDstDS->GetRasterBand(1),m_bgColor.r);
	//GDALSetRasterNoDataValue(pDstDS->GetRasterBand(2),m_bgColor.g);
	//GDALSetRasterNoDataValue(pDstDS->GetRasterBand(3),m_bgColor.b);
	if(iNewBandCount > 3)
	{
		//GDALSetRasterNoDataValue(pDstDS->GetRasterBand(4),m_bgColor.a);
	}
	void *hTransformArg, *hGenImgProjArg=NULL;

	/* -------------------------------------------------------------------- */
	/*      Create a transformation object from the source to               */
	/*      destination coordinate system.                                  */
	/* -------------------------------------------------------------------- */
	hTransformArg = hGenImgProjArg = 
		GDALCreateGenImgProjTransformer2( pSrcDS, (GDALDatasetH)pDstDS,NULL);


	if( hTransformArg == NULL )
	{

		GDALClose((GDALDatasetH) pSrcDS);
		GDALClose((GDALDatasetH) pDstDS);
		return false;
	}
	GDALWarpOptions *psWO = GDALCreateWarpOptions();

	psWO->pfnTransformer = GDALGenImgProjTransform;
	psWO->pTransformerArg = hTransformArg;

	psWO->papszWarpOptions = CSLDuplicate(NULL);
	psWO->eWorkingDataType = dT;
	psWO->eResampleAlg = GRA_Bilinear ;

	psWO->hSrcDS = (GDALDatasetH) pSrcDS;
	psWO->hDstDS = (GDALDatasetH) pDstDS;

	psWO->pfnProgress = NULL;//ALGTermProgress;
	psWO->pProgressArg = NULL;//&m_CliperCallBack;

	psWO->nBandCount = iNewBandCount;

	psWO->dfWarpMemoryLimit = 128000000.0;

	//源和输出对应的波段
	psWO->nBandCount = iBandCount;
	psWO->panSrcBands = (int *) CPLMalloc(iBandCount*sizeof(int));
	psWO->panDstBands = (int *) CPLMalloc(iBandCount*sizeof(int));

	for(int i=0; i<iBandCount; i++) {
		psWO->panSrcBands[i] = pSrcBand[i];
		psWO->panDstBands[i] = pDstBand[i];
	}
	delete[] pSrcBand;
	delete[] pDstBand;

	psWO->hCutline = (void*) hMultiPoly;

	char** papszWarpOptions=NULL;
	//QString strCpuNumber = QString("%0").arg(8);
	//papszWarpOptions = CSLSetNameValue(papszWarpOptions,"NUM_THREADS",strCpuNumber.toLocal8Bit().data());
	papszWarpOptions = CSLSetNameValue(papszWarpOptions,"NUM_THREADS","ALL_CPUS");
	papszWarpOptions = CSLSetNameValue(papszWarpOptions,"WRITE_FLUSH","YES");
	psWO->papszWarpOptions=papszWarpOptions;

	TransformCutlineToSource((GDALDatasetH) pSrcDS, (void*)hMultiPoly, &(psWO->papszWarpOptions), papszTO );

	GDALWarpOperation oWO;
	if (oWO.Initialize(psWO) != CE_None)
	{
		GDALClose((GDALDatasetH) pSrcDS);
		GDALClose((GDALDatasetH) pDstDS);
		return false;
	}
	CPLErr err1 = oWO.ChunkAndWarpMulti(0, 0, iNewWidth, iNewHeigh);

	GDALDestroyGenImgProjTransformer(psWO->pTransformerArg);
	GDALDestroyWarpOptions( psWO );
	GDALClose((GDALDatasetH) pSrcDS);
	GDALClose((GDALDatasetH) pDstDS);

	stlu::fileRemove(strReNameFilePath);
	return true;
}

bool CRasterCliper::Projection2ImageRowCol( double *adfGeoTransform, double dProjX, double dProjY, int &iCol, int &iRow )
{
	double dTemp = adfGeoTransform[1]*adfGeoTransform[5] - adfGeoTransform[2]*adfGeoTransform[4];
	double dCol = 0.0, dRow = 0.0;
	dCol = (adfGeoTransform[5]*(dProjX - adfGeoTransform[0]) - 
		adfGeoTransform[2]*(dProjY - adfGeoTransform[3])) / dTemp + 0.5;
	dRow = (adfGeoTransform[1]*(dProjY - adfGeoTransform[3]) - 
		adfGeoTransform[4]*(dProjX - adfGeoTransform[0])) / dTemp + 0.5;

	iCol = dCol;
	iRow = dRow;
	return true;
}

bool CRasterCliper::ImageRowCol2Projection( double *adfGeoTransform, int iCol, int iRow, double &dProjX, double &dProjY )
{
	//adfGeoTransform[6]  数组adfGeoTransform保存的是仿射变换中的一些参数，分别含义见下
	//adfGeoTransform[0]  左上角x坐标 
	//adfGeoTransform[1]  东西方向分辨率
	//adfGeoTransform[2]  旋转角度, 0表示图像 "北方朝上"
	//adfGeoTransform[3]  左上角y坐标 
	//adfGeoTransform[4]  旋转角度, 0表示图像 "北方朝上"
	//adfGeoTransform[5]  南北方向分辨率

	dProjX = adfGeoTransform[0] + adfGeoTransform[1] * iCol + adfGeoTransform[2] * iRow;
	dProjY = adfGeoTransform[3] + adfGeoTransform[4] * iCol + adfGeoTransform[5] * iRow;
	return true;

}

void CRasterCliper::LoadCutline( const char *pszCutlineDSName, const char *pszCLayer, const char *pszCWHERE, const char *pszCSQL, void **phCutlineRet )
{
	OGRDataSourceH hSrcDS;

	hSrcDS = OGROpen( pszCutlineDSName, FALSE, NULL );
	if( hSrcDS == NULL )
		exit( 1 );

	OGRLayerH hLayer = NULL;

	if( pszCSQL != NULL )
		hLayer = OGR_DS_ExecuteSQL( hSrcDS, pszCSQL, NULL, NULL );
	else if( pszCLayer != NULL )
		hLayer = OGR_DS_GetLayerByName( hSrcDS, pszCLayer );
	else
		hLayer = OGR_DS_GetLayer( hSrcDS, 0 );

	if( hLayer == NULL )
	{
		fprintf( stderr, "Failed to identify source layer from datasource.\n" );
		exit( 1 );
	}

	if( pszCWHERE != NULL )
		OGR_L_SetAttributeFilter( hLayer, pszCWHERE );

	OGRFeatureH hFeat;
	OGRGeometryH hMultiPolygon = OGR_G_CreateGeometry( wkbMultiPolygon );

	OGR_L_ResetReading( hLayer );

	while( (hFeat = OGR_L_GetNextFeature( hLayer )) != NULL )
	{
		OGRGeometryH hGeom = OGR_F_GetGeometryRef(hFeat);

		if( hGeom == NULL )
		{
			fprintf( stderr, "ERROR: Cutline feature without a geometry.\n" );
			exit( 1 );
		}

		OGRwkbGeometryType eType = wkbFlatten(OGR_G_GetGeometryType( hGeom ));

		if( eType == wkbPolygon )
			OGR_G_AddGeometry( hMultiPolygon, hGeom );
		else if( eType == wkbMultiPolygon )
		{
			int iGeom;

			for( iGeom = 0; iGeom < OGR_G_GetGeometryCount( hGeom ); iGeom++ )
			{
				OGR_G_AddGeometry( hMultiPolygon,
					OGR_G_GetGeometryRef(hGeom,iGeom) );
			}
		}
		else
		{
			fprintf( stderr, "ERROR: Cutline not of polygon type.\n" );
			exit( 1 );
		}

		OGR_F_Destroy( hFeat );
	}

	if( OGR_G_GetGeometryCount( hMultiPolygon ) == 0 )
	{
		fprintf( stderr, "ERROR: Did not get any cutline features.\n" );
		exit( 1 );
	}

	OGR_G_AssignSpatialReference(
		hMultiPolygon, OGR_L_GetSpatialRef(hLayer) );

	*phCutlineRet = (void *) hMultiPolygon;

	if( pszCSQL != NULL )
		OGR_DS_ReleaseResultSet( hSrcDS, hLayer );

	OGR_DS_Destroy( hSrcDS );
}

void CRasterCliper::TransformCutlineToSource( GDALDatasetH hSrcDS, void *hCutline, char ***ppapszWarpOptions, char **papszTO_In )
{

	OGRGeometryH hMultiPolygon = OGR_G_Clone( (OGRGeometryH) hCutline );  
	char **papszTO = CSLDuplicate( papszTO_In );  

	/* -------------------------------------------------------------------- */  
	/*      Checkout that SRS are the same.                                 */  
	/* -------------------------------------------------------------------- */  
	OGRSpatialReferenceH  hRasterSRS = NULL;  
	const char *pszProjection = NULL;  

	if( GDALGetProjectionRef( hSrcDS ) != NULL   
		&& strlen(GDALGetProjectionRef( hSrcDS )) > 0 )  
		pszProjection = GDALGetProjectionRef( hSrcDS );  
	else if( GDALGetGCPProjection( hSrcDS ) != NULL )  
		pszProjection = GDALGetGCPProjection( hSrcDS );  

	if( pszProjection != NULL )  
	{  
		hRasterSRS = OSRNewSpatialReference(NULL);  
		if( OSRImportFromWkt( hRasterSRS, (char **)&pszProjection ) != CE_None )  
		{  
			OSRDestroySpatialReference(hRasterSRS);  
			hRasterSRS = NULL;  
		}  
	}  

	OGRSpatialReferenceH hCutlineSRS = OGR_G_GetSpatialReference( hMultiPolygon );  
	if( hRasterSRS != NULL && hCutlineSRS != NULL )  
	{  
		/* ok, we will reproject */  
	}  
	else if( hRasterSRS != NULL && hCutlineSRS == NULL )  
	{  
		fprintf(stderr,  
			"Warning : the source raster dataset has a SRS, but the cutline features/n"  
			"not.  We assume that the cutline coordinates are expressed in the destination SRS./n"  
			"If not, cutline results may be incorrect./n");  
	}  
	else if( hRasterSRS == NULL && hCutlineSRS != NULL )  
	{  
		fprintf(stderr,  
			"Warning : the input vector layer has a SRS, but the source raster dataset does not./n"  
			"Cutline results may be incorrect./n");  
	}  

	if( hRasterSRS != NULL )  
		OSRDestroySpatialReference(hRasterSRS);  

	/* -------------------------------------------------------------------- */  
	/*      Extract the cutline SRS WKT.                                    */  
	/* -------------------------------------------------------------------- */  
	if( hCutlineSRS != NULL )  
	{  
		char *pszCutlineSRS_WKT = NULL;  

		OSRExportToWkt( hCutlineSRS, &pszCutlineSRS_WKT );  
		papszTO = CSLSetNameValue( papszTO, "DST_SRS", pszCutlineSRS_WKT );  
		CPLFree( pszCutlineSRS_WKT );  
	}  

	/* -------------------------------------------------------------------- */  
	/*      Transform the geometry to pixel/line coordinates.               */  
	/* -------------------------------------------------------------------- */  
	CutlineTransformer oTransformer;  

	/* The cutline transformer will *invert* the hSrcImageTransformer */  
	/* so it will convert from the cutline SRS to the source pixel/line */  
	/* coordinates */  
	oTransformer.hSrcImageTransformer =   
		GDALCreateGenImgProjTransformer2( hSrcDS, NULL, papszTO );  

	CSLDestroy( papszTO );  

	if( oTransformer.hSrcImageTransformer == NULL )  
		exit( 1 );  

	OGR_G_Transform( hMultiPolygon,   
		(OGRCoordinateTransformationH) &oTransformer );  

	GDALDestroyGenImgProjTransformer( oTransformer.hSrcImageTransformer );  

	/* -------------------------------------------------------------------- */  
	/*      Convert aggregate geometry into WKT.                            */  
	/* -------------------------------------------------------------------- */  
	char *pszWKT = NULL;  

	OGR_G_ExportToWkt( hMultiPolygon, &pszWKT );  
	OGR_G_DestroyGeometry( hMultiPolygon );  

	*ppapszWarpOptions = CSLSetNameValue( *ppapszWarpOptions,   
		"CUTLINE", pszWKT );  
	CPLFree( pszWKT );  
}

int __stdcall ALGTermProgress( double dfComplete, const char *pszMessage, void *pProgressArg )
{
	//if(pProgressArg != NULL)
	//{
	//	CRasterClipper::IProgressCallback* poProgress =  (CRasterClipper::IProgressCallback*)pProgressArg;

	//	int nValue = dfComplete*100;

	//	if(CGDALJointDataSaver::nFrontValue != nValue)
	//	{
	//		CGDALJointDataSaver::nFrontValue  = nValue;
	//		bool isContinue = poProgress->OnProgress(nValue,100);
	//		return isContinue;
	//	}
	//}
	return true;
}
