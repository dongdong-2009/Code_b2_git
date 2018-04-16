#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif


#include "VideoDisplayManagerPresenter.h"
#include "SequenceEditorDlg.h"
#include "IMonitorAssigner.h"
#include "ViTriggeringMap.h"
#include "Configuration.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/entity_access/src/DisplayManagerEntityData.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::TransactiveException;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using std::vector;

namespace TA_IRS_App
{
VideoDisplayManagerPresenter::VideoDisplayManagerPresenter(ILoadStationDiagramService* loadStationDiagramService) :
        m_currentSeqID(0),
        m_seqEditorView(NULL),
        m_localMonitor(NULL)
{
    m_LoadStationDiagramService = loadStationDiagramService;
    m_hightedMonitorId  = 0;
    m_prvHilitedMonitorID  = 0;
    m_localMonitorKey = 0l;
    m_mouseTracking = false;
    m_contextActive = false;
    m_isAssignedBySchematic = false;
    m_currentSelectedMonitor = NULL;
    m_agentService = & (SwitchAgentCommunicator::getInstance());
}

VideoDisplayManagerPresenter::~VideoDisplayManagerPresenter(void)
{
}

void VideoDisplayManagerPresenter::SetIView(IGFVideoSwitchDisplayView * view)
{
    m_IView = view;
}

void VideoDisplayManagerPresenter::setMonitorAssigner(IMonitorAssigner* virtualMonitorAssigner)
{
    m_monitorAssigner = virtualMonitorAssigner;
}

void VideoDisplayManagerPresenter::SetSeqEditorView(ISequenceEditorView* seqEditorview)
{
    m_seqEditorView = seqEditorview;
}

void VideoDisplayManagerPresenter::setSwtichAgentService(ISwitchAgentService * agentService)
{
    m_agentService = agentService;
}

void VideoDisplayManagerPresenter::closeSchematic()
{
    if (m_LoadStationDiagramService != NULL)
    {
        m_LoadStationDiagramService->closeSchematic();
        delete m_LoadStationDiagramService;
    }
}

void VideoDisplayManagerPresenter::stopSchematic()
{
    if (m_LoadStationDiagramService != NULL)
    {
        m_LoadStationDiagramService->stopSchematic();
    }
}

void VideoDisplayManagerPresenter::refreshSchematic()
{
    if (m_LoadStationDiagramService != NULL)
    {
        m_LoadStationDiagramService->refreshSchematic();
    }
}

void VideoDisplayManagerPresenter::loadStationDiagram(unsigned long level)
{
#ifndef LOCAL_TEST
    unsigned long stationId = m_IView->GetStationIdForGwax();
    if (0 < stationId)
    {
        std::string fullPath;
        DisplayItemManager::getInstance().getGraphicfullPath(stationId, fullPath, level);
        if (!fullPath.empty())
        {
            CRect rect ;
            m_IView->GetSchematicCRET(rect);
            HWND hwnd = m_IView->GetWndHandle();
            m_LoadStationDiagramService->stopSchematic();
            m_LoadStationDiagramService->loadStationDiagram(fullPath , rect , hwnd);
        }
    }
#endif
}

void VideoDisplayManagerPresenter::editSequence(unsigned long seqId)
{
    if (seqId > 0)
    {
        unsigned long stationId = m_IView->GetSelectedStation();
        Sequence* sequence = DisplayItemManager::getInstance().getSequence(seqId);
        if (!sequence->getReadOnly())
        {
            std::auto_ptr<SequenceEditorDlg> seqEditorView(new SequenceEditorDlg());
            SetSeqEditorView(seqEditorView.get());
            seqEditorView->SetPresenter(this);
            if (sequence != NULL)
            {
                m_currentSeqID = seqId;
                seqEditorView->Editsequence(sequence, stationId);
                m_currentSeqID = 0;
            }
            SetSeqEditorView(NULL);
        }
    }
}

void VideoDisplayManagerPresenter :: SelectStation()
{
    unsigned long stationId = m_IView->GetSelectedStation();
    if (0 < stationId)
    {
        std::vector<VideoInput*> seqs;
        DisplayItemManager::getInstance().getSequenceByLocation(stationId , seqs);
        m_IView->DisplaySequences(seqs);
        m_IView->EnableEditSeqButton(false);
    }
}

void VideoDisplayManagerPresenter::SelectSequence(unsigned long sequenceId)
{
    if (sequenceId == 0)
    {
        m_IView->EnableEditSeqButton(false);
        return;
    }
    Sequence* seq = DisplayItemManager::getInstance().getSequence(sequenceId);
    if (seq->getReadOnly())
    {
        m_IView->EnableEditSeqButton(false);
    }
    else
    {
        std::vector < MonitorDisplayItem *> ::iterator it = m_monitorDisplayItems.begin();
        for (it; it != m_monitorDisplayItems.end(); it++)
        {
            MonitorDisplayItem * item = dynamic_cast<MonitorDisplayItem*>(*it);
            if (item->inputIsSequence() && item->getMonitor()->getInput()->getKey() == sequenceId)
            {
                m_IView->EnableEditSeqButton(false);
                break;
            }
        }
        if (it == m_monitorDisplayItems.end())
        {
            m_IView->EnableEditSeqButton(true);
        }
    }
    m_IView->DisplaySequence(seq);
}

void VideoDisplayManagerPresenter::saveSequenceConfig(Sequence* sequence, unsigned short dwellTime, const std::list< Camera* > cameraInputs)
{
    sequence->setDwellTime(dwellTime);
    sequence->setCameras(cameraInputs);
    DisplayItemManager::getInstance().saveSequencesRelationship(sequence->getKey());
    DisplayItemManager::getInstance().updateSequence(sequence);
    DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(sequence);
    m_IView->DisplaySequence(sequence);
}

void VideoDisplayManagerPresenter::setSequenceDescription(Sequence* sequence,
                                                          const std::string& newDescription)
{
    //sequence->setDescription(newDescription);
    sequence->setSequenceDescription(newDescription);
    DisplayItemManager::getInstance().updateSequence(sequence);
    DisplayItemManager::getInstance().saveSequencesRelationship(sequence->getKey());
    TA_Base_Core::Sequence baseSeq(sequence->getKey());
    baseSeq.setSequenceDescription(newDescription);
    m_agentService->setSequenceDescription(sequence , newDescription);
    //Sequence* seq =  DisplayItemManager::getInstance().getSequence(sequence->getKey());
    m_IView->DisplaySequence(sequence);
    m_IView->UpdateSequenceTreeCtrl(sequence);
}

void VideoDisplayManagerPresenter::MainUILButtonDown(CPoint& point)
{
    CPoint nullPt(0, 0);
    CPoint negPt(-1, -1);
    if (point == nullPt)
    {
        m_mouseTracking = true;
        m_IView->beginDragImage();
    }
    else
    {
        if (point == negPt)
        {
            if ((m_hightedMonitorId != 0) && (point == negPt))
            {
                m_prvHilitedMonitorID = m_hightedMonitorId;
                m_hightedMonitorId = 0;
                updateMonitorView();
            }
            return ;
        }
        MonitorDisplayItem* item = getOverMonitor(point);
        if (item != NULL)
        {
            unsigned long temp = item->getMonitor()->getKey();
            m_prvHilitedMonitorID = m_hightedMonitorId;
            m_hightedMonitorId = item->getMonitor()->getKey();
            updateMonitorView();
        }
    }
}

void VideoDisplayManagerPresenter::updateMonitorView()
{
    invalidateControlLabels();
    if (m_prvHilitedMonitorID > 0)
    {
        MonitorDisplayItem* prvItem = getMonitor(m_prvHilitedMonitorID);
        if (prvItem != NULL)
        {
            m_IView->paintStandControl(prvItem);
        }
    }
    if (m_hightedMonitorId > 0)
    {
        MonitorDisplayItem* hilitedItem = getMonitor(m_hightedMonitorId);
        if (hilitedItem != NULL)
        {
            m_IView->paintHightedControl(hilitedItem);
        }
    }
}

MonitorDisplayItem* VideoDisplayManagerPresenter::getOverMonitor(CPoint& point)
{
    std::vector < MonitorDisplayItem *> ::iterator it = m_monitorDisplayItems.begin();
    for (it; it != m_monitorDisplayItems.end(); it++)
    {
        MonitorDisplayItem* item = *it;
        if (item->isInRect(point))
        {
            return item;
        }
    }
    return NULL ;
}

MonitorDisplayItem* VideoDisplayManagerPresenter::getMonitor(unsigned long monitorId)
{
    std::vector< MonitorDisplayItem *>::iterator it = m_monitorDisplayItems.begin();
    for (it; it != m_monitorDisplayItems.end(); it++)
    {
        MonitorDisplayItem* item = *it;
        if (item ->getMonitor()->getKey() == monitorId)
        {
            return item;
        }
    }
    return NULL ;
}

void VideoDisplayManagerPresenter::MainUILButtonUp(CPoint& point)
{
    if (TRUE == m_mouseTracking)
    {
        m_IView->endDrage();
        m_mouseTracking = FALSE;
        m_contextActive = FALSE;
        MonitorDisplayItem *monItem = getOverMonitor(point);
        if (monItem != NULL)
        {
            VideoInput* input = m_IView->getSelectedVideoSource();
            if (NULL != input)
            {
                Sequence* seq = dynamic_cast<Sequence*>(input);
                if (seq != NULL && (false == seq->getReadOnly()))
                {
                    std::list< Camera* > cameraList = seq->getCameras();
                    if (cameraList.empty())
                    {
                        m_IView->displayErrorMessage(CString("请确认该序列已分配了摄像机"));
                        return;
                    }
                }
                if (!assignMonitor(monItem->getMonitor(), input))
                {
                    return;
                }
            }
        }
    }
    else
    {
        MonitorDisplayItem *monItem = getOverMonitor(point);
        if (NULL != monItem)
        {
            handleHighLightMonitor(monItem);
        }
    }
}

void VideoDisplayManagerPresenter::handleHighLightMonitor(MonitorDisplayItem * item)
{
    m_currentSelectedMonitor = item;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "item->inputIsCamera() = %d ,pkey=%ld,address:%s,supporeMovement:%d",
                item->inputIsCamera(), NULL != item->getCamera() ? item->getCamera()->getKey() : -1,
                NULL != item->getCamera() ? item->getCamera()->getAddress().c_str() : "--",
                NULL != item->getCamera() ? item->getCamera()->getIsMovementSupported() : -1);
    if (item->inputIsCamera() && NULL != item->getCamera() && item->getCamera()->getIsMovementSupported())
    {
        m_IView->enablePTZButton(TRUE);
    }
    else
    {
        m_IView->enablePTZButton(FALSE);
    }
    m_IView->refreshIndexLable(item->getIndex());
    item->getLable()->Invalidate();
    m_IView->paintHightedControl(item);
}


