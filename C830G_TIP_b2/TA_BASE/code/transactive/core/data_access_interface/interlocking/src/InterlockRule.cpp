/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/InterlockRule.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif

#include "core/data_access_interface/interlocking/src/InterlockRule.h"
#include "core/data_access_interface/interlocking/src/InterlockRuleHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    InterlockRule::InterlockRule(const unsigned long key)
    {
        m_interlockRuleHelper = new InterlockRuleHelper(key);
    }


    InterlockRule::~InterlockRule() 
	{
        delete m_interlockRuleHelper;
        m_interlockRuleHelper=NULL;
    }


    unsigned long InterlockRule::getKey()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getKey();
    }


    std::string InterlockRule::getName()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getName();
    }


    unsigned long InterlockRule::getEntityKey()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getEntityKey();
    }


    std::string InterlockRule::getEntityType()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getEntityType();
    }


    std::string InterlockRule::getDesiredState()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getDesiredState();
    }


    bool InterlockRule::getPreCondition()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getPreCondition();
    }


    std::string InterlockRule::getCondition()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getCondition();
    }


    std::vector<std::string> InterlockRule::getParameters()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getParameters();
    }


    std::string InterlockRule::getDescription()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getDescription();
    }


    IInterlockAliasVector InterlockRule::getAliases()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getAliases();
    }


    bool InterlockRule::getEnabled()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        return m_interlockRuleHelper->getEnabled();
    }


	void InterlockRule::setName( const std::string& name )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setName( name );
	}


	void InterlockRule::setEntityKey( unsigned long entityKey )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setEntityKey( entityKey );
	}
	

	void InterlockRule::setEntityType( const std::string& entityType )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setEntityType( entityType );
	}


	void InterlockRule::setDesiredState( const std::string& desiredState )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setDesiredState( desiredState );
	}


	void InterlockRule::setPreCondition( const bool precondition )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setPreCondition( precondition );
	}


	void InterlockRule::setCondition( const std::string& condition )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setCondition( condition );
	}


    void InterlockRule::setParameters( const std::vector<std::string>& parameters )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setParameters( parameters );
	}


	void InterlockRule::setDescription( const std::string& expression )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setDescription( expression );
	}


	void InterlockRule::setEnabled( const bool expression )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setEnabled( expression );
	}


    void InterlockRule::setAliases( const IInterlockAliasVector& aliases )
	{
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
		m_interlockRuleHelper->setAliases( aliases );
	}

    void InterlockRule::save()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        m_interlockRuleHelper->writeInterlockRuleData();
    }


    void InterlockRule::invalidate()
    {
        TA_ASSERT(m_interlockRuleHelper != NULL,"The InterlockRuleHelper pointer is null.");
        m_interlockRuleHelper->invalidate();
    }

} // closes TA_Base_Core
