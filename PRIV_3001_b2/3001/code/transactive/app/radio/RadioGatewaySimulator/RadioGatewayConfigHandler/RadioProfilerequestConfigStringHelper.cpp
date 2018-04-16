#include "stdafx.h"
#include "RadioProfilerequestConfigStringHelper.h"#
#include <vector>

#define INSERT_ITEM		"INSERT"
#define DELETE_ITEM		"DELETE"
#define EDIT_ITEM		"EDIT"

#define EQUAL_STRING	0


RadioProfilerequestConfigStringHelper::RadioProfilerequestConfigStringHelper()
{
	
}

RadioProfilerequestConfigStringHelper::~RadioProfilerequestConfigStringHelper(void)
{
	
}

void RadioProfilerequestConfigStringHelper::getAllTalkGroups (std::vector<std::string> & vctrProfileConfig)
{
	std::vector <std::string>::iterator iterItem = vctrProfileConfig.begin();
	std::vector <std::string> vctrTemp;
	
	for (;iterItem != vctrProfileConfig.end(); iterItem++)
	{
		if ((*iterItem).compare(INSERT_ITEM) == EQUAL_STRING ||
			(*iterItem).compare(DELETE_ITEM) == EQUAL_STRING ||
			(*iterItem).compare(EDIT_ITEM) == EQUAL_STRING)
		{
			continue;
		}
		else
		{
			vctrTemp.push_back(*iterItem);
		}
	}

	vctrProfileConfig.clear();

	vctrProfileConfig = vctrTemp;

}