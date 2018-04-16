/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataNodeListItem.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * A DataNode (Equipment) item to be displayed in (or filtered from) the EquipmentStatusListCtrl.
  * This class contains the required datanode proxy.
  *
  */

#include "stdafx.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/TAAssert.h"
#include "Equipmentstatusviewer.h"
#include "DataNodeListItem.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/utilities/src/DateFormat.h" 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{
    
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    DataNodeListItem::DataNodeListItem(unsigned long entitykey , TA_Base_Bus::ScadaProxyFactory& proxyFactory, 
									   HWND hwndToNotify)
        : EquipmentStatusListItem(entitykey, hwndToNotify),
		  m_dataNode(NULL)
    {
        //  This may throw a ScadaProxyException so be prepared to catch it.
        //  If we can't createDataNodeProxy() then try reallocating this DataNodeListItem later.
		m_dataNode = new TA_Base_Bus::DataNodeProxySmartPtr();
        proxyFactory.createDataNodeProxy ( entitykey, *this, (*m_dataNode), false);
		
		if ((*m_dataNode)->getChildDataPointEntities().size() == 0)
		{
			m_updateType = DELETE_ITEM;
		}
		
		initStaticColumn();
    }

    DataNodeListItem::DataNodeListItem(boost::shared_ptr<TA_Base_Core::DataNodeEntityData> entityData,
										TA_Base_Bus::ScadaProxyFactory& proxyFactory, 
										HWND hwndToNotify)
        : EquipmentStatusListItem(entityData->getKey(), hwndToNotify),
		  m_dataNode(NULL)
    {
        //  This may throw a ScadaProxyException so be prepared to catch it.
        //  If we can't createDataNodeProxy() then try reallocating this DataNodeListItem later.
        m_dataNode = new TA_Base_Bus::DataNodeProxySmartPtr();
		proxyFactory.createDataNodeProxy ( entityData, *this, (*m_dataNode), false);

		if ((*m_dataNode)->getChildDataPointEntities().size() == 0)
		{
			m_updateType = DELETE_ITEM;
		}

		initStaticColumn();
    }

    DataNodeListItem::~DataNodeListItem()
    {
		TA_Base_Bus::DataNodeProxySmartPtr* dnProxy = m_dataNode;

		{
			TA_Base_Core::ThreadGuard guard( m_lock );
			m_dataNode = NULL;
		}
		
		if(dnProxy != NULL)
		{
			delete dnProxy;
		}
    }


    CString DataNodeListItem::buildNewColumnText( TA_Base_App::EslColumns column )
    {
        //  Get the new text for the requested column. By calling this function 
        //  for each column we need only one try-catch(ScadaProxyException) block.

        CString newText("");
        try
        {
            switch (column)
            {
            case STATUS_COLUMN:
                {
                    newText= buildSummaryText();
                    break;
                }
            case MODIFIED_BY_COLUMN:
                {
                    if (!getColumnText(STATUS_COLUMN).IsEmpty() )
                    {                        
                        std::string sessionId = (*m_dataNode)->getLastStatusChangeSessionId();
                        if (!sessionId.empty())
                        {
                            newText = getInfoFromSession(sessionId);
                        }
                    }
                    break;
                }
            /*case TIMESTAMP_COLUMN:
                {
                    if (!getColumnText(STATUS_COLUMN).IsEmpty() )
                    {                        
                        m_timeStamp = (*m_dataNode)->getLastStatusChangeTimestamp();
                        if (0L != m_timeStamp)
                        {
                            CTime displayTime( m_timeStamp);
		                    TA_Base_Core::DateFormat dateFormat; 
		                    CString formatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4).c_str(); 
                            newText = displayTime.Format(formatStr.operator LPCTSTR());
                        }
                    }
                    break;
                }*/
            default:
                {
                    //  Requested column uses a field shared between DataNodes and DataPoints
                    newText = EquipmentStatusListItem::buildNewColumnText( column);
                }
            }
        }
        catch (TA_Base_Core::ScadaProxyException& )
        {
            //  The proxy data for this column is not ready yet
        }
        return newText;
    }


    CString DataNodeListItem::buildSummaryText() 
    {
		int numSummaries = 0;
		CString summaryTxt("");

        //firstly, check the tag state
		try
		{
            TA_Base_Bus::ETagState tagState = (*m_dataNode)->getTaggingState();
            if (TA_Base_Bus::NotTagged != tagState)
            {
                summaryTxt = ( TA_Base_Bus::Tagged == tagState)?FilterString[TAGGED_FILTERSTRING]:FilterString[TAGGED_PTW_FILTERSTRING];
                summaryTxt += FilterString[STATUS_SEPARATOR_FILTERSTRING];
            }
        }
		catch (...)
		{
		}

		try
		{
			if ( (*m_dataNode)->getControlInhibitSummary().value)
			{
				summaryTxt +=  FilterString[CONTROL_FILTERSTRING]; //  "Control";
				numSummaries++;
			};
		}
		catch (...)
		{
		}

		try
		{
			if ((*m_dataNode)->getScanInhibitSummary().value)
			{
				if (numSummaries == 0)
					summaryTxt += FilterString[SCAN_FILTERSTRING]; //  "Scan"
				else
					summaryTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[SCAN_FILTERSTRING]; //  ", Scan"
				numSummaries++;
			}
		}
		catch (...)
		{
		}

		try
		{
			if ((*m_dataNode)->getAlarmInhibitSummary().value)
			{
				if (numSummaries == 0)
					summaryTxt += FilterString[ALARM_FILTERSTRING]; //  "Alarm";
				else
					summaryTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[ALARM_FILTERSTRING]; // ", Alarm";
				numSummaries++;
			}
		}
		catch (...)
		{
		}

		try
		{
			if ((*m_dataNode)->getMmsAlarmInhibitSummary().value)
			{
				if (numSummaries == 0)
					summaryTxt += FilterString[MMS_FILTERSTRING]; //  "MMS";
				else
					summaryTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[MMS_FILTERSTRING]; //  ", MMS";
				numSummaries++;
			}
		}
		catch (...)
		{
		}

		if (numSummaries != 0)
			summaryTxt += FilterString[INHIBITED_FILTERSTRING] + FilterString[SUMMARY_FILTERSTRING]; //  " Inhibited(Summary)";

		try
		{
			if ((*m_dataNode)->getOverrideSummary().value)
			{
		        if (numSummaries == 0)
			        summaryTxt += FilterString[OVERRIDDEN_FILTERSTRING] + FilterString[SUMMARY_FILTERSTRING]; //  "Overridden(Summary)"
		        else
			        summaryTxt += FilterString[STATUS_SEPARATOR_FILTERSTRING] + FilterString[OVERRIDDEN_FILTERSTRING] + FilterString[SUMMARY_FILTERSTRING]; //  ", Overridden(Summary)";
			}
		}
		catch (...)
		{
		}

		return summaryTxt;
	}

    void DataNodeListItem::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
		TA_Base_Core::ThreadGuard guard( m_lock );
		if ( NULL != m_dataNode )
		{
			EquipmentStatusListItem::processEntityUpdateEvent( entityKey, updateType);
		}
    }


    void DataNodeListItem::CheckFilters(FilterFlagsAndStrings const& filters)
    {
        if ( true == filters[REPORT_TYPE_FILTER_OFFSET].filterInUse )
        {
            if ( ! ( (REPORT_TYPE_EQUIPMENT == filters[REPORT_TYPE_FILTER_OFFSET].filterString) ||
                     (REPORT_TYPE_ALL       == filters[REPORT_TYPE_FILTER_OFFSET].filterString) ) )
            {
                m_isFiltered = true;
                return;
            }
        }

        EquipmentStatusListItem::CheckFilters(filters);
    }

    unsigned long DataNodeListItem::getItemDataNodeEntityKey()
    {
        return (*m_dataNode)->getEntityKey();
    }

    std::string DataNodeListItem::getItemDataNodeEntityName()
    {
        return (*m_dataNode)->getEntityName();
    }

	bool DataNodeListItem::UpdateTextIfChanged( EslColumns column, CString const& newText)
	{
		//we only care about these dynamic columns
		bool dynamicColumns = ( column == STATUS_COLUMN ||
								column == MODIFIED_BY_COLUMN/* ||
								column == TIMESTAMP_COLUMN*/);
		if ( dynamicColumns && (newText != m_text[column]) )
		{
			m_text[column] = newText;
			m_columnUpdateFlags|= (1 << column);
			return true;
		}
		return false;
	}

	void DataNodeListItem::initStaticColumn()
	{
		m_text[ASSET_COLUMN] = (*m_dataNode)->getAssetName().c_str();
		m_text[DESCRIPTION_COLUMN] = (*m_dataNode)->getLabel().c_str();		
	}

} // TA_Base_App

