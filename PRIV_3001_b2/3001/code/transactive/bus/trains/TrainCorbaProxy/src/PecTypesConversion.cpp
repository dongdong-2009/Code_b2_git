/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainCorbaProxy/src/PecTypesConversion.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Bus
{

    TA_Base_Bus::ITrainPecCorba::PecUpdateList PecTypesConversion::convertToCorba( const PecTypes::PecUpdateList& cppType )
    {
        TA_Base_Bus::ITrainPecCorba::PecUpdateList pecUpdateList;

        pecUpdateList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            pecUpdateList[i] = convertToCorba( cppType[i] );
        }

        return  pecUpdateList;
    }


    TA_Base_Bus::ITrainPecCorba::PecPointCollection PecTypesConversion::convertToCorba( const PecTypes::ActivePecPointList& cppType )
    {
        TA_Base_Bus::ITrainPecCorba::PecPointCollection pecPointCollection;

        PecTypes::ActivePecPointList::const_iterator ite = cppType.begin();

        pecPointCollection.length( cppType.size() );

        for ( unsigned long i = 0; ite != cppType.end(); ++i, ++ite )
        {
            pecPointCollection[i] = convertToCorba( *ite );
        }

        return  pecPointCollection;
    }


    TA_Base_Bus::ITrainExceptionsCorba::EPecCommandResult PecTypesConversion::convertToCorba( const ProtocolPecTypes::EPecCommandResult& cppType )
    {
        TA_Base_Bus::ITrainExceptionsCorba::EPecCommandResult ePecCommandResult;

        switch ( cppType )
        {

            case ProtocolPecTypes::PecProceeding:
                ePecCommandResult = TA_Base_Bus::ITrainExceptionsCorba::PecProceeding;
                break;

            case ProtocolPecTypes::PecInUse:
                ePecCommandResult = TA_Base_Bus::ITrainExceptionsCorba::PecInUse;
                break;

            case ProtocolPecTypes::PecFailed:
                ePecCommandResult = TA_Base_Bus::ITrainExceptionsCorba::PecFailed;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                // fall through if release version
        }

        return  ePecCommandResult;
    }


    TA_Base_Bus::ITrainPecCorba::PecPoint PecTypesConversion::convertToCorba( const PecTypes::PecPoint& cppType )
    {
        TA_Base_Bus::ITrainPecCorba::PecPoint pecPoint;

        pecPoint.car = convertToCorba( cppType.car );
        pecPoint.pec = convertToCorba( cppType.pec );
        pecPoint.valid = cppType.valid;
        pecPoint.source = convertToCorba( cppType.source );
        pecPoint.creationTime = cppType.creationTime;
		pecPoint.pecTrainSource = convertToCorba(cppType.pecTrainSource); //Lucky

        return  pecPoint;
    }


    TA_Base_Bus::ITrainPecCorba::PecUpdate PecTypesConversion::convertToCorba( const PecTypes::PecUpdate& cppType )
    {
        TA_Base_Bus::ITrainPecCorba::PecUpdate pecUpdate;

        pecUpdate.pec = convertToCorba( cppType.pec );
        pecUpdate.sessionId = cppType.sessionId.c_str();
        pecUpdate.trainId = cppType.trainId;
        pecUpdate.updateType = convertToCorba( cppType.updateType );
		pecUpdate.primaryTsi = cppType.primaryTsi;
		pecUpdate.secondaryTsi = cppType.secondaryTsi;
	
        return  pecUpdate;
    }


    TA_Base_Bus::ITrainPecCorba::EPecUpdateType PecTypesConversion::convertToCorba( const PecTypes::EPecUpdateType& cppType )
    {
        TA_Base_Bus::ITrainPecCorba::EPecUpdateType ePecUpdateType;

        switch ( cppType )
        {

            case PecTypes::PECUpdated:
                ePecUpdateType = TA_Base_Bus::ITrainPecCorba::PECUpdated;
                break;

            case PecTypes::PECReset:
                ePecUpdateType = TA_Base_Bus::ITrainPecCorba::PECReset;
                break;

            case PecTypes::PECCallLost:
                ePecUpdateType = TA_Base_Bus::ITrainPecCorba::PECCallLost;
                break;

            case PecTypes::PECMpuChangeover:
                ePecUpdateType = TA_Base_Bus::ITrainPecCorba::PECMpuChangeover;
                break;

			case PecTypes::PECHold:
				ePecUpdateType = TA_Base_Bus::ITrainPecCorba::PECHold;
				break;


            default:
                TA_ASSERT( false, "Invalid Enum value" );
                // fall through if release version
        }

        return  ePecUpdateType;
    }


    TA_Base_Bus::ITrainPecCorba::EPecCarNumber PecTypesConversion::convertToCorba( const ProtocolPecTypes::EPecCarNumber& cppType )
    {
        TA_Base_Bus::ITrainPecCorba::EPecCarNumber ePecCarNumber;

        switch ( cppType )
        {

            case ProtocolPecTypes::AllCars:
                ePecCarNumber = TA_Base_Bus::ITrainPecCorba::AllCars;
                break;

            case ProtocolPecTypes::CarOne:
                ePecCarNumber = TA_Base_Bus::ITrainPecCorba::CarOne;
                break;

            case ProtocolPecTypes::CarTwo:
                ePecCarNumber = TA_Base_Bus::ITrainPecCorba::CarTwo;
                break;

            case ProtocolPecTypes::CarThree:
                ePecCarNumber = TA_Base_Bus::ITrainPecCorba::CarThree;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                // fall through if release version
        }

        return  ePecCarNumber;
    }


    TA_Base_Bus::ITrainPecCorba::EPecNumber PecTypesConversion::convertToCorba( const ProtocolPecTypes::EPecNumber& cppType )
    {
        TA_Base_Bus::ITrainPecCorba::EPecNumber ePecNumber;

        switch ( cppType )
        {

            case ProtocolPecTypes::AllPec:
                ePecNumber = TA_Base_Bus::ITrainPecCorba::AllPec;
                break;

            case ProtocolPecTypes::PecOne:
                ePecNumber = TA_Base_Bus::ITrainPecCorba::PecOne;
                break;

            case ProtocolPecTypes::PecTwo:
                ePecNumber = TA_Base_Bus::ITrainPecCorba::PecTwo;
                break;

            case ProtocolPecTypes::PecThree:
                ePecNumber = TA_Base_Bus::ITrainPecCorba::PecThree;
                break;

            case ProtocolPecTypes::PecFour:
                ePecNumber = TA_Base_Bus::ITrainPecCorba::PecFour;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                // fall through if release version
        }

        return  ePecNumber;
    }


    TA_Base_Bus::ITrainPecCorba::EPecSource PecTypesConversion::convertToCorba( const PecTypes::EPecSource& cppType )
    {
        TA_Base_Bus::ITrainPecCorba::EPecSource ePecSource;

        switch ( cppType )
        {

            case PecTypes::PecSourceUnassigned:
                ePecSource = TA_Base_Bus::ITrainPecCorba::PecSourceUnassigned;
                break;

            case PecTypes::PecSourceDriver:
                ePecSource = TA_Base_Bus::ITrainPecCorba::PecSourceDriver;
                break;

            case PecTypes::PecSourceOperator:
                ePecSource = TA_Base_Bus::ITrainPecCorba::PecSourceOperator;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                // fall through if release version
        }

        return  ePecSource;
    }

	TA_Base_Bus::ITrainPecCorba::EPecTrainSource PecTypesConversion::convertToCorba(const PecTypes::EPecTrainSource& cppType)
	{
		TA_Base_Bus::ITrainPecCorba::EPecTrainSource pecTrainSource;

		switch( cppType )
		{

			case PecTypes::LocalTrain:
				pecTrainSource = TA_Base_Bus::ITrainPecCorba::LocalTrain;
				break;

			case PecTypes::RemoteTrain:
				pecTrainSource = TA_Base_Bus::ITrainPecCorba::RemoteTrain;
				break;
			default:;
			// fall through if release version
		}
		return pecTrainSource;
	}


    PecTypes::ActivePecPointList PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPecCorba::PecPointCollection& CorbaType )
    {
        PecTypes::ActivePecPointList activePecPointList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            activePecPointList.insert( convertToCpp( CorbaType[i] ) );
        }

        return  activePecPointList;
    }


    PecTypes::EPecSource PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPecCorba::EPecSource& CorbaType )
    {
        PecTypes::EPecSource ePecSource;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainPecCorba::PecSourceUnassigned:
                ePecSource = PecTypes::PecSourceUnassigned;
                break;

            case TA_Base_Bus::ITrainPecCorba::PecSourceDriver:
                ePecSource = PecTypes::PecSourceDriver;
                break;

            case TA_Base_Bus::ITrainPecCorba::PecSourceOperator:
                ePecSource = PecTypes::PecSourceOperator;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
				// fall through if release version
        }

        return  ePecSource;
    }

	PecTypes::EPecTrainSource PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPecCorba::EPecTrainSource& CorbaType)
	{
		PecTypes::EPecTrainSource pecTrainSource;

		switch( CorbaType )
		{

		case TA_Base_Bus::ITrainPecCorba::LocalTrain:
			pecTrainSource = PecTypes::LocalTrain;
			break;

		case TA_Base_Bus::ITrainPecCorba::RemoteTrain:
			pecTrainSource = PecTypes::RemoteTrain;
			break;

		default:
			TA_ASSERT( false, "Invalid Enum value");
			// fall through if release version
		}

		return pecTrainSource;
	}


    ProtocolPecTypes::EPecNumber PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPecCorba::EPecNumber& CorbaType )
    {
        ProtocolPecTypes::EPecNumber ePecNumber;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainPecCorba::AllPec:
                ePecNumber = ProtocolPecTypes::AllPec;
                break;

            case TA_Base_Bus::ITrainPecCorba::PecOne:
                ePecNumber = ProtocolPecTypes::PecOne;
                break;

            case TA_Base_Bus::ITrainPecCorba::PecTwo:
                ePecNumber = ProtocolPecTypes::PecTwo;
                break;

            case TA_Base_Bus::ITrainPecCorba::PecThree:
                ePecNumber = ProtocolPecTypes::PecThree;
                break;

            case TA_Base_Bus::ITrainPecCorba::PecFour:
                ePecNumber = ProtocolPecTypes::PecFour;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                // fall through if release version
        }

        return  ePecNumber;
    }


    ProtocolPecTypes::EPecCarNumber PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPecCorba::EPecCarNumber& CorbaType )
    {
        ProtocolPecTypes::EPecCarNumber ePecCarNumber;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainPecCorba::AllCars:
                ePecCarNumber = ProtocolPecTypes::AllCars;
                break;

            case TA_Base_Bus::ITrainPecCorba::CarOne:
                ePecCarNumber = ProtocolPecTypes::CarOne;
                break;

            case TA_Base_Bus::ITrainPecCorba::CarTwo:
                ePecCarNumber = ProtocolPecTypes::CarTwo;
                break;

            case TA_Base_Bus::ITrainPecCorba::CarThree:
                ePecCarNumber = ProtocolPecTypes::CarThree;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                // fall through if release version
        }

        return  ePecCarNumber;
    }


    PecTypes::EPecUpdateType PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPecCorba::EPecUpdateType& CorbaType )
    {
        PecTypes::EPecUpdateType ePecUpdateType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainPecCorba::PECUpdated:
                ePecUpdateType = PecTypes::PECUpdated;
                break;

            case TA_Base_Bus::ITrainPecCorba::PECReset:
                ePecUpdateType = PecTypes::PECReset;
                break;

            case TA_Base_Bus::ITrainPecCorba::PECCallLost:
                ePecUpdateType = PecTypes::PECCallLost;
                break;

            case TA_Base_Bus::ITrainPecCorba::PECMpuChangeover:
                ePecUpdateType = PecTypes::PECMpuChangeover;
                break;

			case TA_Base_Bus::ITrainPecCorba::PECHold:
				ePecUpdateType = PecTypes::PECHold;
				break;


            default:
                TA_ASSERT( false, "Invalid Enum value" );
                // fall through if release version
        }

        return  ePecUpdateType;
    }


    PecTypes::PecUpdate PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPecCorba::PecUpdate& CorbaType )
    {
        PecTypes::PecUpdate pecUpdate;

        pecUpdate.pec = convertToCpp( CorbaType.pec );
        pecUpdate.sessionId = CorbaType.sessionId.in();
        pecUpdate.trainId = CorbaType.trainId;
        pecUpdate.updateType = convertToCpp( CorbaType.updateType );
		pecUpdate.primaryTsi = CorbaType.primaryTsi;
		pecUpdate.secondaryTsi = CorbaType.secondaryTsi;

        return  pecUpdate;
    }


    PecTypes::PecPoint PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPecCorba::PecPoint& CorbaType )
    {
        PecTypes::PecPoint pecPoint;

        pecPoint.car = convertToCpp( CorbaType.car );
        pecPoint.pec = convertToCpp( CorbaType.pec );
        pecPoint.valid = CorbaType.valid;
        pecPoint.source = convertToCpp( CorbaType.source );
        pecPoint.creationTime = CorbaType.creationTime;
		pecPoint.pecTrainSource = convertToCpp( CorbaType.pecTrainSource ); //Lucky

        return  pecPoint;
    }


    PecTypes::PecUpdateList PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainPecCorba::PecUpdateList& CorbaType )
    {
        PecTypes::PecUpdateList pecUpdateList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            pecUpdateList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return  pecUpdateList;
    }


    ProtocolPecTypes::EPecCommandResult PecTypesConversion::convertToCpp( const TA_Base_Bus::ITrainExceptionsCorba::EPecCommandResult& CorbaType )
    {
        ProtocolPecTypes::EPecCommandResult ePecCommandResult;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainExceptionsCorba::PecProceeding:
                ePecCommandResult = ProtocolPecTypes::PecProceeding;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::PecInUse:
                ePecCommandResult = ProtocolPecTypes::PecInUse;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::PecFailed:
                ePecCommandResult = ProtocolPecTypes::PecFailed;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
				// fall through if release version
        }

        return  ePecCommandResult;
    }
}
