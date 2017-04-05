#include "cColumn.h"


class cRecordGeneratorVar
{
public:
	std::vector<cColumn*>*columns;
	cSpaceDescriptor *columnSD;
	int index = 0;


	cRecordGeneratorVar(vector<cColumn*>*mColumns,cSpaceDescriptor *cSD);
	bool LoadGenerator(vector<cColumn*>*mColumns, cSpaceDescriptor *cSD);
	cHNTuple* CreateNewRecord();
	cNTuple* GenerateVarchar(int size,cSpaceDescriptor *sd);
	int GenerateInt();
};


inline cRecordGeneratorVar::cRecordGeneratorVar(vector<cColumn*>*mColumns, cSpaceDescriptor *cSD)
{
	LoadGenerator(mColumns, cSD);
}


inline bool cRecordGeneratorVar::LoadGenerator(vector<cColumn*>* mColumns, cSpaceDescriptor *cSD)
{
	columns = mColumns;
	columnSD = cSD;
	
	return true;
}


inline cHNTuple* cRecordGeneratorVar::CreateNewRecord()
{
	cHNTuple* record = new cHNTuple();
	
	record->Resize(columnSD);
	
	


	int positionInTable;
	for (int i = 0; i < columns->size(); i++)
	{
		if (columns->at(i)->cType->GetCode() == 'i')
		{
			if (columns->at(i)->primaryKey)
			{
				record->SetValue(i,index, columnSD);
				index++;
			}
			else
			{
				record->SetValue(i, GenerateInt(), columnSD);
			}
			
		}
		else if (columns->at(i)->cType->GetCode() == 'n')
		{
			cNTuple *value = GenerateVarchar(columns->at(i)->size, columns->at(i)->columnSD);
			record->SetValue(i, *value, columnSD);
		}
	
	}
	
	
	
	return record;
}


inline cNTuple* cRecordGeneratorVar::GenerateVarchar(int size, cSpaceDescriptor * sd)
{
	
	int randNumber;
	//srand(time(NULL));
	cNTuple * varcharTuple = new cNTuple(sd);

	for (int i = 0; i < size / 2; i++)
	{
		randNumber = rand() % 94 + 20;
		varcharTuple->SetValue(i, (char)randNumber, sd);
	}
	cout << endl;
	
	
	return varcharTuple;
}


inline int cRecordGeneratorVar::GenerateInt()
{
	int randNumber;
	//srand(time(NULL));
	
	randNumber = rand() % 10000000000;
	
	return randNumber;
}