void VideoDisplayManagerPresenter::handleVideoSwitchException(TA_Base_Bus_GZL9::VideoSwitchAgentException& te)
{
    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::VideoSwitchException", te.what.in());
    CString message;
    message.Format("显示控制服务错误");
    m_IView->displayErrorMessage(message);
}

void VideoDisplayManagerPresenter::handleTransactiveException(TA_Base_Core::TransactiveException& te)
{
    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
    CString errorNumber1 = "UE-090047";
    CString errorNumber2 = "UE-090048";
    CString errorNumber3 = "IDS_UE_090083";
    CString errorNumber4 = "UE-020056";
    CString errorNumber5 = "UE-010021";
    CString errorNumber6 = "IDS_UE_090089";
    CString errorNumber7 = "IDS_UE_090090";
    CString errorNumber8 = "UE-090037";
    CString message;
    message.Format("显示控制错误。请检查VideoSwitchAgent是否运行正常。");
    TA_Base_Bus::TransActiveMessage userMsg;
    if (te.what() == errorNumber1)
    {
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090047);
    }
    else if (te.what() == errorNumber2)
    {
        userMsg << message;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090048);
    }
    else if (te.what() == errorNumber3)
    {
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090083);
    }
    else if (te.what() == errorNumber4)
    {
        userMsg << message;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_020056);
    }
    else if (te.what() == errorNumber5)
    {
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
    }
    else if (te.what() == errorNumber6)
    {
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090089);
    }
    else if (te.what() == errorNumber7)
    {
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090090);
    }
    else if (te.what() == errorNumber8)
    {
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090037);
    }
    m_IView->displayErrorMessage(message);
}

