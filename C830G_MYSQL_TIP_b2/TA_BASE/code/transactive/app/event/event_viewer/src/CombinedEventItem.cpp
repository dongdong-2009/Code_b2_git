/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/event/event_viewer/src/CombinedEventItem.cpp $
 * @author:  Bradley Cleaver
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2011/01/24 12:57:22 $
 * Last modified by:  $Author: builder $
 * 
 * CombinedEventItem is an implementation of the DisplayItem abstract class. It contains the data for one row in the list, and 
 * defines the standard ordering operator (<) for list sorting.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/EventViewer.h"
#include "app/event/event_viewer/src/CombinedEventItem.h"
#include "app/event/event_viewer/src/CombinedEventFilter.h"
#include "core/data_access_interface/src/AlarmData.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/utilities/src/RunParams.h"

// NOTE: This inclusion of the CombinedEventFilter is simply to gain access to the statically declared variables. While
// this does lead to a cross-dependency (as AuditEventFilter is dependant on this class), that is preferable to 
// hard coding string variables throughout the application
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#include <iomanip>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace
{
    const std::string DEFAULT_VALUE("");
	// Alarm Comments identifiers
    const std::string COMMENT_IDENTIFIER("C");
	// Alarm DSS Plan identifiers
	const std::string AUTO_PLAN_NOT_RUN_ID("A");
	const std::string SEMI_AUTO_PLAN_NOT_RUN_ID("S");
	const std::string PLAN_RUN_ID("R");
	const std::string PLAN_POSTPONED_ID("X");
	// Alarm Avalanche Status identifiers
	const std::string AVL_HEAD_ID("H");
	const std::string AVL_CHILD_ID("F");
	// Alarm Mms Status identifiers
	const std::string MMS_AUTO_ID("A");
	const std::string MMS_SEMI_ID("S");
	const std::string MMS_JR_SENT_ID("R");
	const std::string MMS_FAILED_ID("D");
	const std::string MMS_INHIBIT_ID("I");
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CombinedEventItem::CombinedEventItem( TA_Base_Core::ICombinedEventData* const eventData )
{
    TA_ASSERT( NULL != eventData, "CombinedEventItem constructor provided with NULL data." );

    // Convert the creation time into a string in the required format.
	// TD #11252
	// Format the event date as "dd-Mon-yy hh:mm:ss"
	// eg. "28-Jul-05 10:23:40"

	// TD12474 ++
	//std::string timePart(CTime(eventData->getCreateTime().time).Format("%d-%b-%Y %H:%M:%S"));

	TA_Base_Core::DateFormat dateFormat; 
	std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4);

	std::string timePart(CTime(eventData->getCreateTime().time).Format(dateFormatStr.c_str()));
	// ++ TD12474

    std::stringstream fullString;
    fullString << timePart;
    m_strCreateTime = CTime(eventData->getCreateTime().time).Format("%Y%m%d%H%M%S");
	
	// wenguang++
	// Change the millisecond setting to the RUNPARAM
	if (TA_Base_Core::RunParams::getInstance().isSet( EventConstants::RPARAM_MILLISECOND.c_str()))
	{
		fullString << "." << std::setfill('0') << std::setw(3) << eventData->getCreateTime().millitm;
	}

    // Insert the derived time string into the data
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_DATETIME,
         fullString.str()) );
    m_createTime = eventData->getCreateTime();

    // Insert the severity name.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_SEVERITY,
          eventData->getAlarmSeverityName() ) );

    // Insert the asset name.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_ASSET,
         eventData->getAssetName() ) );

    // Insert the description.

	/////////////////////////////////////////////////////////////////////
	//CL-15090 modified by: zhangjunsheng 2008/08/06 14:00:00
	
	std::string strTem = eventData->getDescription();
	std::string strDescription("JobRequestSubmitted from ");
	std::string strValue("");
	CString cstrTem("JobRequestSubmitted");
	
	if (cstrTem == strTem.substr(0,19).c_str())
	{
		strDescription += eventData->getAssetName();
		m_columnData.insert(std::map<std::string,std::string>::value_type
			(CombinedEventFilter::COLUMN_DESCRIPTION,
			strDescription ) );
		m_columnData.insert(std::map<std::string,std::string>::value_type
			(CombinedEventFilter::COLUMN_VALUE,
			strValue ) );
	}

	else
	{
		m_columnData.insert(std::map<std::string,std::string>::value_type
			(CombinedEventFilter::COLUMN_DESCRIPTION,
			eventData->getDescription() ) );
		
		// Insert the value.
		std::string valueStr(eventData->getValue());
		//TD12415
		// Change the format 20.0(0C) to correct format of degree Celsius
		// using ALT+0176
		CString degreeString = valueStr.c_str();
		
		if (degreeString.Find("?C", 0) >= 0)
		{
            // MYSQL-FIXME
			// degreeString.Replace(_T('?'), _T('?));
		}
		valueStr.assign(degreeString);
		
		//TD12415
		
		m_columnData.insert(std::map<std::string,std::string>::value_type
			(CombinedEventFilter::COLUMN_VALUE,
			valueStr ) );
	}
	
    /*m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_DESCRIPTION,
        eventData->getDescription() ) );

    // Insert the value.
    std::string valueStr(eventData->getValue());
	//TD12415
	// Change the format 20.0(0C) to correct format of degree Celsius
	// using ALT+0176
	CString degreeString = valueStr.c_str();

	if (degreeString.Find("?C", 0) >= 0)
	{
		degreeString.Replace(_T('?'), _T('?));
	}
	valueStr.assign(degreeString);

	//TD12415

    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_VALUE,
         valueStr ) );*/
    //CL-15090
	/////////////////////////////////////////////////////////////////////

    // Insert the MMS state.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_MMS,
         eventData->getAlarmMmsState() ) );

    // Insert the DSS state.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_DSS,
         eventData->getAlarmDssState() ) );
    
    // Insert the AVL state.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_AVL,
         eventData->getAlarmAvlState() ) );

    // Insert the operator name.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_OPERATOR,
         eventData->getOperatorName() ) );

    // Insert the comment.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::COLUMN_COMMENT,
        eventData->getAlarmComment() ) );

    // Insert the alarmid.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::HIDDEN_COLUMN_ALARMID,
        eventData->getAlarmId() ) );

    // Insert the ack status.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::HIDDEN_COLUMN_ACKED,
        eventData->getAlarmAck() ) );

    // Insert the close status.
    m_columnData.insert(std::map<std::string,std::string>::value_type
        (CombinedEventFilter::HIDDEN_COLUMN_STATUS,
         eventData->getAlarmStatus() ) );
}


