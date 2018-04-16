#pragma once;
#include "app/response_plans/plan_step_plugin/common_editor/src/UniqueListItemCache.h"

struct ListItem;

namespace TA_Base_App
{
	class SchematicStepEditorHelper
	{
	public:
		static const unsigned int INVALID_ID;
		SchematicStepEditorHelper();
		~SchematicStepEditorHelper();
		std::vector<ListItem> getAllItems();
		std::string getSchematicById(unsigned int id);
		unsigned int getIdBySchematic(const std::string& name);
	private:
		UniqueListItemCache m_schematics;
	};
}
