#include "cColumn.h"


class cRecordGeneratorFix
{
public:
	std::vector<cColumn*>*columns;
	cSpaceDescriptor *columnSD;

	int index = 0;


	cRecordGeneratorFix(vector<cColumn*>*mColumns, cSpaceDescriptor *cSD);
	bool LoadGenerator(vector<cColumn*>*mColumns, cSpaceDescriptor *cSD);
	cTuple* CreateNewRecord();
	cNTuple* GenerateVarchar(int size, cSpaceDescriptor *sd);
	static int GenerateInt();
};


inline cRecordGeneratorFix::cRecordGeneratorFix(vector<cColumn*>*mColumns, cSpaceDescriptor *cSD)
{
	LoadGenerator(mColumns, cSD);
}


inline bool cRecordGeneratorFix::LoadGenerator(vector<cColumn*>* mColumns, cSpaceDescriptor *cSD)
{
	columns = mColumns;
	columnSD = cSD;

	return true;
}


inline cTuple* cRecordGeneratorFix::CreateNewRecord()
{
	cTuple* record = new cTuple(columnSD);

	

	


	int positionInTable;
	for (int i = 0; i < columns->size(); i++)
	{
		if (columns->at(i)->cType->GetCode() == 'i')
		{
			if (columns->at(i)->primaryKey)
			{
				record->SetValue(i, index, columnSD);
				index++;
			}
			else
			{
				int genInt = GenerateInt();
				record->SetValue(i, genInt, columnSD);
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


inline cNTuple* cRecordGeneratorFix::GenerateVarchar(int size, cSpaceDescriptor * sd)
{

	int randNumber;
	//srand(time(NULL));
	cNTuple * varcharTuple = new cNTuple(sd);

	for (int i = 0; i < size / 2; i++)
	{
		randNumber = rand() % 94 + 20;
		varcharTuple->SetValue(i, (char)randNumber, sd);
	}



	return varcharTuple;
}


inline int cRecordGeneratorFix::GenerateInt()
{
	int randNumber;
	//srand(time(NULL));

	randNumber = rand() % 100000;
	

	return randNumber;
}
