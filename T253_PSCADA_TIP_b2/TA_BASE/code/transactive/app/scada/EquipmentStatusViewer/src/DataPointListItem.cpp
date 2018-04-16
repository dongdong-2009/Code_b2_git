/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataPointListItem.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/01/23 17:49:34 $
  * Last modified by:  $Author: builder $
  *
  * A DataPoint item to be displayed in (or filtered from) the EquipmentStatusListCtrl.
  * This class contains the required datapoint proxy.
  *
  */

#include "stdafx.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/TAAssert.h"
#include "equipmentstatusviewer.h"
#include "DataPointListItem.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
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

    DataPointListItem::DataPointListItem(unsigned long entitykey, TA_Base_Bus::ScadaProxyFactory& proxyFactory, HWND hwndToNotify)
        : EquipmentStatusListItem(entitykey, hwndToNotify),
		  m_dataPoint(NULL),
		  m_dataNode(NULL)
	{
		m_dataPoint = new TA_Base_Bus::DataPointProxySmartPtr(); 
		m_dataNode = new TA_Base_Bus::DataNodeProxySmartPtr();

		proxyFactory.createDataPointProxy ( entitykey, *this, (*m_dataPoint));	
		loadParentDataNode((*m_dataPoint)->getParentKey());   

		proxyFactory.createDataNodeProxy( m_dataNodeEntity, *this, (*m_dataNode));

		initStaticColumn();
    }

	DataPointListItem::DataPointListItem(boost::shared_ptr<TA_Base_Core::DataPointEntityData> dpEntityData, 
		TA_Base_Bus::ScadaProxyFactory& proxyFactory, HWND hwndToNotify)
		: EquipmentStatusListItem(dpEntityData->getKey(), hwndToNotify),
		  m_dataPoint(NULL),
		  m_dataNode(NULL)
	{
		loadParentDataNode(dpEntityData->getParent());

		m_dataPoint = new TA_Base_Bus::DataPointProxySmartPtr();
		m_dataNode = new TA_Base_Bus::DataNodeProxySmartPtr();
		proxyFactory.createDataPointProxy ( dpEntityData, *this, (*m_dataPoint));
		proxyFactory.createDataNodeProxy( m_dataNodeEntity, *this, (*m_dataNode));

		initStaticColumn();
	}

	DataPointListItem::DataPointListItem(boost::shared_ptr<TA_Base_Core::DataPointEntityData> dpEntityData, 
										boost::shared_ptr<TA_Base_Core::DataNodeEntityData> dnParentData, 
										TA_Base_Bus::ScadaProxyFactory& proxyFactory, 
										HWND hwndToNotify)
		: EquipmentStatusListItem(dpEntityData->getKey(), hwndToNotify),
		  m_dataNodeEntity(dnParentData),
		  m_dataPoint(NULL),
		  m_dataNode(NULL)
	{
		if(dnParentData.get() == NULL)
		{
			loadParentDataNode(dpEntityData->getParent());
		}

		m_dataPoint = new TA_Base_Bus::DataPointProxySmartPtr();
		m_dataNode = new TA_Base_Bus::DataNodeProxySmartPtr();
		proxyFactory.createDataPointProxy( dpEntityData, *this, (*m_dataPoint));	
		proxyFactory.createDataNodeProxy( m_dataNodeEntity, *this, (*m_dataNode));

		initStaticColumn();
	}

    DataPointListItem::~DataPointListItem()
    {
		TA_Base_Bus::DataPointProxySmartPtr* dpProxy = m_dataPoint;
		TA_Base_Bus::DataNodeProxySmartPtr*  dnProxy = m_dataNode;

		{
			TA_Base_Core::ThreadGuard guard( m_lock );
			m_dataPoint = NULL;
			m_dataNode = NULL;
		}

		if (dpProxy != NULL)
        {
            delete dpProxy;
        }

		if (dnProxy != NULL)
		{
			delete dnProxy;
		}
    }

    void DataPointListItem::loadParentDataNode(unsigned long parentKey)
    {
        try
        {
            TA_Base_Core::IEntityData* data = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(parentKey);
			TA_Base_Core::DataNodeEntityData* pDnData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(data);
			TA_ASSERT(pDnData != NULL, "Failed to read the datanode information for the datapoint");   
			m_dataNodeEntity = boost::shared_ptr<TA_Base_Core::DataNodeEntityData>(pDnData);                 
        }
        catch (...)
        {
             TA_ASSERT(false, "Failed to create the datanode for the datapoint");            
        }
    }


    CString DataPointListItem::buildStatusText()
    {
		int numInhibits = 0;
		CString statusTxt("");

        try
		{
			if ( (*m_dataPoint)->isScanInhibited())
			{
				statusTxt = FilterString[SCAN_FILTERSTRING]; //  "Scan"
				numInhibits++;
			}
		}
		catch (...)
		{
		}

		try
		{
			if ((*m_dataPoint)->isAlarmInhibited())
			{
				if (numInhibits == 0)
					statusTxt = FilterString[ALARM_FILTERSTRING]; //  "Alarm";
				else
					statusTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[ALARM_FILTERSTRING]; //  ", Alarm";
				numInhibits++;
			}
		}
		catch (...)
		{
		}

		try
		{
			if ((*m_dataPoint)->isMmsAlarmInhibited())
			{
				if (numInhibits == 0)
					statusTxt = FilterString[MMS_FILTERSTRING]; //  "MMS";
				else
					statusTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[MMS_FILTERSTRING]; //  ", MMS";
				numInhibits++;
			}
		}
		catch (...)
		{
		}

		try
		{
			if (m_dataNode != NULL &&
				(*m_dataNode)->getCombinedInhibitMms().quality == TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON && 
				(*m_dataNode)->getCombinedInhibitMms().value)
			{
				if (numInhibits == 0)
					statusTxt = FilterString[MMS_FILTERSTRING]; //  "MMS";
				else
					statusTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[MMS_FILTERSTRING]; //  ", MMS";
				numInhibits++;
			}
		}
		catch (...)
		{
		}

		try
		{
			// if the data point is an output
			if ( true == (*m_dataPoint)->isWriteable() )
			{
				// check if its control is inhibited
				if ((*m_dataPoint)->isControlInhibited())
				{
					statusTxt = FilterString[CONTROL_FILTERSTRING]; //  "Control";
					numInhibits++;
				};
			}
		}
		catch (const TA_Base_Core::ScadaProxyException&)
		{
		}
		catch (...)
		{
		}

		if (numInhibits == 0)
		{
			statusTxt = "";
		}
		else
		{
			statusTxt += FilterString[INHIBITED_FILTERSTRING]; //  " Inhibited";
		}

		try
		{
			if ((*m_dataPoint)->isOverrideEnabled())
			{
			    if (numInhibits == 0)
				    statusTxt = FilterString[OVERRIDDEN_FILTERSTRING]; //  "Overridden";
			    else
				    statusTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[OVERRIDDEN_FILTERSTRING]; //  ", Overridden";
			}
		}
		catch (...)
		{
		}

		return statusTxt;
    }


    void DataPointListItem::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
		TA_Base_Core::ThreadGuard guard( m_lock );
		if ( NULL != m_dataPoint && NULL != m_dataNode)
		{
			EquipmentStatusListItem::processEntityUpdateEvent(getEntityKey(), updateType);
		}
    }


    CString DataPointListItem::buildNewColumnText( TA_Base_App::EslColumns column )
    {
        //  Get the new text for the requested column. By calling this function 
        //  for each column we need only one try-catch(ScadaProxyException) block.
        CString newText("");
        try
        {
            switch (column)
            {
            case VALUE_COLUMN:
                {
                    //  VALUE default is blank
                    newText = (*m_dataPoint)->getValueAsString().c_str();
                    break;
                }
            case QUALITY_COLUMN:
                {
                    newText = (*m_dataPoint)->getQualityAsString();
                    break;
                }
            case STATUS_COLUMN:
                {
                    newText = buildStatusText();
                    break;
                }
            case MODIFIED_BY_COLUMN:
                {
                    if (!getColumnText(STATUS_COLUMN).IsEmpty() )
                    {
                        std::string sessionId = (*m_dataPoint)->getLastStatusChangeSessionId();
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
                        m_timeStamp = (*m_dataPoint)->getLastStatusChangeTimestamp();
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
                    //  Requested column probably uses a field shared between DataNodes and DataPoints
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


    int DataPointListItem::buildNewColumnIcon( TA_Base_App::EslColumns column )
    {
        //  Currently the only icon displayed in the EquipmentStatusViewer is 
        //  the "quality" icon displayed in the DataPoints Value column.

        if ( VALUE_COLUMN != column)
        {
            return EquipmentStatusListItem::buildNewColumnIcon( column);
        }

		bool isAlarm = false;
		try
		{
			if ( (*m_dataPoint)->isInAlarmState())
			{
				isAlarm = true;
			}
		}
		catch (TA_Base_Core::ScadaProxyException& )
		{
			return ESL_BLUE;
		}

		try
		{
			switch ((*m_dataPoint)->getStatus() & TA_Base_Bus::QUALITY_MASK)
			{
			case TA_Base_Bus::QUALITY_GOOD:
				if ((*m_dataPoint)->getStatus() == TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE)
				{
					return ESL_ORANGE;
				}
				if ((*m_dataPoint)->getStatus() == TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON)
				{
					if( !isAlarm )
					{
						return ESL_GREEN;
					}
					else
					{
						return ESL_RED;
					}
				}

				return -1;

			case TA_Base_Bus::QUALITY_UNCERTAIN:
			case TA_Base_Bus::QUALITY_BAD:
				return ESL_BLUE;

			default:
				return -1;

			}
		}
		catch (TA_Base_Core::ScadaProxyException& )
		{
            //  Datapoint status not ready
			return ESL_BLUE;
		}
    }


    void DataPointListItem::CheckFilters( FilterFlagsAndStrings const& filters)
    {
        if ( true == filters[REPORT_TYPE_FILTER_OFFSET].filterInUse )
        {
            if ( ! ( (REPORT_TYPE_DATAPOINT == filters[REPORT_TYPE_FILTER_OFFSET].filterString) ||
                     (REPORT_TYPE_ALL       == filters[REPORT_TYPE_FILTER_OFFSET].filterString) ) )
            {
                m_isFiltered = true;
                return;
            }
        }

        EquipmentStatusListItem::CheckFilters(filters);
    }

    unsigned long DataPointListItem::getItemDataNodeEntityKey()
    {
		if(m_dataNodeEntity.get() != NULL)
		{
			return m_dataNodeEntity->getKey();   
		}
		else
		{
			return 0L;
		}
    }

    std::string DataPointListItem::getItemDataNodeEntityName()
    {
        if(m_dataNodeEntity.get() != NULL)
		{
			return m_dataNodeEntity->getName();
		}
		else
		{
			return "";
		}
    }
	
	bool DataPointListItem::UpdateTextIfChanged( EslColumns column, CString const& newText)
	{
		bool dynamicColumn = !( column == ASSET_COLUMN ||
								column == DESCRIPTION_COLUMN ||
								column == DATAPOINT_COLUMN ||
								column == TYPE_COLUMN );
		if ( dynamicColumn && (newText != m_text[column]) )
		{
			m_text[column] = newText;
			m_columnUpdateFlags|= (1 << column);
			return true;
		}
		return false;
	}

	void DataPointListItem::initStaticColumn()
	{
		if(m_dataNodeEntity.get() != NULL)
		{
			m_text[ASSET_COLUMN] = m_dataNodeEntity->getAssetName().c_str();
			m_text[DESCRIPTION_COLUMN] = m_dataNodeEntity->getLabel().c_str();
		}

		m_text[DATAPOINT_COLUMN] = (*m_dataPoint)->getDescription().c_str();
		m_text[TYPE_COLUMN] = (*m_dataPoint)->getShortNameOfType().c_str();
	}
} // TA_Base_App

