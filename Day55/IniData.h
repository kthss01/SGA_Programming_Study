#pragma once

#include "SIngletonBase.h"

/*
	ini ������
	��ܶ�
		Ű
			������ �Ǿ�����
*/

struct tagIniData {
	const char* section;
	const char* key;
	const char* value;
};

class IniData : public SingletonBase<IniData>
{
private:
	vector<tagIniData> _vIniData;
public:
	IniData();
	~IniData();

	void AddData(const char* section,
		const char* key, const char* value);
	void SaveData(const char* fileName);
	
	char* LoadDataString(const char* fileName,
		const char* section, const char* key);
	int LoadDataInteger(const char* fileName,
		const char* section, const char* key);
};

#define INIDATA IniData::GetSingleton()