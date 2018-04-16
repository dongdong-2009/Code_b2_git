#pragma once;
#include <vector>
#include <map>
#include <boost/noncopyable.hpp>

struct ListItem;

namespace TA_Base_Core
{
class IPaTrainDvaMessage;
}

namespace TA_Base_App
{

class PaTrainDvaMsgEditorHelper : boost::noncopyable
{
public:
    PaTrainDvaMsgEditorHelper();
    ~PaTrainDvaMsgEditorHelper();
    std::vector<ListItem> getAllPaTrainDvaMsgItems();
    TA_Base_Core::IPaTrainDvaMessage* getPaTrainDvsMsgByKey(unsigned long key);
private:
    typedef std::map<unsigned long, TA_Base_Core::IPaTrainDvaMessage*> PaTrainDvaMsgMap;
    PaTrainDvaMsgMap m_paTrainDvaMsgMap;
};

}
