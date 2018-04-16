#ifndef ATS_SIM_MODBUS_FRAMES_H
#define ATS_SIM_MODBUS_FRAMES_H
/*
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  */

/** @file ATSSimModbusFrames.h
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimModbusFrames.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Declaration file for classes that represent basic data types. They could
  * just as easily be structs, but that confuses most C++ programmers.
  */

#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"

namespace TA_Base_Core
{
	class ISocket;
}

namespace TA_IRS_App
{
namespace ATS_Sim
{


    /**
      *                                    +-- ModbusReadQuery
      *               +--- ModbusQuery ----+
      *               |                    +-- ModbusWriteQuery
      *               |
      * ModbusFrame --+
      *               |
      *               |                    +-- ModbusReadResponse
      *               +-- ModbusResponse --+
      *                                    +-- ModbusWriteResponse
      *                              
      * Frame   : contains data members
      * Query   : implements fromNetwork()
      * Response: implements toNetwork()
      *
      *
      */
    class ModbusFrame
    {
        public:
            ModbusFrame(Byte functionCode);
            virtual ~ModbusFrame();

            unsigned char   m_functionCode;
            unsigned short  m_tableAddress;
            unsigned short  m_tableSize;
            unsigned char   m_exceptionCode;
            unsigned short  m_crc;
            ByteVector      m_tableData;

            TA_Base_Core::ISocket * getSocket() const;
            void setSocket(TA_Base_Core::ISocket * socket);

        private:
            TA_Base_Core::ISocket * m_socket;
    };

    class ModbusQuery : public ModbusFrame
    {
        public:
            ModbusQuery(Byte functionCode);

            virtual void fromNetwork(std::vector<unsigned char>::iterator & iter) = 0;

            virtual int getHeaderSize() const = 0;
            virtual int getFooterSize() const = 0;
    };

    class ModbusReadQuery : public ModbusQuery
    {
        public:
            ModbusReadQuery();

            /**
              *
              * <b><u>Modified</u></b> Read Query Packet Format
              *
              * <PRE>
              *    BYTE  | SIZE  |  DESCRIPTION           | DATA
              * ---------+-------+------------------------+------------------
              * ---------+-------+------------------------+------------------
              * </PRE>
              *
              */
            void fromNetwork(std::vector<unsigned char>::iterator & iter);

            /**
              * constant representing the size of the header for a read query
              * packet from the Modbus to the ATS (master->slave)
              */
            virtual int getHeaderSize() const;

            /**
              * constant representing the size of the header for a read query
              * packet from the Modbus to the ATS (master->slave)
              */
            virtual int getFooterSize() const;

    };
        
    class ModbusWriteQuery : public ModbusQuery
    {
        public:
            ModbusWriteQuery();

            /**
              * <b><u>Modified</u></b> Write Query Packet Format
              *
              * <PRE>
              *     BYTE    |   SIZE    | DESCRIPTION          | DATA
              * ------------+-----------+----------------------+------------
              * ------------+-----------+----------------------+------------
              * </PRE>
              */
            void fromNetwork(std::vector<unsigned char>::iterator & iter);

            /**
              * constant representing the size of the header for a write query
              * packet from the Modbus to the ATS (master->slave)
              */
            virtual int getHeaderSize() const;

            /**
              * constant representing the size of the header for a write query
              * packet from the Modbus to the ATS (master->slave)
              */
            virtual int getFooterSize() const;

    };
        
    class ModbusResponse : public ModbusFrame
    {
        public:
            ModbusResponse(Byte functionCode);

            virtual void toNetwork(std::vector<unsigned char> & packet) = 0;
    };
        
    class ModbusReadResponse : public ModbusResponse
    {
        public:
            ModbusReadResponse();

            /**
              *
              * <b><u>Modified</u></b> Read Response Packet Format
              *
              * <PRE>
              *    BYTE  | SIZE  |  DESCRIPTION           | DATA
              * ---------+-------+------------------------+------------------------
              * ---------+-------+------------------------+------------------------
              * </PRE>
              *
              */
            void toNetwork(std::vector<unsigned char> & packet);
    };
        
    class ModbusWriteResponse : public ModbusResponse
    {
        public:
            ModbusWriteResponse();

            /**
              * <b><u>Modified</u></b> Write Response Packet Format
              *
              * <PRE>
              *    BYTE  | SIZE  |  DESCRIPTION           | DATA
              * ---------+-------+------------------------+------------------------
              * ---------+-------+------------------------+------------------------
              * </PRE>
              */
            void toNetwork(std::vector<unsigned char> & packet);
    };
        

    inline TA_Base_Core::ISocket * ModbusFrame::getSocket() const
    {
        return (m_socket);
    }

    inline void ModbusFrame::setSocket(TA_Base_Core::ISocket * socket)
    {
        m_socket = socket;
    }


} // namespace ATS_Sim
} // namespace TA_IRS_App
#endif // #ifndef ATS_SIM_MODBUS_FRAMES_H
