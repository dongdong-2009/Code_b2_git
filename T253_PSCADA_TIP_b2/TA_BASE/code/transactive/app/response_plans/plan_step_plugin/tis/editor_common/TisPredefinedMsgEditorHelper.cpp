#include "TisPredefinedMsgEditorHelper.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "bus/signs/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

namespace TA_Base_App
{
const unsigned int TisPredefinedMsgEditorHelper::INVALID_ID = -1;

TisPredefinedMsgEditorHelper::TisPredefinedMsgEditorHelper(const int type)
    : m_libraryVersion(0)
{
    TA_ASSERT(type == TRAIN || type == STATION, "");
    TA_Base_Bus::TISAgentAccessFactory::StisNamedObj& stisAgent =
        TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent();
    try
    {
        if(type == STATION)
        {
            CORBA_CALL_RETURN(m_libraryVersion, stisAgent, getCurrentCDBSTISMessageLibraryVersion, ());
        }
        else
        {
            CORBA_CALL_RETURN(m_libraryVersion, stisAgent, getCurrentCDBTTISMessageLibraryVersion, ());
        }
    }
    catch(...)
    {
    }
    if(0 != m_libraryVersion)
    {
        TisConfig::PredefinedTisMessageList msgs;
        if(type == STATION)
        {
            msgs = TisConfig::getSTisMessages(m_libraryVersion);
        }
        else
        {
            msgs = TisConfig::getTTisMessages(m_libraryVersion);
        }
        unsigned int id = 1;
        BOOST_FOREACH(const TisConfig::PredefinedTisMessage & i, msgs)
        {
            PredefinedMsgItem item(id++, i.librarySection, i.messageTag, i);
            m_predefinedMsgItems.insert(item);
        }
    }
}

TisPredefinedMsgEditorHelper::~TisPredefinedMsgEditorHelper()
{
}

std::vector<ListItem> TisPredefinedMsgEditorHelper::getPredefinedMsgItems() const
{
    std::vector<ListItem> items;
    BOOST_FOREACH(const Container::value_type & i, m_predefinedMsgItems)
    {
        ListItem tmp = {i.id, i.msg.description};
        items.push_back(tmp);
    }
    return items;
}

TisConfig::PredefinedTisMessage TisPredefinedMsgEditorHelper::getPredefinedMsgById(const unsigned int id) const
{
    TisConfig::PredefinedTisMessage msg;
    Container::const_iterator it = m_predefinedMsgItems.find(id);
    if(m_predefinedMsgItems.end() != it)
    {
        msg = (*it).msg;
    }
    return msg;
}

unsigned long TisPredefinedMsgEditorHelper::getLibraryVersion() const
{
    return m_libraryVersion;
}

unsigned int TisPredefinedMsgEditorHelper::getIdBySectionAndMessageId(const int section, const unsigned short msgId) const
{
    const IndexSectionAndMsgId& tmp = boost::multi_index::get<1>(m_predefinedMsgItems);
    IndexSectionAndMsgId::const_iterator it = tmp.find(boost::make_tuple(section, msgId));
    if(tmp.end() != it)
    {
        return (*it).id;
    }
    return INVALID_ID;
}

TisConfig::PredefinedTisMessage TisPredefinedMsgEditorHelper::getPredefinedMsgBySectionAndMessageId(const int section, const unsigned short msgId ) const
{
	PredefinedMsgItem item;
    const IndexSectionAndMsgId& tmp = boost::multi_index::get<1>(m_predefinedMsgItems);
    IndexSectionAndMsgId::const_iterator it = tmp.find(boost::make_tuple(section, msgId));
	if(tmp.end() != it)
	{
		item = *it;
	}
	return item.msg;
}
}