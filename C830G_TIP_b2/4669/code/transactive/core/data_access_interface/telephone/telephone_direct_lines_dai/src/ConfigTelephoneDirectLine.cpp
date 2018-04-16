/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/ConfigTelephoneDirectLine.cpp $
  * @author Raymond Pau
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigTelephoneDirectLine is a concrete implementation of IConfigTelephoneDirectLine, which is in turn an implementation
  * of ITelephoneDirectLine. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TelephoneDirectLines.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <iterator>
#include <sstream>

#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/ConfigTelephoneDirectLine.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    long ConfigTelephoneDirectLine::s_nextAvailableIdentifier = 0;

    ConfigTelephoneDirectLine::ConfigTelephoneDirectLine()
     : m_teSpeedDialSetHelper( new TelephoneDirectLineHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        FUNCTION_ENTRY("Default Constructor");
        FUNCTION_EXIT;
    }


    ConfigTelephoneDirectLine::ConfigTelephoneDirectLine(unsigned long row, TA_Base_Core::IData& data)
     : m_teSpeedDialSetHelper(new TelephoneDirectLineHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        FUNCTION_ENTRY("Constructor - IData");
        FUNCTION_EXIT;
    }


    ConfigTelephoneDirectLine::ConfigTelephoneDirectLine(const unsigned long key)
     : m_teSpeedDialSetHelper( new TelephoneDirectLineHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        FUNCTION_ENTRY("Constructor - key");
        FUNCTION_EXIT;
    }


    ConfigTelephoneDirectLine::ConfigTelephoneDirectLine( const ConfigTelephoneDirectLine& theConfigTelephoneDirectLine)
     : m_teSpeedDialSetHelper( new TelephoneDirectLineHelper( *(theConfigTelephoneDirectLine.m_teSpeedDialSetHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
        FUNCTION_ENTRY("Copy Constructor");
        FUNCTION_EXIT;
    }


    ConfigTelephoneDirectLine::ConfigTelephoneDirectLine(unsigned long setPrimaryKey, long operaterKey, long profId, const std::string& setName)
     : m_teSpeedDialSetHelper( new TelephoneDirectLineHelper(setPrimaryKey, operaterKey, profId, setName)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        FUNCTION_ENTRY("Constructor");
        FUNCTION_EXIT;
    }


    ConfigTelephoneDirectLine::~ConfigTelephoneDirectLine()
    {
        FUNCTION_ENTRY("Destructor");
        delete m_teSpeedDialSetHelper;
        m_teSpeedDialSetHelper = NULL;
        FUNCTION_EXIT;
    }


    unsigned long ConfigTelephoneDirectLine::getUniqueIdentifier()
    {
        FUNCTION_ENTRY("getUniqueIdentifier");
        FUNCTION_EXIT;
        return m_uniqueIdentifier;
    }


    bool ConfigTelephoneDirectLine::isNew()
    {
        FUNCTION_ENTRY("isNew");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getIsDialBtnNew();
    }


    std::string ConfigTelephoneDirectLine::getName()
    {
        FUNCTION_ENTRY("string ConfigTelephoneDirectLine::getName");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The m_teSpeedDialSetHelper pointer is null.");

        std::ostringstream name;
	    name << "TE_SPEED_DIAL_BUTTON " << m_teSpeedDialSetHelper->getDialBtnPrimaryKey();

        FUNCTION_EXIT;
        return name.str();
    }


    void ConfigTelephoneDirectLine::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }


    void ConfigTelephoneDirectLine::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL, "applyChanges() has been called after ConfigTelephoneDirectLine was deleted");
        m_teSpeedDialSetHelper->writeSetData(); 
        m_teSpeedDialSetHelper->writeBtnData();
        FUNCTION_EXIT;
    }


    void ConfigTelephoneDirectLine::writeBtnData()
    {
        FUNCTION_ENTRY("writeBtnData");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->writeBtnData();
        FUNCTION_EXIT;
    }


    void ConfigTelephoneDirectLine::writeSetData()
    {
        FUNCTION_ENTRY("writeSetData");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->writeSetData();
        FUNCTION_EXIT;
    }


    unsigned long ConfigTelephoneDirectLine::getBtnSetKey()
    {
        FUNCTION_ENTRY("getBtnSetKey");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialSetPrimaryKey();
    }


    void ConfigTelephoneDirectLine::setBtnSetKey(const unsigned long key)
    {
        FUNCTION_ENTRY("setBtnSetKey");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->setDialSetPrimaryKey(key);
        FUNCTION_EXIT;
    }


    long ConfigTelephoneDirectLine::getOperatorKey()
    {
        FUNCTION_ENTRY("getOperatorKey");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialSetOperatorKey();
    }


    void ConfigTelephoneDirectLine::setOperatorKey(const long key)
    {
        FUNCTION_ENTRY("setOperatorKey");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->setDialSetOperatorKey(key);
        FUNCTION_EXIT;
    }


    long ConfigTelephoneDirectLine::getSeProfId()
    {
        FUNCTION_ENTRY("getSeProfId");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialSetSeProfId();
    }


    void ConfigTelephoneDirectLine::setSeProfId(const long profId)
    {
        FUNCTION_ENTRY("setSeProfId");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->setDialSetSeProfId(profId);
        FUNCTION_EXIT;
    }


    std::string ConfigTelephoneDirectLine::getSetName()
    {
        FUNCTION_ENTRY("string ConfigTelephoneDirectLine::getSetName");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialSetSetName();
    }


    void ConfigTelephoneDirectLine::setSetName(const std::string& name)
    {
        FUNCTION_ENTRY("setSetName");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->setDialSetSetName(name);
        FUNCTION_EXIT;
    }


    int ConfigTelephoneDirectLine::getButtonPosition()
    {
        FUNCTION_ENTRY("getButtonPosition");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialBtnPos();
    }


    void ConfigTelephoneDirectLine::setButtonPosition(const int btnPos)
    {
        FUNCTION_ENTRY("setButtonPosition");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->setDialBtnPos(btnPos);
        FUNCTION_EXIT;
    }


    std::string ConfigTelephoneDirectLine::getTelephoneNumber()
    {
        FUNCTION_ENTRY("string ConfigTelephoneDirectLine::getTelephoneNumber");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialBtnTsi();
    }


    void ConfigTelephoneDirectLine::setTelephoneNumber(const std::string& number)
    {
        FUNCTION_ENTRY("setTelephoneNumber");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->setDialBtnTsi(number);
        FUNCTION_EXIT;
    }


    std::string ConfigTelephoneDirectLine::getLabel()
    {
        FUNCTION_ENTRY("string ConfigTelephoneDirectLine::getLabel");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialBtnLabel();
    }


    void ConfigTelephoneDirectLine::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->setDialBtnLabel(label);
        FUNCTION_EXIT;
    }


    unsigned long ConfigTelephoneDirectLine::getKey()
    {
        FUNCTION_ENTRY("getKey");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        FUNCTION_EXIT;
        return m_teSpeedDialSetHelper->getDialBtnPrimaryKey();
    }


    void ConfigTelephoneDirectLine::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->invalidate();
        FUNCTION_EXIT;
    }


    void ConfigTelephoneDirectLine::deleteTeSpeedDialSetRecord(bool cascade)
    {
        FUNCTION_ENTRY("deleteTeSpeedDialSetRecord");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->deleteTeSpeedDialSetRecord(cascade);
        FUNCTION_EXIT;
    }


    void ConfigTelephoneDirectLine::deleteTeSpeedDialButtonRecord(bool cascade)
    {
        FUNCTION_ENTRY("deleteTeSpeedDialButtonRecord");
        TA_ASSERT(m_teSpeedDialSetHelper != NULL,"The TelephoneDirectLineHelper pointer is null.");
        m_teSpeedDialSetHelper->deleteTeSpeedDialButtonRecord();
        FUNCTION_EXIT;
    }

} //close namespace TA_IRS_Core
