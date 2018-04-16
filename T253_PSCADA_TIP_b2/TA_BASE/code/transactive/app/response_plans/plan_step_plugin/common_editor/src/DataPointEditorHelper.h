#pragma once;

#include <vector>
#include "app/response_plans/plan_step_plugin/common_editor/src/TreeItemCache.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointValueTypeEditorHelper.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include <ace/RW_Mutex.h>

struct TreeItem;
struct ListItem;

namespace TA_Base_Core
{
	class DataPointEntityData;
}

namespace TA_Base_App
{

class DataPointEditorHelper
{
public:
	static const unsigned int INVALID_ID;
    DataPointEditorHelper( const bool bIdAsValue = false );
    ~DataPointEditorHelper();
    std::string getDisplayName(const std::string& fullName);
    std::vector<TreeItem> getRootDataPointItems();
    std::vector<TreeItem> getChildDataPointItems(const unsigned int id);
    unsigned int getItemIdByDataPointName(const std::string& name);
    std::string getDataPointNameByItemId(const unsigned int id);
	TA_Base_Core::DataPointEntityData* getDataPointEntityById(const unsigned int id);

    std::string getTypeDescription( const int nType );
    void getDataPointInfo( const unsigned int unId, std::string& strName, std::string& strValue, int& nType );

    void getDataPointValues( const unsigned int id, const int nType, const std::string& strValue, std::vector<ListItem>& vecItems, unsigned int& unValueId );
    void getDataPointValues( const std::string& strName, const int nType, const std::string& strValue, std::vector<ListItem>& vecItems, unsigned int& unValueId );
    void getDataPointValue( const unsigned int id, const int nType, const unsigned int unValueId, std::string& strValue, std::string& strDispValue );
    void getDataPointValue( const std::string& strName, const int nType, const unsigned int unValueId, std::string& strValue, std::string& strDispValue );
    void getDataPointDisplayValue( const unsigned int id, const int nType, const std::string& strValue, std::string& strDispValue );
    void getDataPointDisplayValue( const std::string& strName, const int nType, const std::string& strValue, std::string& strDispValue );

private:
    std::string getLabel(TA_Base_Core::DataPointEntityData& refData, const bool bValue);

private:
    struct DpNameEqual
    {
        bool operator()(TA_Base_Core::IEntityData& entityData, const std::string& name) const
        {
            return entityData.getName() == name;
        }
    };

    static TreeItemCache<TA_Base_Core::IEntityData> s_dataPoints;
	static ACE_RW_Mutex s_mtxDataPoints;
    DataPointValueTypeEditorHelper m_oValueTypeHelper;
	bool m_bIdAsValue;
};

}