CombinedEventItem::~CombinedEventItem()
{
}


const std::string& CombinedEventItem::getColumnData(const std::string& columnName)
{
    // Try to find the specified column in the data available for this item
	std::map<std::string,std::string>::iterator iter = m_columnData.find(columnName);

    if (iter != m_columnData.end())
    {
        // If the comment column is requested then we only want to display a 'C'.
        if ( ( CombinedEventFilter::COLUMN_COMMENT == columnName ) && ( 0 < iter->second.size() ) )
        {
            return COMMENT_IDENTIFIER;
        }
		else if ( ( CombinedEventFilter::COLUMN_MMS == columnName ) && ( 0 < iter->second.size() ) )
		{
			// this is the MMS status column
			 return getMmsStatusIdentifier(iter->second);
		}
		else if ( ( CombinedEventFilter::COLUMN_DSS == columnName ) && ( 0 < iter->second.size() ) )
		{
			// this is the DSS Plan column
			return getDssPlanStatusIdentifier(iter->second);
		}
		else if ( ( CombinedEventFilter::COLUMN_AVL == columnName ) && ( 0 < iter->second.size() ) )
		{
			// this is the AVL status column
			return getAvlStatusIdentifier(iter->second);
		}

        // If it is found, return the string stored there
        return iter->second;
    }
    else
    {
        // If not return an empty string
        return DEFAULT_VALUE;
    }
}

// These functions are to convert the various mms, dss and avl status from string stored in database
// to defined identifier (ie. letter) for display
const std::string& CombinedEventItem::getMmsStatusIdentifier(const std::string& mmsStatus)
{
	//check the given string with the static const used in the alarm data
	//as mms status is a number under alarm data, there is need to convert the string to number
	int status = atoi(mmsStatus.c_str());
	
	switch (status)
	{
		case TA_Base_Core::IAlarmData::TYPE_SEMI:
			return MMS_SEMI_ID;
			break;
		case TA_Base_Core::IAlarmData::TYPE_AUTO:
			return MMS_AUTO_ID;
			break;
		case TA_Base_Core::IAlarmData::TYPE_JR_SENT:
			return MMS_JR_SENT_ID;
			break;
		case TA_Base_Core::IAlarmData::TYPE_INHIBIT:
			return MMS_INHIBIT_ID;
			break;
		case TA_Base_Core::IAlarmData::TYPE_FAILED:
			return MMS_FAILED_ID;
			break;
		default:
			return DEFAULT_VALUE;
	}
}

