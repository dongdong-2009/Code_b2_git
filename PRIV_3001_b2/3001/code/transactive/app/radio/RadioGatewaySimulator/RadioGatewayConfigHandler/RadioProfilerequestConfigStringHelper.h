#pragma once
#include "StdAfx.h"

#include <vector>

class RadioProfilerequestConfigStringHelper
{
	public:
		
		RadioProfilerequestConfigStringHelper();
		~RadioProfilerequestConfigStringHelper(void);
		
		void getAllTalkGroups (std::vector<std::string> & vctrProfileConfig);

	private:
		std::vector<std::string> m_vctrProfileConfig;
		
};
