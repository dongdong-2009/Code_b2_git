/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include <sstream>

namespace TA_IRS_Bus
{

    namespace PecTypes
    {

        std::string pecUpdateTypeToString( EPecUpdateType pecUpdateType )
        {
            switch ( pecUpdateType )
            {

                case PECUpdated:
                    return "PECUpdated";
                    //break;	// not passable

                case PECReset:
                    return "PECReset";
                    //break;

                case PECMpuChangeover:
                    return "PECMpuChangeover";
                    //break;  // not passable

                case PECCallLost:
                    return "PECCallLost";
                    //break;  // not passable

				case PECHold:
					return "PECHold";
					//break;  // not passable
  
                default:
                    break;
            }

            return "Unknown EPecUpdateType";
        }


        std::string pecSourceToString( EPecSource pecSource )
        {
            switch ( pecSource )
            {

                case PecSourceUnassigned:
                    return "Unassigned";
                    //break;  // not passable

                case PecSourceDriver:
                    return "Driver";
                    //break;  // not passable

                case PecSourceOperator:
                    return "Operator";
                    //break; //not passable

                default:
                    break;
            }

            return "Unknown EPecSource";
        }
		
		std::string pecTrainSourceToString( EPecTrainSource pecTrainSource )
		{
			switch ( pecTrainSource )
			{

			case LocalTrain:
				return "Local Train";
				//break;	// not passable

			case RemoteTrain:
				return "Remote Train";
				//break;   // not passable

			default:
				break;
			}

			return "Unknown EPecTrainSource";
		}

        bool PecPoint::operator<( const PecPoint& rhs ) const
        {
            // first, compare car, then compare pec
            // so if car < rhs.car, return true
            // else if car == rhs.car,
            // if pec < rhs.pec ,return true
            // otherwise return false
            if ( car < rhs.car )
            {
                return true;
            }
            else if ( car == rhs.car )
            {
                if ( pec < rhs.pec )
                {
                    return true;
                }
            }
			
            return false;
        }


        bool PecPoint::operator==( const PecPoint& rhs ) const
        {
            // true if the car and pec match, nothing else considered.
            return ( ( rhs.car == car ) &&
                     ( rhs.pec == pec ) );
        }


        std::string PecPoint::getLogString() const
        {
            std::ostringstream result;

            result << "PecPoint [car: " << ProtocolPecTypes::pecCarNumberToString( car )
                   << "][pec: " << ProtocolPecTypes::pecNumberToString( pec )
                   << "][valid: " << valid
                   << "][source: " << pecSourceToString( source )
				   << "][pecTrainSource: " << pecTrainSourceToString( pecTrainSource )
                   << "][creationTime: " << creationTime
                   << "]";

            return result.str();
        }


        std::string activePecPointListToString( const ActivePecPointList& activePecPointList )
        {
            return structListToString< ActivePecPointList >( activePecPointList );
        }


        std::string PecUpdate::getLogString() const
        {
            std::ostringstream result;

            result << "PecUpdate [trainId: " << static_cast< unsigned int>( trainId )
                   << "][updateType: " << pecUpdateTypeToString( updateType )
                   << "][sessionId: " << sessionId
				   << "][primaryTsi: " << primaryTsi
				   << "][secondaryTsi: " << secondaryTsi
				   << "]" << pec.getLogString();

            return result.str();
        }


        std::string pecUpdateListToString( const PecUpdateList& pecUpdateList )
        {
            return structListToString< PecUpdateList >( pecUpdateList );
        }

    }

}
