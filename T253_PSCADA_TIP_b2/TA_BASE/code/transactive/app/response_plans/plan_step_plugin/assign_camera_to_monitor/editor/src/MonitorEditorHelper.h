#pragma once;

#include "app/response_plans/plan_step_plugin/common_editor/src/TreeItemCache.h"
#include <vector>

struct TreeItem;

namespace TA_Base_App
{

class MonitorEditorHelper
{
public:
	static const unsigned int INVALID_ID;
	MonitorEditorHelper();
	~MonitorEditorHelper();
	std::vector<TreeItem> getRootMonitorItems();
	std::vector<TreeItem> getChildMonitorItems(unsigned int id);
	unsigned int getItemIdByMonitorKey(unsigned int key);
	unsigned int getMonitorKeyByItemId(unsigned int id);
	TreeItem getMonitorTreeItemById(unsigned int id);
private:
	void initItems();
	struct Equal
	{
		bool operator()(const unsigned int& i, const unsigned int& j) const
		{
			return i == j;
		}
	};
	typedef TreeItemCache<unsigned int> MonitorItems;
	static MonitorItems s_monitorItems;
	static bool s_inited;
};

}
