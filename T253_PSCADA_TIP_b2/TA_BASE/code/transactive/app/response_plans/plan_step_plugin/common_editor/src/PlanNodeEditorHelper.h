#pragma once;
#include <vector>

struct TreeItem;

namespace TA_Base_Bus
{
	class IPlanAccess;
}

namespace TA_Base_App
{

class PlanNodeEditorHelper
{
public:
	PlanNodeEditorHelper();
	~PlanNodeEditorHelper();
	std::vector<TreeItem> getRootItems();
	std::vector<TreeItem> getChildItems(unsigned long nodeId);
	std::string getPlanPath(unsigned long planNodeId);
private:
	TA_Base_Bus::IPlanAccess* m_planAccess;
};

};