void VideoDisplayManagerPresenter::MainUIMouseMove(CPoint& point)
{
    if (m_mouseTracking)
    {
        m_IView->setDragIcon();
        MonitorDisplayItem* item = getOverMonitor(point);
        unsigned long hile = m_hightedMonitorId;
        unsigned long prev = m_prvHilitedMonitorID;
        if (item  == NULL)
        {
            m_prvHilitedMonitorID = m_hightedMonitorId;
            m_hightedMonitorId = 0;
        }
        else
        {
            m_prvHilitedMonitorID = m_hightedMonitorId;
            m_hightedMonitorId = item->getMonitor()->getKey();
        }
        if ((hile != m_hightedMonitorId) || (prev != m_prvHilitedMonitorID))
        {
            m_contextActive = TRUE;
            m_IView->handleDragImageOnMouseMove(true , point);
            updateMonitorView();
        }
        else if (m_hightedMonitorId == 0 && m_prvHilitedMonitorID == 0)
        {
            m_contextActive = TRUE;
            m_IView->handleDragImageOnMouseMove(true , point);
            updateMonitorView();
        }
        else
        {
            m_IView->handleDragImageOnMouseMove(false , point);
            m_contextActive = FALSE;
        }
    }
}

void VideoDisplayManagerPresenter::MainUIContextMenu(CPoint& point)
{
    if ((TRUE == m_contextActive) || (TRUE == m_mouseTracking))
    {
        return;
    }
    MonitorDisplayItem* item = getOverMonitor(point);
    if (item == NULL)
    {
        return;
    }
    bool isLocalMonitor = (m_localMonitorKey == item->getMonitor()->getKey());
    updateMonitorView();
    if (item->inputIsSequence())
    {
        m_IView->loadSequenceSubMenu(point , item->getMonitor(), item->getSequence() , isLocalMonitor);
    }
    else if (item->inputIsCamera())
    {
        m_IView->loadCameraSubMenu(point , item->getMonitor() , isLocalMonitor);
    }
}

