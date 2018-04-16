 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLine.cpp $
  * @author Raymond Pau
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by: $Author: CM $
  * 
  * TelephoneDirectLine is an implementation of ITelephoneDirectLine. It holds the data specific to an TelephoneDirectLine entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLine.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{


    TelephoneDirectLine::TelephoneDirectLine(const unsigned long key)
     : m_teSpeedDialSetHelper ( new TelephoneDirectLineHelper(key))
    {
        FUNCTION_ENTRY("TelephoneDirectLine");
        FUNCTION_EXIT;
    }

    TelephoneDirectLine::TelephoneDirectLine(unsigned long row, TA_Base_Core::IData& data)
     : m_teSpeedDialSetHelper( new TelephoneDirectLineHelper(row, data))
    {
        FUNCTION_ENTRY("TelephoneDirectLine");
        FUNCTION_EXIT;
    }

    TelephoneDirectLine::TelephoneDirectLine(unsigned long setPrimaryKey, long operaterKey, long profId, const std::string& setName)
         : m_teSpeedDialSetHelper( new TelephoneDirectLineHelper(setPrimaryKey, operaterKey, profId, setName))
    {
    }

    TelephoneDirectLine::TelephoneDirectLine()
     : m_teSpeedDialSetHelper ( new TelephoneDirectLineHelper())
    {
        FUNCTION_ENTRY("TelephoneDirectLine");
        FUNCTION_EXIT;
    }

    TelephoneDirectLine::~TelephoneDirectLine() 
    {
        FUNCTION_ENTRY("~TelephoneDirectLine");
        delete m_teSpeedDialSetHelper;
        m_teSpeedDialSetHelper=NULL;
        FUNCTION_EXIT;
    }

    std::string TelephoneDirectLine::getName()
    {
        FUNCTION_ENTRY("string TelephoneDirectLine::getName");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The m_teSpeedDialSetHelper pointer is null.");

        std::ostringstream name;
	    name << "TE_SPEED_DIAL_BUTTON " << m_teSpeedDialSetHelper->getDialBtnPrimaryKey();

        FUNCTION_EXIT;
        return name.str();
    }

    unsigned long TelephoneDirectLine::getBtnSetKey()
    {
        FUNCTION_ENTRY("getBtnSetKey");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialSetPrimaryKey();
    }

    long TelephoneDirectLine::getOperatorKey()
    {
        FUNCTION_ENTRY("getOperatorKey");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialSetOperatorKey();
    }

    long TelephoneDirectLine::getSeProfId()
    {
        FUNCTION_ENTRY("getSeProfId");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialSetSeProfId();
    }

    std::string TelephoneDirectLine::getSetName()
    {
        FUNCTION_ENTRY("string TelephoneDirectLine::getSetName");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialSetSetName();
    }

    int TelephoneDirectLine::getButtonPosition()
    {
        FUNCTION_ENTRY("getButtonPosition");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialBtnPos();
    }

    std::string TelephoneDirectLine::getTelephoneNumber()
    {
        FUNCTION_ENTRY("string TelephoneDirectLine::getTelephoneNumber");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialBtnTsi();
    }

    std::string TelephoneDirectLine::getLabel()
    {
        FUNCTION_ENTRY("string TelephoneDirectLine::getLabel");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialBtnLabel();
    }

    unsigned long TelephoneDirectLine::getKey()
    {
        FUNCTION_ENTRY("getKey");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialBtnPrimaryKey();
    }

    void TelephoneDirectLine::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->invalidate();
        FUNCTION_EXIT;
    }
} // close namespace TA_IRS_Core