const std::string& CombinedEventItem::getDssPlanStatusIdentifier(const std::string& dssStatus)
{
	if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::AUTO_PLAN_NOT_RUN) )
	{
		return AUTO_PLAN_NOT_RUN_ID;
	}
	else if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::SEMIAUTO_PLAN_NOT_RUN) )
	{
		return SEMI_AUTO_PLAN_NOT_RUN_ID;
	}
	else if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::PLAN_RUN) )
	{
		return PLAN_RUN_ID;
	}
	else if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::PLAN_POSTPONED) )
	{
		return PLAN_POSTPONED_ID;
	}
	else
	{
		return DEFAULT_VALUE;
	}
}

const std::string& CombinedEventItem::getAvlStatusIdentifier(const std::string& avlStatus)
{
	//check the given string with the static const used in the alarm data
	//as avl status is a number under alarm data, there is need to convert the string to number
	int status = atoi(avlStatus.c_str());
	
	switch (status)
	{
		case TA_Base_Core::IAlarmData::HEAD_AVL:
			return AVL_HEAD_ID;
			break;
		case TA_Base_Core::IAlarmData::CHILD_AVL:
			return AVL_CHILD_ID;
			break;
		default:
			return DEFAULT_VALUE;
	}
}


const std::string& CombinedEventItem::getCompleteColumnData( const std::string& columnName )
{
    // Try to find the specified column in the data available for this item
    std::map<std::string,std::string>::iterator iter = m_columnData.find(columnName);

    if (iter != m_columnData.end())
    {
        // If it is found, return the string stored there
        return iter->second;
    }
    else
    {
        // If not return an empty string
        return DEFAULT_VALUE;
    }
}


bool CombinedEventItem::operator<(DisplayItem& rhs)
{

    // These are both string values, so use std::string.compare(), and store the result, where
    // result > 0 --> this item is lexicographically greater than the rhs item
    // result = 0 --> this item is lexicographically equal to the rhs item
    // result < 0 --> this item is lexicographically less than the rhs item
    int sortValue = 0;
    if ( CombinedEventFilter::COLUMN_DATETIME == s_sortColumn )
    {
        CombinedEventItem* pRhs = dynamic_cast<CombinedEventItem*>(&rhs);
        sortValue = m_createTime.time - pRhs->m_createTime.time;
        if (0 == sortValue)
        {
            // If the sort value was 0, it means the two times were the same, so want to sort on the milliseconds.
            sortValue = m_createTime.millitm - pRhs->m_createTime.millitm;
        }
    }
	else
	{
        std::string lhsData(getColumnData(s_sortColumn));
        std::string rhsData(rhs.getColumnData(s_sortColumn));
		std::transform(lhsData.begin(), lhsData.end(), lhsData.begin(),toupper);
		std::transform(rhsData.begin(), rhsData.end(), rhsData.begin(),toupper);
		sortValue = lhsData.compare(rhsData);

		if( 0 == sortValue ){
			//if the compare result is equal, go on comparing based on timestamp, and by the same sort direction. Added by xiangmei.
			CombinedEventItem* pRhs = dynamic_cast<CombinedEventItem*>(&rhs);
			sortValue = m_createTime.time - pRhs->m_createTime.time;
			if (0 == sortValue)
			{
				// If the sort value was 0, it means the two times were the same, so want to sort on the milliseconds.
				sortValue = m_createTime.millitm - pRhs->m_createTime.millitm;
			}
            
		}
	}

    // Now compare the sort values to determine what we're doing.
    if (s_sortAscending)
    {
        // If the sort method is ascending, a result less than 0 is true (i.e. lower item first)
        if (0 > sortValue)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // If the sort method is decending, a result greater than 0 is true (i.e. lower item last).
        if (0 < sortValue)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

std::string CombinedEventItem::getCreateTimeStr()
{
	return m_strCreateTime;
}
