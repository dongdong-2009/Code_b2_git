/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Jade Welton
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * <description>
 */
#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/ConfigUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDSequence.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/GlobalConstants.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/GenericAgent.h"

//#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"  // TD14548

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include <sstream>

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

    //
    // Constructor
    //
    Sequence::Sequence(TA_Base_Core::IEntityDataPtr entityData)
        : VideoInput(entityData)
        , m_sequenceEntityData(boost::dynamic_pointer_cast<TA_Base_Core::Sequence>(entityData))
        , m_pdSequence(NULL)
    {
        m_pdSequence = std::auto_ptr< IPDSequence >(PDInterfaceFactory::createSequence(m_sequenceEntityData));
        activateServantAtInit();
    }


    //
    // Destructor
    //
    Sequence::~Sequence()
    {
        deactivateServantAtClose();
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // State Update Methods
    //
    ///////////////////////////////////////////////////////////////////////

    //
    // updateState
    //
    void Sequence::updateState(const TA_Base_Bus::SequenceCorbaDef::SequenceState& state)
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "updateState");
        TA_ASSERT(0 != m_pdSequence.get(), "The object handling the protocol dependent sequence tasks is NULL");
        m_pdSequence->updateState(state);
        LOG_FUNCTION_EXIT(SourceInfo, "updateState");
    }


    //
    // updateSequenceConfig
    //
    void Sequence::updateSequenceConfig(const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig)
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "updateSequenceConfig");
        TA_ASSERT(0 != m_pdSequence.get(), "The object handling the protocol dependent sequence tasks is NULL");
        m_pdSequence->updateSequenceConfig(sequenceConfig);
        const char* tmp = sequenceConfig.description;
        std::string desc = tmp;
        m_sequenceEntityData->setSequenceDescription(desc);
        LOG_FUNCTION_EXIT(SourceInfo, "updateSequenceConfig");
    }


    //
    // isReadOnly
    //
    bool Sequence::isReadOnly()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        return m_sequenceEntityData->isReadOnly();
    }

    //
    // getSequenceDescription
    char* Sequence::getSequenceDescription()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        return CORBA::string_dup((m_sequenceEntityData->getSequenceDescription()).c_str());
    }

    //
    // getDwellTimeRange
    //
    void Sequence::getDwellTimeRange(unsigned short& minimumDwellTime, unsigned short& maximumDwellTime)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        m_pdSequence->getDwellTimeRange(minimumDwellTime, maximumDwellTime);
    }


    //
    // getMaxNumberOfInputs
    //
    unsigned short Sequence::getMaxNumberOfInputs()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        return m_pdSequence->getMaxNumberOfInputs();
    }


    //
    // getSequenceConfig
    //
    TA_Base_Bus::SequenceCorbaDef::SequenceConfig* Sequence::getSequenceConfig()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        // Pass to protocol dependent component
        TA_Base_Bus::SequenceCorbaDef::SequenceConfig* seqConf = m_pdSequence->getSequenceConfig();
        seqConf->description = getSequenceDescription();
        return seqConf;
    }


    //
    // setSequenceConfig
    //
    void Sequence::setSequenceConfig(const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& newConfig, const char* sessionId)
    {
        if(isReadOnly())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Sequence configuration is read only");
            throw EXCEPTION_PERFORMING_REQUEST;
        }
        AgentModeMonitor::getInstance().verifyInControlMode(TA_Visual::SEQUENCE_SET_CONFIG);
        m_userPrivilegeVerifier.verifyRights(sessionId, m_sequenceEntityData,
                                             TA_Base_Bus::aca_EDIT_SEQUENCES,
                                             "Edit Sequence Configuration");
		m_pdSequence->setSequenceConfig(newConfig);
    }


    //
    // setSequenceDescription
    //
    void Sequence::setSequenceDescription(const char* newDescription, const char* sessionId)
    {
        if ( isReadOnly() )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Sequence configuration is read only" );
        }

        AgentModeMonitor::getInstance().verifyInControlMode( TA_Visual::SEQUENCE_SET_CONFIG );
        m_userPrivilegeVerifier.verifyRights( sessionId, m_sequenceEntityData,
                                              TA_Base_Bus::aca_EDIT_SEQUENCES, 
                                              "Edit Sequence Description" );

        try
        {
			m_sequenceEntityData->setSequenceDescription(newDescription);
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException" );
            throw TA_Base_Bus::VideoSwitchAgentException( e.what() );
        }  
    }


    //
    // cycleToNextVideoInput
    //
    void Sequence::cycleToNextVideoInput(unsigned long videoOutputEntityKey, const char* sessionId)
    {
        return;
    }


    //
    // cycleToPreviousVideoInput
    //
    void Sequence::cycleToPreviousVideoInput(unsigned long videoOutputEntityKey, const char* sessionId)
    {
        return;
    }


    //
    // pause
    //
    void Sequence::pause(unsigned long videoOutputEntityKey, const char* sessionId)
    {
        return;
    }


    //
    // play
    //
    void Sequence::play(unsigned long videoOutputEntityKey, const char* sessionId)
    {
        return;
    }


    //
    // stop
    //
    void Sequence::stop(unsigned long videoOutputEntityKey, const char* sessionId)
    {
        return;
    }


    //
    // start
    //
    void Sequence::start()
    {
        VideoInput::start();
    }


    //
    // stop
    //
    void Sequence::stop()
    {
        deactivateServant();
        VideoInput::stop();
    }


    //
    // getSequenceState
    //
    // Useless in c955
    TA_Base_Bus::SequenceCorbaDef::SequenceState Sequence::getSequenceState(unsigned long videoOutputEntityKey)
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();
        return m_pdSequence->getSequenceState(videoOutputEntityKey);
    }


    //
    // update
    //
    void Sequence::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        m_sequenceEntityData->invalidate();
        VideoInput::update(updateEvent);
    }


    //
    // remove
    //
    void Sequence::remove()
    {
    }


    //
    // setToControlMode
    //
    void Sequence::activateServantAtInit()
    {
        try
        {
            activateServantWithName(m_sequenceEntityData->getName());
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            // There is nothing we can do to recover.
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException", "A Sequence cannot be activated.");
        }
        catch(const TA_Base_Core::DataException& e)
        {
            // There is nothing we can do to recover.
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException", "A Sequence cannot be activated.");
        }
        catch(const TA_Base_Core::TransactiveException& e)
        {
            // There is nothing we can do to recover.
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "TransactiveException", "A Sequence cannot be activated.");
        }
        catch(...)
        {
            // There is nothing we can do to recover.
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "A Sequence cannot be activated.");
        }
    }


    //
    // setToMonitorMode
    //
    void Sequence::deactivateServantAtClose()
    {
        deactivateServant();
    }

}