MonitorDisplayItem * VideoDisplayManagerPresenter::getMonitorDisplayItemByIndex(unsigned short index)
{
    std::vector < MonitorDisplayItem *>::iterator it = m_monitorDisplayItems.begin();
    for (it; it != m_monitorDisplayItems.end(); ++it)
    {
        MonitorDisplayItem * item = (*it);
        if (item->getIndex() == index)
        {
            return item;
        }
    }
    return NULL;
}

//RPARAM_DISPLAYMONITOR==camera name;monitor index( 0-8)
void VideoDisplayManagerPresenter::onRunParamChange(const std::string& name, const std::string& value)
{
    FUNCTION_ENTRY("onRunParamChange");
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "onRunParamChange( name = %s, value = %s)", name.c_str() , value.c_str());
    if (name == RPARAM_DISPLAYMONITOR)
    {
        std::vector<std::string> strs;
        boost::split(strs, value, boost::is_any_of(";"));
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Param name is correct");
        if (strs.size() == 2)
        {
            std::string cameraAddress = strs[0];
            std::string monitorIndex = strs[1];
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Parameter number is correct");
            unsigned short index = -1;
            try
            {
                index = boost::lexical_cast<unsigned short> (monitorIndex);
            }
            catch (boost::bad_lexical_cast &)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "wrong monitor index format %s ", monitorIndex.c_str());
                return;
            }
            Camera* ca = DisplayItemManager::getInstance().getCameraByAddress(cameraAddress);
            if (ca != NULL)
            {
                MonitorDisplayItem * item =  getMonitorDisplayItemByIndex(index);
                if (item != NULL)
                {
                    if (!assignMonitor(item->getMonitor(), ca))
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "fail to assign camera(%s) to monitor %ld with index %d",
                                    cameraAddress.c_str() , item->getMonitor()->getKey(), item->getIndex());
                        return;
                    }
                    m_prvHilitedMonitorID = m_hightedMonitorId;
                    m_hightedMonitorId = item->getMonitor()->getKey();
                    updateMonitorView();
                    handleHighLightMonitor(item);
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "onRunParamChange is done ");
                }
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "can not find the camera with the address %s ", cameraAddress.c_str());
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Incorect parameter number.(%d)", strs.size());
        }
    }
    else if (name == RPARAM_TRIGGER_ENTITY)
    {
        unsigned long triggeringEntity = atol(value.c_str());
        try
        {
            ViTriggeringMap t(triggeringEntity);
            unsigned long cameraEntityKey = t.getCameraEntityKey();
            Camera* camera = DisplayItemManager::getInstance().getCamera(cameraEntityKey);
            if (NULL == camera)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Can't found the camera %lu", cameraEntityKey);
                FUNCTION_EXIT;
                return;
            }
            // Get the monitor address if set
            std::string monitorId = "0";
            std::string paramMonitorId = RunParams::getInstance().get(RPARAM_MONITOR_ID);
            if (!paramMonitorId.empty())
            {
                monitorId = paramMonitorId;
            }
            std::string displayParamValue = camera->getAddress() + ";" + monitorId;
            RunParams::getInstance().set(RPARAM_DISPLAYMONITOR, displayParamValue.c_str());
        }
        catch (const TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
        }
    }
    else if (name == RPARAM_CLEAR_MONITOR)
    {
        int monitorIndex = atoi(value.c_str());
        MonitorDisplayItem * item =  getMonitorDisplayItemByIndex(monitorIndex);
        if (item != NULL)
        {
            clearMonitor(item->getMonitor());
        }
    }
    else if (name == "DisplaySeq") //add for support test
    {
        std::vector<std::string> strs;
        boost::split(strs, value, boost::is_any_of(";"));
        if (strs.size() == 2)
        {
            std::string seqName = strs[0];
            unsigned short monindex = boost::lexical_cast<unsigned short> (strs[1]);
            MonitorDisplayItem* mon = getMonitorDisplayItemByIndex(monindex);
            Sequence* seq = DisplayItemManager::getInstance().getSequence(seqName);
            if (NULL != mon && NULL != seq)
            {
                assignMonitor(mon->getMonitor(), seq);
            }
        }
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown Param name");
    }
    FUNCTION_EXIT;
}

