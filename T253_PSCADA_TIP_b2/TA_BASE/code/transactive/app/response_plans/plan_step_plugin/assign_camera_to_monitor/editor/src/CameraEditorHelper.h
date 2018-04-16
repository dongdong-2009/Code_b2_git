#pragma once;

#include "app/response_plans/plan_step_plugin/common_editor/src/TreeItemCache.h"
#include <vector>

struct TreeItem;

namespace TA_Base_App
{

class CameraEditorHelper
{
public:
	static const unsigned int INVALID_ID;
	CameraEditorHelper();
	~CameraEditorHelper();
	std::vector<TreeItem> getRootCameraItems();
	std::vector<TreeItem> getChildCameraItems(unsigned int id);
	unsigned int getItemIdByCameraKey(unsigned int key);
	unsigned int getCameraKeyByItemId(unsigned int id);
	TreeItem getCameraTreeItemById(unsigned int id);
private:
	void initItems();
	struct Equal
	{
		bool operator() (const unsigned int& i, const unsigned int& j) const
		{
			return i == j;
		}
	};
	typedef TreeItemCache<unsigned int> CameraItems;
	static CameraItems s_cameraItems;
	static bool s_inited;
};

}