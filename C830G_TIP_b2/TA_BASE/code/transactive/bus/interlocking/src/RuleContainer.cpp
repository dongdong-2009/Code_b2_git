/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/RuleContainer.cpp $
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
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/InterlockingException.h"
#include "core/data_access_interface/interlocking/src/IInterlockAlias.h"
#include "core/data_access_interface/interlocking/src/InterlockRuleAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/interlocking/src/RuleContainer.h"
#include "bus/interlocking/src/InterlockingRule.h"
#include "core/data_access_interface/interlocking/src/IInterlockRule.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"

#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"


using TA_Base_Core::ThreadGuard;
using TA_Base_Core::ThreadReadGuard;


namespace TA_Base_Bus
{


    //
    // Constructor
    //
    RuleContainer::RuleContainer()
        :m_entityKey(0),
         m_entityType("")
    {
        loadRules();
    }

    //
    // Constructor for a specific entity
    //
    RuleContainer::RuleContainer(const unsigned long p_entityKey, const std::string& p_entityType)
        :m_entityKey(p_entityKey),
         m_entityType(p_entityType)
    {
        loadRules();
    }


    //
    // Destructor
    //
    RuleContainer::~RuleContainer()
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        m_entityKeyToRuleMap.empty();
    }


    //
    // getRulesForEntity and requested action
    //
    std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> > RuleContainer::getRulesForEntity(unsigned long p_entityKey,
                                                                          const std::string& p_action,
                                                                          const bool preCondition) const
    {
        ThreadReadGuard guard(m_lock);
        std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> > ret;

        std::multimap< unsigned long, unsigned long >::const_iterator p;

        p = m_entityKeyToRuleMap.find(p_entityKey);

        if( p != m_entityKeyToRuleMap.end() )
        {
            do
            {
                if( ( p_action.compare(m_rules[p->second]->getEntityAction()) ) == 0 &&
                    ( m_rules[p->second]->getPreCondition() == preCondition) )
                {
                    ret.push_back( m_rules[p->second] );
                }
                p++;
            }
            while( p!= m_entityKeyToRuleMap.upper_bound(p_entityKey) );
        }
        return ret;
    }


    std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> > RuleContainer::getRulesForEntity(unsigned long p_entityKey,
                                                                 const std::string& p_action,
                                                                 const std::string& p_parameter,
                                                                 const bool preCondition) const
    {
        ThreadReadGuard guard(m_lock);
        std::vector< boost::shared_ptr<TA_Base_Bus::InterlockingRule> > ret;

        std::multimap< unsigned long, unsigned long >::const_iterator p;

        p = m_entityKeyToRuleMap.find(p_entityKey);

        if( p != m_entityKeyToRuleMap.end() )
        {
            do
            {
                if( p_action.compare(m_rules[p->second]->getEntityAction()) == 0 &&
                    ( m_rules[p->second]->getPreCondition() == preCondition) )
                {
                    std::vector<std::string> parameters = m_rules[p->second]->getActionParameters();
                    std::vector<std::string>::iterator ip = parameters.begin();
                    while ( ip != parameters.end() )
                    {
                        if (p_parameter.compare( *ip ) == 0 )
                        {
                            ret.push_back( m_rules[p->second] );
                            break;
                        }
                        ip++;
                    }
                }
                p++;
            }
            while( p!= m_entityKeyToRuleMap.upper_bound(p_entityKey) );
        }
        return ret;
    }


    //
    // operator []
    //
    TA_Base_Bus::InterlockingRule* RuleContainer::operator[](unsigned int i)
    {
        ThreadReadGuard guard(m_lock);
        if ( i >= m_rules.size() )
        {
            TA_ASSERT(false,"index out of bounds");
        }
        return m_rules[i].get();
    }


    //
    // addRule
    //
    void RuleContainer::addRule( boost::shared_ptr<TA_Base_Bus::InterlockingRule>& p_rule )
    {
        ThreadGuard guard(m_lock);
        m_rules.push_back( p_rule );
        m_entityKeyToRuleMap.insert(std::pair<unsigned long, unsigned long >
			(p_rule->getEntityKey(), m_rules.size() - 1 ));
    }


    //
    // deleteRule
    //
    void RuleContainer::deleteRule( unsigned int p_entityKey, unsigned long p_ruleId )
    {
        ThreadGuard guard(m_lock);
        std::multimap< unsigned long, unsigned long >::iterator p;

        p = m_entityKeyToRuleMap.find(p_entityKey);

        if( p != m_entityKeyToRuleMap.end() )
        {
            do
            {
                if( m_rules[p->second]->getRuleId() == p_ruleId )
                {
                    // found it!
                    std::vector<boost::shared_ptr<TA_Base_Bus::InterlockingRule> >::iterator r = m_rules.begin();
                    r += p->second;
                    m_rules.erase(r);
                    m_entityKeyToRuleMap.erase(p);
                    return;
                }
                
                p++;\

            }
            while( p!= m_entityKeyToRuleMap.upper_bound(p_entityKey) );
        }

    }


    //
    // addBooleanContext
    //
    void RuleContainer::addBooleanContext( const IBooleanContext* p_booleanContext )
    {
        ThreadGuard guard(m_lock);
        for( std::multimap<unsigned long, unsigned long >::iterator p = m_entityKeyToRuleMap.begin();
             p != m_entityKeyToRuleMap.end();
             p++ )
        {
            (m_rules[p->second])->addBooleanContext(p_booleanContext);
        }
    }


    //
    // addRealNumberContext
    //
    void RuleContainer::addRealNumberContext( const IRealNumberContext* p_realNumContext )
    {
        ThreadGuard guard(m_lock);
        for( std::multimap<unsigned long, unsigned long >::iterator p = m_entityKeyToRuleMap.begin();
             p != m_entityKeyToRuleMap.end();
             p++ )
        {
            (m_rules[p->second])->addRealNumberContext(p_realNumContext);
        }
    }


    //
    // addStringContext
    //
    void RuleContainer::addStringContext( const IStringContext* p_stringContext )
    {
        ThreadGuard guard(m_lock);
        for( std::multimap<unsigned long, unsigned long >::iterator p = m_entityKeyToRuleMap.begin();
             p != m_entityKeyToRuleMap.end();
             p++ )
        {
            (m_rules[p->second])->addStringContext(p_stringContext);
        }
    }


    //
    // reloadRule
    //
    void RuleContainer::reloadRule( unsigned long p_entityKey, unsigned long p_ruleId )
    {
        ThreadGuard guard(m_lock);
        // find our copy of the rule
        std::multimap< unsigned long, unsigned long >::const_iterator p;

        p = m_entityKeyToRuleMap.find(p_entityKey);

        int index = 0;
        if( p != m_entityKeyToRuleMap.end() )
        {
            do
            {
                if( m_rules[p->second]->getRuleId() == p_ruleId )
                {
                    // found it!
                    index = p->second;
                    break;
                }
            
                p++;\

            }
            while( p!= m_entityKeyToRuleMap.upper_bound(p_entityKey) );

            // Should never happen
            // a possible workaround if it does is to reload the list from scratch.
            TA_ASSERT( index != 0, "Rule list out of sync - missing rule" );
        }

        try
        {
            // get the rule again
            TA_Base_Core::IInterlockRule* rulePointer = NULL;
            rulePointer = TA_Base_Core::InterlockRuleAccessFactory::getInstance().getInterlockRule( p_ruleId, false );

            m_rules[index]->setRuleId(rulePointer->getKey());
            m_rules[index]->setEntityKey(rulePointer->getEntityKey());
            m_rules[index]->setEntityType(rulePointer->getEntityType());
            m_rules[index]->setEntityAction(rulePointer->getDesiredState());
            m_rules[index]->setActionParameters(rulePointer->getParameters());
            m_rules[index]->setPrecondition(rulePointer->getPreCondition());
            m_rules[index]->setEnabled(rulePointer->getEnabled());
            m_rules[index]->setCondition(rulePointer->getCondition());
            m_rules[index]->setName(rulePointer->getName());
            m_rules[index]->setDescription(rulePointer->getDescription());

            TA_Base_Core::IInterlockAliasVector ialiases = rulePointer->getAliases();
            InterlockingRuleAliasVector aliasList;
            m_rules[index]->setAliases( aliasList );
            for (TA_Base_Core::IInterlockAliasVector::iterator ap = ialiases.begin(); ap != ialiases.end(); ap++)
            {
                boost::shared_ptr<TA_Base_Bus::InterlockingRuleAlias> alias(new TA_Base_Bus::InterlockingRuleAlias(**ap));
                m_rules[index]->addAlias(alias);
            }

        }
        catch( const TA_Base_Core::DataException& de )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", de.what());

            std::string msg = "Caught a DataException with the following message - ";
                        msg+= de.what();
            TA_THROW(TA_Base_Core::InterlockingException(msg));
        }
        catch( const TA_Base_Core::DatabaseException& dbe )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", dbe.what());

            std::string msg = "Caught a DatabaseException with the following message - ";
                        msg+= dbe.what();
            TA_THROW(TA_Base_Core::InterlockingException(msg));
        }
        catch( ... )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught while loading rules");

            TA_THROW(TA_Base_Core::InterlockingException("Unable to load interlocking rules from database"));
        }
    }

    //
    // loadRule
    //
    void RuleContainer::loadRule( unsigned long ruleId )
    {
        try
        {
            TA_Base_Core::IInterlockRule* rulePointer = NULL;

            // get all rules
            rulePointer = TA_Base_Core::InterlockRuleAccessFactory::getInstance().getInterlockRule( ruleId, false );

			// transform to a smart pointer
            boost::shared_ptr<TA_Base_Bus::InterlockingRule> rule( new TA_Base_Bus::InterlockingRule() );

            rule->setRuleId(rulePointer->getKey());
            rule->setEntityKey(rulePointer->getEntityKey());
            rule->setEntityType(rulePointer->getEntityType());
            rule->setEntityAction(rulePointer->getDesiredState());
            rule->setActionParameters(rulePointer->getParameters());
            rule->setPrecondition(rulePointer->getPreCondition());
            rule->setEnabled(rulePointer->getEnabled());
            rule->setCondition(rulePointer->getCondition());
            rule->setName(rulePointer->getName());
            rule->setDescription(rulePointer->getDescription());

            TA_Base_Core::IInterlockAliasVector ialiases = rulePointer->getAliases();
            for (TA_Base_Core::IInterlockAliasVector::iterator ap = ialiases.begin(); ap != ialiases.end(); ap++)
            {
                boost::shared_ptr<TA_Base_Bus::InterlockingRuleAlias> alias(new TA_Base_Bus::InterlockingRuleAlias(**ap));
                rule->addAlias(alias);
            }

            addRule( rule );

        }
        catch( const TA_Base_Core::DataException& de )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", de.what());

            std::string msg = "Caught a DataException with the following message - ";
                        msg+= de.what();
            TA_THROW(TA_Base_Core::InterlockingException(msg));
        }
        catch( const TA_Base_Core::DatabaseException& dbe )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", dbe.what());

            std::string msg = "Caught a DatabaseException with the following message - ";
                        msg+= dbe.what();
            TA_THROW(TA_Base_Core::InterlockingException(msg));
        }
        catch( ... )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught while loading rules");

            TA_THROW(TA_Base_Core::InterlockingException("Unable to load interlocking rules from database"));
        }
    }

    //
    // loadRules
    //
    void RuleContainer::loadRules()
    {
        try
        {
            std::vector<TA_Base_Core::IInterlockRule*> rulesAsPointers;
            if ( !m_entityType.empty() && (m_entityKey != 0) )
            {
                // get the rules for this entity
                rulesAsPointers = 
                    TA_Base_Core::InterlockRuleAccessFactory::getInstance().getInterlockRules(m_entityKey, m_entityType);
            }
            else
            {
                // get all rules
                rulesAsPointers = 
                    TA_Base_Core::InterlockRuleAccessFactory::getInstance().getAllInterlockRules();
            }
			std::vector<TA_Base_Core::IInterlockRule*>::iterator p = rulesAsPointers.begin();

			// transform to a vector of smart pointers
			std::vector< boost::shared_ptr<TA_Base_Core::IInterlockRule> > rules;			
			for( p; p != rulesAsPointers.end(); p++ )
			{
				rules.push_back( boost::shared_ptr<TA_Base_Core::IInterlockRule> ( *p ) );
			}

			std::vector< boost::shared_ptr<TA_Base_Core::IInterlockRule> >::iterator sp = rules.begin();
            while( sp != rules.end() )
            {
                boost::shared_ptr<TA_Base_Bus::InterlockingRule> rule( new TA_Base_Bus::InterlockingRule() );

                rule->setRuleId((*sp)->getKey());
                rule->setEntityKey((*sp)->getEntityKey());
                rule->setEntityType((*sp)->getEntityType());
                rule->setEntityAction((*sp)->getDesiredState());
                rule->setActionParameters((*sp)->getParameters());
                rule->setPrecondition((*sp)->getPreCondition());
                rule->setEnabled((*sp)->getEnabled());
                rule->setCondition((*sp)->getCondition());
                rule->setName((*sp)->getName());
                rule->setDescription((*sp)->getDescription());

                TA_Base_Core::IInterlockAliasVector ialiases = (*sp)->getAliases();
                for (TA_Base_Core::IInterlockAliasVector::iterator ap = ialiases.begin(); ap != ialiases.end(); ap++)
                {
                    boost::shared_ptr<TA_Base_Bus::InterlockingRuleAlias> alias(new TA_Base_Bus::InterlockingRuleAlias(**ap));
                    rule->addAlias(alias);
                }

                addRule( rule );

                sp++;
            }
        }
        catch( const TA_Base_Core::DataException& de )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", de.what());

            std::string msg = "Caught a DataException with the following message - ";
                        msg+= de.what();
            TA_THROW(TA_Base_Core::InterlockingException(msg));
        }
        catch( const TA_Base_Core::DatabaseException& dbe )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", dbe.what());

            std::string msg = "Caught a DatabaseException with the following message - ";
                        msg+= dbe.what();
            TA_THROW(TA_Base_Core::InterlockingException(msg));
        }
        catch( ... )
        {
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught while loading rules");

            TA_THROW(TA_Base_Core::InterlockingException("Unable to load interlocking rules from database"));
        }
    }

    //
    // enableOnlineConfig
    //
    void RuleContainer::enableOnlineConfig()
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToConfigUpdateMessage( 
                                                    TA_Base_Core::ConfigUpdate::ConfigInterlocking,
                                                    this);
    }

    //
    // receiveSpecialisedMessage
    //
    void RuleContainer::receiveSpecialisedMessage(const TA_Base_Core::ConfigUpdateMessageCorbaDef& message)
    {
        // do we care about this message?
        if ( m_entityKey != 0 && m_entityKey != message.assocEntityKey )
        {
            return;
        }

        // get rule id from first entry in change list
        if ( message.changes.length() < 1 )
        {
            TA_ASSERT(false, "Badly formed change message");
        }
        unsigned long ruleId = atol (message.changes[0]);

        switch( message.modificationType )
        {
        case TA_Base_Core::Create:
            // handle new rule
            loadRule( ruleId );
            break;
            
        case TA_Base_Core::Update:
            // handle changed rule
            reloadRule( message.assocEntityKey, ruleId );
            break;
            
        case TA_Base_Core::Delete:
            // handle deleted rules
            deleteRule( message.assocEntityKey, ruleId );
            break;

        default:
            TA_ASSERT(false, "Received unknown message type");
        }

    }

}
