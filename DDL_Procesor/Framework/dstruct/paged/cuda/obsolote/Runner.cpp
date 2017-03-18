//CudaRQ

////////////////////////////////////////////////////////
/// COLLECTIONS ON dbedu.cs.vsb cz are located in folder
/// 
/// E:\data\collectionsDERI\<NAME OF COLLECTION>\Export.bin
/// E:\data\collectionsDERI\<NAME OF COLLECTION>\Queries.txt
///
/// ON 158.196.157.187 are probably located in folder:
///    
////////////////////////////////////////////////////////

#include <common/cuda/globalDefs.h>
#include <common/cuda/dataDefs.h>
#include <common/cuda/dataManager.h>

extern "C" void startProcessingOnCUDA();

#include "common/datatype/tuple/cSpaceDescriptor.h"
#include "common/datatype/tuple/cTuple.h"
//#include "common\datatype\tuple\cMBRectangle.h"
#include "common/datatype/cBasicType.h"
#include "common/datatype/cDataType.h"
#include "common/stream/cStream.h"
#include "dstruct/paged/core/cNodeCache.h"
//#include "dstruct\paged\rtree\cRTree.h"
//#include "dstruct\paged\rtree\cRTreeHeader.h"
#include "common/utils/cTimer.h"
#include "dstruct/paged/rtree/compression/cTuplesCompressor.h"
#include "common/random/cGaussRandomGenerator.h"
#include "common/data/cTuplesGenerator.h"
#include "dstruct/paged/rtree/cRTreeConst.h"
#include "test/range_query/sequence_scan/cDataManager.h" //Contains paths for Collections and methods for loading.

using namespace common::data;
using namespace common::datatype::tuple;

//Collection specified by MYCOLLECTION is used when paths are not set via command line arguments
static const unsigned int MYCOLLECTION = Collection::XML;

int main2(int argc, char *argv[])
{
	///*QCoreApplication a(argc, argv);
	//return a.exec();*/

	int did = 0;
	cudaGetDevice(&did);
	printf("SELECTED DEVICE ID = %i\n", did);
	
	DataManager *dm = DataManager::Instance();
	//-i "c:\Technics Documents\PhD\SVN2\src\test\range_query\Cuda\bin\Win32\Release\export.bin" -dim 2 -norq 1 -r meteo_635278860.r -t meteo.time -bs 900000000 -rqsfile "c:\Technics Documents\PhD\SVN2\src\test\range_query\Cuda\bin\Win32\Release\queries.txt" 
	//if (dm->parseCommandLineArguments(argc, argv))
	{
		unsigned int tuplesCount = 0;
		unsigned int queriesCount = 0;
		int *p1=0;
		int *q1 =0;
		char* rqFile = dm->getRqs_file();
		int *queries = cDataManager::LoadQueries(rqFile,queriesCount,q1);
		dm->prepareInputs();
		dm->dataStorage.print();
		
		for(unsigned int i = 0; i < queriesCount; i++)
		{
			dm->dataStorage.data.H_rqs = new RQElement[dm->dataStorage.info.dim];
			for (unsigned int j = 0;j <dm->dataStorage.info.dim;j++)
			{
				dm->dataStorage.data.H_rqs[j].minimum = (int) *queries; //Ql[i]
				dm->dataStorage.data.H_rqs[j].maximum = (int) *(queries+dm->dataStorage.info.dim); //Qh[i]
				queries++;
			}
			//startProcessingOnCUDA();
			queries+=dm->dataStorage.info.dim;
		}
		int dim=8;
		cSpaceDescriptor *psd = new cSpaceDescriptor(dim, new cInt());

		dm->prepareInputs();
		dm->dataStorage.print();

		dm->dataStorage.data.H_rqs = new RQElement[dm->dataStorage.info.dim];
		//test query
		for (unsigned int i=0; i<dm->dataStorage.info.dim; i++)
		{
			dm->dataStorage.data.H_rqs[i].minimum = 10; //Ql
			dm->dataStorage.data.H_rqs[i].maximum = 100; //Qh
		}
		//startProcessingOnCUDA();
		return 0;
	}
}
