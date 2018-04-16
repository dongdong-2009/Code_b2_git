/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/ConfigInterlockRule.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif /* defined( WIN32 ) */

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/interlocking/src/InterlockAlias.h"
#include "core/data_access_interface/interlocking/src/ConfigInterlockRule.h"
#include "core/data_access_interface/interlocking/src/InterlockRuleHelper.h"

namespace TA_Base_Core
{
    ConfigInterlockRule::ConfigInterlockRule()
    {
        m_interlockRuleHelper = new InterlockRuleHelper();
    }


    ConfigInterlockRule::ConfigInterlockRule(const unsigned long key)
    {
        m_interlockRuleHelper = new InterlockRuleHelper(key);
    }


    ConfigInterlockRule::ConfigInterlockRule(const ConfigInterlockRule& c)
    {
        TA_ASSERT(false,"function should not be used");
    }


    ConfigInterlockRule::~ConfigInterlockRule() 
	{
        delete m_interlockRuleHelper;
        m_interlockRuleHelper=NULL;
    }


    void ConfigInterlockRule::deleteThisObject()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        m_interlockRuleHelper->deleteInterlockRule();
    }

    void ConfigInterlockRule::applyChanges()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        m_interlockRuleHelper->writeInterlockRuleData();
    }

    bool ConfigInterlockRule::isNew()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->isNew();
    }

    bool ConfigInterlockRule::hasChanged()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return !m_interlockRuleHelper->isValid();
    }

    ItemChanges ConfigInterlockRule::getAllItemChanges()
    {
        ItemChanges ret;

        return ret; 
    }

    unsigned long ConfigInterlockRule::getKey()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getKey();
    }


    std::string ConfigInterlockRule::getName()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getName();
    }


    unsigned long ConfigInterlockRule::getEntityKey()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getEntityKey();
    }


    std::string ConfigInterlockRule::getEntityType()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getEntityType();
    }


    std::string ConfigInterlockRule::getDesiredState()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getDesiredState();
    }


    bool ConfigInterlockRule::getPreCondition()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getPreCondition();
    }


    std::string ConfigInterlockRule::getCondition()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getCondition();
    }


    std::vector<std::string> ConfigInterlockRule::getParameters()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getParameters();
    }


    std::string ConfigInterlockRule::getDescription()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getDescription();
    }


    IInterlockAliasVector ConfigInterlockRule::getAliases()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getAliases();
    }


    bool ConfigInterlockRule::getEnabled()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getEnabled();
    }


	void ConfigInterlockRule::setName( const std::string& name )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setName( name );
	}


	void ConfigInterlockRule::setEntityKey( unsigned long entityKey )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setEntityKey( entityKey );
	}
	

	void ConfigInterlockRule::setEntityType( const std::string& entityType )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setEntityType( entityType );
	}


	void ConfigInterlockRule::setDesiredState( const std::string& desiredState )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setDesiredState( desiredState );
	}


	void ConfigInterlockRule::setPreCondition( const bool precondition )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setPreCondition( precondition );
	}


	void ConfigInterlockRule::setCondition( const std::string& condition )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setCondition( condition );
	}


    void ConfigInterlockRule::setParameters( const std::vector<std::string>& parameters )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setParameters( parameters );
	}


	void ConfigInterlockRule::setDescription( const std::string& expression )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setDescription( expression );
	}


	void ConfigInterlockRule::setEnabled( const bool expression )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setEnabled( expression );
	}


    void ConfigInterlockRule::setAliases( const IInterlockAliasVector& aliases )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setAliases( aliases );
	}

    void ConfigInterlockRule::save()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        m_interlockRuleHelper->writeInterlockRuleData();
    }


    void ConfigInterlockRule::invalidate()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        m_interlockRuleHelper->invalidate();
    }

} // closes TA_Base_Core

