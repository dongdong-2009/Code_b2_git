#pragma once;

#include <vector>
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"

namespace TA_Base_App
{
	class DataPointTestTypeEditorHelper
	{
	public:
		DataPointTestTypeEditorHelper();
		~DataPointTestTypeEditorHelper();
		std::vector<ListItem> getItems( const int eDpType );
		bool isValidTestType(int type);
		std::string getTypeDescription(int type);
	private:
		static ListItem s_items[];
	};
}