#pragma once;

#include <vector>
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/DataPointEnum.h"

namespace TA_Base_App
{
	class DataPointValueTypeEditorHelper
	{
	public:
		DataPointValueTypeEditorHelper();
		~DataPointValueTypeEditorHelper();
		static std::vector<ListItem> getItems();
		static bool isValidType(int type);
		static std::string getTypeDescription(int type);
		static std::string getTypeDescriptionByScadaType(const std::string& scadaType);
		static EDataPoint getTypeByScadaType(const std::string& scadaType);
	private:
		static ListItem s_items[];
	};
}