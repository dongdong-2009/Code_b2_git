#ifndef COE_HELPER_H
#define COE_HELPER_H

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Rob Young
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * Some constants and static functions to assist in the creation and processing of COE Protocol agent
 * request and hardware notification commands.
 */

#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <boost/static_assert.hpp>
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    class IAgentRequestCommand;
    class COECommunicationsHandler;
    class COEVideoOutput;
    class COESequence;
}

namespace TA_IRS_App
{

    namespace TA_COE
    {
        struct AlarmState
        {
            unsigned long eventId;
            unsigned long stationId;
            bool operator<(const AlarmState& rhs) const
            {
                return stationId < rhs.stationId || (stationId == rhs.stationId && eventId < rhs.eventId);
            };

            bool operator==(const AlarmState& rhs) const
            {
                return eventId == rhs.eventId && stationId == rhs.stationId;
            };
        };

        typedef std::set< AlarmState > AlarmStack;


        // Signs position in command
        const unsigned int LSB_LENGTH_BYTE = 0;
        const unsigned int MSB_LENGTH_BYTE = 1;
        const unsigned int ERROR_BYTE      = 2;
        const unsigned int COMMAND_BYTE    = 3;
        const unsigned int FIRST_DATA_BYTE = 4;

        // Command length define
        const unsigned int NUM_LENGTH_BYTES = 2;
        const unsigned int PACKET_HEADER_LENGTH = 2; // Not including length bytes themselves
		const unsigned int MIN_VALID_RECEIVED_LENGTH = 4;
        const unsigned int MAXIMUM_PACKET_LENGTH = 65535;
        const unsigned short BYTE_HEX_CEILING = 0x0100;

        // Response code
        const unsigned char OK              = 0x00;
        const unsigned char NOT_IMPLEMENTED = 0x01;
        const unsigned char PROHIBITED      = 0x02;
        const unsigned char BAD_LENGTH      = 0x03;
        const unsigned char BAD_COMMAND     = 0x04;
        const unsigned char BUSY            = 0x05;

        // Command type
        const unsigned char CLEAR_COMMAND = 0x06;
        const unsigned char PROG_SEQ_COMMAND = 0x17;
        const unsigned char PROG_QUAD_COMMAND = 0x18;
        const unsigned char CAM_AND_MON_COMMAND = 0x21;
        const unsigned char SEQ_AND_MON_COMMAND = 0x22;
        const unsigned char PTZ_CTRL_COMMAND = 0x25;
        const unsigned char RECORDING_PLAYBACK_COMMAND = 0x27;
        const unsigned char PLAYBACK_CTRL_COMMAND = 0x28;
        const unsigned char SUMMARY_POLLING_COMMAND = 0x3a;

		inline bool isLittleEndian()
		{
			int one = 1;
			return *((char*)&one) > 0;
		}

        template<class T> std::string getPacketLogFormatStr(const T& packet)
        {
            FUNCTION_ENTRY("getPacketLogFormatStr");
            std::stringstream ss;
			typename T::const_iterator it;
			BOOST_STATIC_ASSERT(sizeof(*it) == 1);
            for(it = packet.begin(); it != packet.end(); ++it)
            {
                ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned short>((unsigned char)*it);
                ss << '.';
            }
            FUNCTION_EXIT;
            return ss.str();
        }

		template <class T> std::vector<char> convertValuetoLittleEndianByteArray(T value)
		{
			std::vector<char> t;
			if(isLittleEndian())
			{
				for(int i = 0; i < sizeof(value); ++i)
				{
					t.push_back(((char*)&value)[i]);
				}
			}
			else
			{
				for(int i = sizeof(value) - 1; i >= 0; --i)
				{
					t.push_back(((char*)&value)[i]);
				}
			}
			return t;
		}


        template<class T> void fillupDataInLittleEndian(typename std::vector<char>& dest,
														typename std::vector<char>::iterator& fillupFrom,
														T data)
        {
            TA_ASSERT(std::distance(fillupFrom, dest.end()) >= sizeof(data), "");
			std::vector<char> fillData = convertValuetoLittleEndianByteArray(data);
			fillupFrom = std::copy(fillData.begin(), fillData.end(), fillupFrom);
        }

        unsigned short getPacketLength(unsigned char leastSigByte, unsigned char mostSigByte);
        void setPacketLength(unsigned short packetLength, char& leastSigByte, char& mostSigByte);
        unsigned short getIdFromAddress(const std::string& address);
        unsigned short getIdFromByte(unsigned char idByte);
		std::pair< unsigned short, unsigned short > getParsedAddress(const std::string& address);
        // Data structure for using in process received data
        struct ReceivedDataProcessingInfo
        {
            unsigned char command;
            unsigned int minResponsePacketLength;
            unsigned int maxResponsePacketLength;
        };

        const unsigned int RECEIVED_DATA_PROCESSING_INFO_ARRAY_SIZE = 20;
        const ReceivedDataProcessingInfo RECEIVED_DATA_PROCESSING_INFO_ARRAY[ RECEIVED_DATA_PROCESSING_INFO_ARRAY_SIZE ] =
        {
            {
                CLEAR_COMMAND,
                MIN_VALID_RECEIVED_LENGTH,
                MIN_VALID_RECEIVED_LENGTH
            },

            {
                PROG_SEQ_COMMAND,
                MIN_VALID_RECEIVED_LENGTH,
                MIN_VALID_RECEIVED_LENGTH
            },

            {
                PROG_QUAD_COMMAND,
                MIN_VALID_RECEIVED_LENGTH,
                MIN_VALID_RECEIVED_LENGTH
            },

            {
                CAM_AND_MON_COMMAND,
                MIN_VALID_RECEIVED_LENGTH,
                MIN_VALID_RECEIVED_LENGTH
            },

            {
                SEQ_AND_MON_COMMAND,
                MIN_VALID_RECEIVED_LENGTH,
                MIN_VALID_RECEIVED_LENGTH
            },

            {
                PTZ_CTRL_COMMAND,
                MIN_VALID_RECEIVED_LENGTH,
                MIN_VALID_RECEIVED_LENGTH
            },

            {
                RECORDING_PLAYBACK_COMMAND,
                MIN_VALID_RECEIVED_LENGTH,
                MIN_VALID_RECEIVED_LENGTH
            },

            {
                PLAYBACK_CTRL_COMMAND,
                MIN_VALID_RECEIVED_LENGTH,
                MIN_VALID_RECEIVED_LENGTH
            },

            {
                SUMMARY_POLLING_COMMAND,
                MIN_VALID_RECEIVED_LENGTH,
                MIN_VALID_RECEIVED_LENGTH
            }
        };

    }
}

#endif // COE_HELPER_H
