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
#include "bus/trains/TrainProtocolLibrary/src/AbstractPecCommand.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip>

namespace TA_IRS_Bus
{
    const unsigned short AbstractPecCommand::MESSAGE_LENGTH = 9; //CL-20650
    const unsigned short AbstractPecCommand::PEC_NUMBER_POSITION = 5;
	const unsigned short AbstractPecCommand::PEC_TRAIN_SOURCE_POSITION = 6;


    AbstractPecCommand::~AbstractPecCommand()
    {
        FUNCTION_ENTRY( "~AbstractPecCommand" );
        FUNCTION_EXIT;
    }


    AbstractPecCommand::AbstractPecCommand( CommonTypes::TrainIdType trainId,
                                            TrainMessageType messageType,
                                            unsigned char origin,
                                            ProtocolPecTypes::EPecCarNumber carNumber,
                                            ProtocolPecTypes::EPecNumber pecNumber,
											bool responseExpected, PecTypes::EPecTrainSource pecTrainSource, unsigned short messageLength  )
      //  : TimsCommand( trainId, MESSAGE_LENGTH, messageType, origin, responseExpected ),
	  : TimsCommand( trainId, messageLength, messageType, origin, responseExpected ),
          m_carNumber( carNumber ),
          m_pecNumber( pecNumber ),
		   m_pecTrainSource( pecTrainSource ),
		  m_combinedPecByte( 0 ),
		  m_trainSource( 0 )		 
    {
        FUNCTION_ENTRY( "AbstractPecCommand" );

        m_combinedPecByte = ( ( m_carNumber & 0x0f ) << 4 ) | // take the lower 4 bits of the CAR number, and move to the top half of the byte
                            ( m_pecNumber & 0x0f );           // take the lower 4 bits of the PEC number, and put it in the bottom half of the byte

		switch(m_carNumber) 
		{
			case ProtocolPecTypes::CarOne:
				switch (m_pecNumber) 
				{
					case ProtocolPecTypes::PecOne:
						m_combinedPecByte = 1;
						break;
					case ProtocolPecTypes::PecTwo:
						m_combinedPecByte = 2;
						break;
					case ProtocolPecTypes::PecThree:
						m_combinedPecByte = 3;
						break;
					case ProtocolPecTypes::PecFour:
						m_combinedPecByte = 4;
						break;
					default:
						m_combinedPecByte = 0;
						// fall through
				};
				break;
			case ProtocolPecTypes::CarTwo:
				switch (m_pecNumber) 
				{
					case ProtocolPecTypes::PecOne:
						m_combinedPecByte = 5;
						break;
					case ProtocolPecTypes::PecTwo:
						m_combinedPecByte = 6;
						break;
					case ProtocolPecTypes::PecThree:
						m_combinedPecByte = 7;
						break;
					case ProtocolPecTypes::PecFour:
						m_combinedPecByte = 8;
						break;
					default:
						m_combinedPecByte = 0;
						// fall through
				};
				break;
			case ProtocolPecTypes::CarThree:
				switch (m_pecNumber) 
				{
					case ProtocolPecTypes::PecOne:
						m_combinedPecByte = 12;
						break;
					case ProtocolPecTypes::PecTwo:
						m_combinedPecByte = 11;
						break;
					case ProtocolPecTypes::PecThree:
						m_combinedPecByte = 10;
						break;
					case ProtocolPecTypes::PecFour:
						m_combinedPecByte = 9;
						break;
					default:
						m_combinedPecByte = 0;
						// fall through
				};
				break;
			default:
				m_combinedPecByte = 0;
				// fall through
		};

        setByte( PEC_NUMBER_POSITION, m_combinedPecByte );

		switch( m_pecTrainSource )
		{
		
			case PecTypes::LocalTrain:
				m_trainSource = 0;
				break;
			
			case PecTypes::RemoteTrain:
				m_trainSource = 1;
				break;

			default:
				m_trainSource = 0;
				// fall through

		}

		setByte( PEC_TRAIN_SOURCE_POSITION, m_trainSource );

        FUNCTION_EXIT;
    }


    std::string AbstractPecCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        // append the following:
        // "[Car=X][Pec=X][PecByte=X]"
        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[Car=" << static_cast<int>( m_carNumber ) << "]"
                  << "[Pec=" << static_cast<int>( m_pecNumber ) << "]"
                  << "[PecByte=" << std::hex << std::setw(2) << std::setfill('0')
                                 << static_cast<unsigned int>( m_combinedPecByte ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
