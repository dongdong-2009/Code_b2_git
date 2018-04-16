#pragma once;
#include <vector>
#include <map>
#include <boost/noncopyable.hpp>

struct ListItem;

namespace TA_Base_Core
{
class IPaDvaMessage;
}

namespace TA_Base_App
{

class PaDvaMsgEditorHelper : boost::noncopyable
{
public:
    PaDvaMsgEditorHelper();
    ~PaDvaMsgEditorHelper();
    std::vector<ListItem> getAllPaDvaMsgItems();
    std::vector<ListItem> getPaDvaMsgItemsByLocation(unsigned long locationkey);
    TA_Base_Core::IPaDvaMessage* getPaDvsMsgByKey(unsigned long key);
private:
    typedef std::map<unsigned long, TA_Base_Core::IPaDvaMessage*> PaDvaMsgMap;
    PaDvaMsgMap m_paDvaMsgMap;
};

}