Camera * VideoDisplayManagerPresenter::getSelectedCamera()
{
    return m_currentSelectedMonitor->getCamera();
}

BOOL VideoDisplayManagerPresenter::isCameraSelected()
{
    if (m_currentSelectedMonitor == NULL)
    {
        return FALSE;
    }
    return m_currentSelectedMonitor->inputIsCamera();
}

void VideoDisplayManagerPresenter::selectCameraFromSequenceList()
{
}

void VideoDisplayManagerPresenter::beginDrage(CPoint & point , CImageList * image)
{
    m_IView->setDragAdjust(point);
    m_IView->setDragImage(image);
}

void VideoDisplayManagerPresenter::loadMonitorRelationship()
{
	/*
    unsigned long pkey;
    int typekey;
    std::vector < MonitorDisplayItem *>::iterator item_it;
    for (item_it = m_monitorDisplayItems.begin(); item_it != m_monitorDisplayItems.end(); item_it++)
    {
        MonitorDisplayItem* monItem = dynamic_cast<MonitorDisplayItem*>(*item_it);
        if (NULL == monItem)
        {
            continue;
        }
        DisplayItemManager::getInstance().loadEntityRelationship(monItem->getMonitor()->getKey(), pkey, &typekey);
        if (0 == pkey)
        {
            continue;
        }
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "load Monitor.Monitor key:[%ld],item key:[%ld].", monItem->getMonitor()->getKey(), pkey);
        VideoInput* input = DisplayItemManager::getInstance().getInput(pkey);
        if (input != NULL)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Camera item key:(%ld) is not empty.", pkey);
            if ((monItem->getMonitor()->getInput() != NULL &&  monItem->getMonitor()->getInput()->getKey() == pkey))
            {
                continue;
            }
            if (m_localMonitorKey == monItem->getMonitor()->getKey() || monItem->getMonitor()->isQuadMonitor())
            {
                assignMonitor(monItem->getMonitor(), input);
            }
            else
            {
                monItem->getMonitor()->assignInput(input);
                m_IView->updateMonitor(monItem);
            }
        }
    }
	*/
}

