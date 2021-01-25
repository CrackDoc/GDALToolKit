#include "RasterReprojection.h"
#include "XFile.h"
#include "StlUtil.h"
#include "gdal_priv.h"
#include "gdalwarper.h"

CRasterReprojection::CRasterReprojection(void)
{
}


CRasterReprojection::~CRasterReprojection(void)
{

}

bool CRasterReprojection::Reprojection( const std::string & strTifPath,OGRSpatialReference exportSRS )
{
	const char *pszOutFile = strTifPath.c_str();

	std::stringstream ss;

	IOx::XFile xfile(strTifPath);
	xfile.Normalize();

	std::string strAbsolutePath = xfile.absoluteDir();

	ss<<strAbsolutePath<<"/ClipTmp.tif";

	std::string strReNameFilePath = ss.str();

	stlu::fileRename(pszOutFile,strReNameFilePath);


	GDALDataset *pSrcDataset = NULL;

	pSrcDataset = (GDALDataset*)GDALOpenEx(strReNameFilePath.c_str(), GDAL_OF_ALL, NULL, NULL, NULL);

	if (pSrcDataset == NULL)
	{
		GDALClose(pSrcDataset);
		pSrcDataset = NULL;
		return false;
	}
	double stGeoTransform[6] = {0};
	int iDstWidth = 0;
	int iDstHeight = 0;
	char *szWkt = (char *)GDALGetProjectionRef(pSrcDataset);

	char *szExportWkt;
	exportSRS.exportToWkt(&szExportWkt);
	std::string strExportWkt(szExportWkt);
	void* hTransformArg = GDALCreateGenImgProjTransformer(pSrcDataset,szWkt,NULL,strExportWkt.c_str(),false,0,0);
	if(hTransformArg != NULL)
	{
		CPLErr err = GDALSuggestedWarpOutput(pSrcDataset,GDALGenImgProjTransform,hTransformArg,stGeoTransform,&iDstWidth,&iDstHeight);
		GDALDestroyGenImgProjTransformer(hTransformArg);
	}
	char** papszWarpOptions = NULL;
	papszWarpOptions = CSLSetNameValue( papszWarpOptions, "INIT_DEST","NO_DATA");
	papszWarpOptions = CSLSetNameValue( papszWarpOptions, "TILED","YES");
	papszWarpOptions = CSLSetNameValue( papszWarpOptions, "INTERLEAVE","PIXEL");
	papszWarpOptions = CSLSetNameValue( papszWarpOptions, "BIGTIFF","IF_NEEDED");

	int nBoundCnt = GDALGetRasterCount(pSrcDataset);

	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");

	GDALDataset *pGDALDataset = poDriver->Create(pszOutFile,iDstWidth,iDstHeight,nBoundCnt,GDT_Byte,papszWarpOptions);

	if(pGDALDataset == NULL)
	{
		return false;
	}
	CPLAssert( pGDALDataset != NULL );
	// 设置投影信息，包含7参数
	GDALSetProjection(pGDALDataset,strExportWkt.c_str());  
	GDALSetGeoTransform(pGDALDataset,stGeoTransform );  
	// 建立变换选项  
	GDALWarpOptions* psWarpOptions = GDALCreateWarpOptions();  
	psWarpOptions->hSrcDS =pSrcDataset;  
	psWarpOptions->hDstDS =pGDALDataset;  

	int nBandCount = GDALGetRasterCount(pSrcDataset);
	psWarpOptions->nBandCount = nBandCount; 
	psWarpOptions->panSrcBands =  
		(int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );  
	for (int i = 0; i < nBandCount; i ++)
	{
		psWarpOptions->panSrcBands[i] = i+1;
	}
	psWarpOptions->panDstBands =  
		(int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );  
	for (int i = 0; i < nBandCount; i ++)
	{
		psWarpOptions->panDstBands[i] = i+1;
	} 
	psWarpOptions->pfnProgress = NULL; //ALGTermProgress;
	psWarpOptions->pProgressArg = NULL;//&m_RejectionCallBack;

	psWarpOptions->eResampleAlg = GRA_NearestNeighbour;

	// 创建重投影变换函数  
	psWarpOptions->pTransformerArg =  
		GDALCreateGenImgProjTransformer( pSrcDataset,  
		GDALGetProjectionRef(pSrcDataset),  
		pGDALDataset,  
		GDALGetProjectionRef(pGDALDataset),  
		FALSE,0.0, 3);  
	psWarpOptions->pfnTransformer = GDALGenImgProjTransform;  

	// 初始化并且执行变换操作  

	GDALWarpOperation oOperation;  
	if(oOperation.Initialize(psWarpOptions ) != CE_None)
	{
		if(pGDALDataset!= NULL)
		{
			GDALClose(pGDALDataset);
			pGDALDataset = NULL;
		}
		if(pSrcDataset != NULL)
		{
			GDALClose(pSrcDataset);
			pSrcDataset = NULL;
		}
		return false;
	}
	//优化选项
	psWarpOptions->dfWarpMemoryLimit = 128000000.0;
	char** szWarpOptions=NULL;
	//QString strCpuNumber = QString("%0").arg(8);
	szWarpOptions = CSLSetNameValue(papszWarpOptions,"NUM_THREADS","ALL_CPUS");

	//szWarpOptions = CSLSetNameValue(szWarpOptions,"NUM_THREADS",strCpuNumber.toLocal8Bit().data());

	szWarpOptions = CSLSetNameValue(szWarpOptions,"WRITE_FLUSH","YES");

	psWarpOptions->papszWarpOptions=szWarpOptions;


	oOperation.ChunkAndWarpMulti(0,0,GDALGetRasterXSize(pGDALDataset),GDALGetRasterYSize(pGDALDataset));  

	GDALDestroyGenImgProjTransformer(psWarpOptions->pTransformerArg );  
	GDALDestroyWarpOptions( psWarpOptions );  

	if(pGDALDataset!= NULL)
	{
		GDALClose(pGDALDataset);
		pGDALDataset = NULL;
	}
	if(pSrcDataset != NULL)
	{
		GDALClose(pSrcDataset);
		pSrcDataset = NULL;
	}
	CPLFree(szExportWkt);
	stlu::fileRemove(strReNameFilePath);
	return true;
}