void VideoDisplayManagerPresenter::CheckSequeceUpdate(VideoSourcesTreeCtrl* sequenceTreeCtrl)
{
    std::map<std::string, Sequence*> SeqMap = DisplayItemManager::getInstance().getAllSequences();
    std::map<std::string, Sequence*>::iterator it = SeqMap.begin();
    CString itemName, itemText, itemDes;
    HTREEITEM hitem;
    for (; it != SeqMap.end(); it++)
    {
        Sequence* seq = dynamic_cast<Sequence*>((*it).second);
        itemName.Format("%s", seq->getName().c_str());
        hitem = sequenceTreeCtrl->findItemFromSubstr(itemName);
        if (hitem == NULL)
        {
            continue;
        }
        itemText = sequenceTreeCtrl->GetItemText(hitem);
        int pos = itemText.Find(" - ");
        if (pos < 0)
        {
            continue;
        }
        itemDes.Format("%s", seq->getSequenceDescription().c_str());
        CString itemRight = itemText.Right(itemText.GetLength() - pos - 3);
        if (itemRight != itemDes)
        {
            sequenceTreeCtrl->videoInputUpdated(seq);
        }
    }
}

void VideoDisplayManagerPresenter::invalidateControlLabels()
{
    FUNCTION_ENTRY("invalidateControlLabels");
    if (m_prvHilitedMonitorID > 0)
    {
        MonitorDisplayItem* prvItem = getMonitor(m_prvHilitedMonitorID);
        if (prvItem != NULL)
        {
            prvItem->getLable()->Invalidate();
            m_IView->refreshIndexLable(prvItem->getIndex());
        }
    }
    if (m_hightedMonitorId > 0)
    {
        MonitorDisplayItem* hilitedItem = getMonitor(m_hightedMonitorId);
        if (hilitedItem != NULL)
        {
            hilitedItem->getLable()->Invalidate();
            m_IView->refreshIndexLable(hilitedItem->getIndex());
        }
    }
    FUNCTION_EXIT;
}

void VideoDisplayManagerPresenter::monitorAssignChanged(Monitor* monitor)
{
    if (NULL != monitor)
    {
        MonitorDisplayItem* monItem = getMonitor(monitor->getKey());
        if (NULL != monItem)
        {
            m_IView->updateMonitor(monItem);
        }
    }
}
}
