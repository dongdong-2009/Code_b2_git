#ifndef PA_SIM_MODBUS_FRAMES_H
#define PA_SIM_MODBUS_FRAMES_H
/*
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  */

/** @file PASimModbusFrames.h
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimModbusFrames.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Declaration file for classes that represent basic data types. They could
  * just as easily be structs, but that confuses most C++ programmers.
  */

// #include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"
// 
// namespace TA_Base_Core
// {
// 	class ISocket;
// }
// 
// namespace TA_IRS_App
// {
// namespace PA_Sim
// {
// 
// 
//     /**
//       *                                    +-- ModbusReadQuery
//       *               +--- ModbusQuery ----+
//       *               |                    +-- ModbusWriteQuery
//       *               |
//       * ModbusFrame --+
//       *               |
//       *               |                    +-- ModbusReadResponse
//       *               +-- ModbusResponse --+
//       *                                    +-- ModbusWriteResponse
//       *                              
//       * Frame   : contains data members
//       * Query   : implements fromNetwork()
//       * Response: implements toNetwork()
//       *
//       *
//       */
//     class ModbusFrame
//     {
//         public:
//             ModbusFrame(Byte functionCode);
//             virtual ~ModbusFrame();
// 
//             unsigned char   m_functionCode;
//             unsigned short  m_tableAddress;
//             unsigned short  m_tableSize;
//             unsigned char   m_exceptionCode;
//             unsigned short  m_crc;
//             ByteVector      m_tableData;
// 
//             TA_Base_Core::ISocket * getSocket() const;
//             void setSocket(TA_Base_Core::ISocket * socket);
// 
//         private:
//             TA_Base_Core::ISocket * m_socket;
//     };
// 
//     class ModbusQuery : public ModbusFrame
//     {
//         public:
//             ModbusQuery(Byte functionCode);
// 
//             virtual void fromNetwork(std::vector<unsigned char>::iterator & iter) = 0;
// 
//             virtual int getHeaderSize() const = 0;
//             virtual int getFooterSize() const = 0;
//     };
// 
//     class ModbusReadQuery : public ModbusQuery
//     {
//         public:
//             ModbusReadQuery();
// 
//             /**
//               *
//               * <b><u>Modified</u></b> Read Query Packet Format
//               *
//               * <PRE>
//               *    BYTE  | SIZE  |  DESCRIPTION           | DATA
//               * ---------+-------+------------------------+------------------
//               * ---------+-------+------------------------+------------------
//               * </PRE>
//               *
//               */
//             void fromNetwork(std::vector<unsigned char>::iterator & iter);
// 
//             /**
//               * constant representing the size of the header for a read query
//               * packet from the Modbus to the PA (master->slave)
//               */
//             virtual int getHeaderSize() const;
// 
//             /**
//               * constant representing the size of the header for a read query
//               * packet from the Modbus to the PA (master->slave)
//               */
//             virtual int getFooterSize() const;
// 
//     };
//         
//     class ModbusWriteQuery : public ModbusQuery
//     {
//         public:
//             ModbusWriteQuery();
// 
//             /**
//               * <b><u>Modified</u></b> Write Query Packet Format
//               *
//               * <PRE>
//               *     BYTE    |   SIZE    | DESCRIPTION          | DATA
//               * ------------+-----------+----------------------+------------
//               * ------------+-----------+----------------------+------------
//               * </PRE>
//               */
//             void fromNetwork(std::vector<unsigned char>::iterator & iter);
// 
//             /**
//               * constant representing the size of the header for a write query
//               * packet from the Modbus to the PA (master->slave)
//               */
//             virtual int getHeaderSize() const;
// 
//             /**
//               * constant representing the size of the header for a write query
//               * packet from the Modbus to the PA (master->slave)
//               */
//             virtual int getFooterSize() const;
// 
//     };
//         
//     class ModbusResponse : public ModbusFrame
//     {
//         public:
//             ModbusResponse(Byte functionCode);
// 
//             virtual void toNetwork(std::vector<unsigned char> & packet) = 0;
//     };
//         
//     class ModbusReadResponse : public ModbusResponse
//     {
//         public:
//             ModbusReadResponse();
// 
//             /**
//               *
//               * <b><u>Modified</u></b> Read Response Packet Format
//               *
//               * <PRE>
//               *    BYTE  | SIZE  |  DESCRIPTION           | DATA
//               * ---------+-------+------------------------+------------------------
//               * ---------+-------+------------------------+------------------------
//               * </PRE>
//               *
//               */
//             void toNetwork(std::vector<unsigned char> & packet);
//     };
//         
//     class ModbusWriteResponse : public ModbusResponse
//     {
//         public:
//             ModbusWriteResponse();
// 
//             /**
//               * <b><u>Modified</u></b> Write Response Packet Format
//               *
//               * <PRE>
//               *    BYTE  | SIZE  |  DESCRIPTION           | DATA
//               * ---------+-------+------------------------+------------------------
//               * ---------+-------+------------------------+------------------------
//               * </PRE>
//               */
//             void toNetwork(std::vector<unsigned char> & packet);
//     };
//         
// 
//     inline TA_Base_Core::ISocket * ModbusFrame::getSocket() const
//     {
//         return (m_socket);
//     }
// 
//     inline void ModbusFrame::setSocket(TA_Base_Core::ISocket * socket)
//     {
//         m_socket = socket;
//     }
// 
// 
// } // namespace PA_Sim
// } // namespace TA_IRS_App

/*
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  */

/** @file PASimModbusFrames.h
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimModbusFrames.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Declaration file for classes that represent basic data types. They could
  * just as easily be structs, but that confuses most C++ programmers.
  */
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Log_Msg.h"
#include "ace/Task.h"
#include "ace/ACE.h"

#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"

namespace TA_Base_Core
{
	class ISocket;
}

namespace TA_IRS_App
{
namespace PA_Sim
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
    // Standard modbus TCP/IP ADU
    //
    // |---------------------------------MODBUS TCP/IP ADU------------------------------------|
    // |-----MBAP Header(7 Bytes)----||-----Function Code(1 Byte)-----||---------Data---------|
    //                                |----------------------PDU------------------------------|

    //     BYTE  | SIZE  |  DESCRIPTION           | DATA
    //  ---------+-------+------------------------+------------------
    //      0    |   1   |  Device Address        | 0x00
    //  ---------+-------+------------------------+------------------
    //      1    |   1   |  Function Code         | 0x04
    //  ---------+-------+------------------------+------------------
    //     2-3   |   2   |  Starting Address      | 0x0064
    //           |       |  (a.k.a. Table Number) |
    //  ---------+-------+------------------------+------------------
    //     4-5   |   2   |  Number Of Words       | 0x0064
    //           |       |  To Read (Table Size)  |
    //  ---------+-------+------------------------+------------------
    //     6-7   |   2   |  CRC (zero)            | 0x0000
    //  ---------+-------+------------------------+------------------
    class ModbusFrame
    {
        public:
            ModbusFrame(Byte functionCode);
            virtual ~ModbusFrame();

            // Header
            unsigned short m_transactionIdentifier;
            unsigned short m_protocolIdentifier;
            unsigned short m_length;
            unsigned char  m_unitIdentifier;

            // Function code
            unsigned char   m_functionCode;

            // Error
            unsigned char   m_exceptionCode;

            /*TA_Base_Core::ISocket * getSocket() const;*/
            ACE_SOCK_Stream* getSocket() const;
            /*void setSocket(TA_Base_Core::ISocket * socket);*/
            void setSocket(ACE_SOCK_Stream* socket);
        private:
/*            TA_Base_Core::ISocket * m_socket;*/
            ACE_SOCK_Stream* m_socket;
    };

    class ModbusQuery : public ModbusFrame
    {
        public:
            unsigned short  m_startAddress;
            unsigned short  m_registersQuantity;

            ModbusQuery(Byte functionCode);

            virtual void fromNetwork(std::vector<unsigned char>::iterator & iter) = 0;
            virtual int getHeaderSize() const = 0;
    };



    class ModbusReadQuery : public ModbusQuery
    {
        public:

            ModbusReadQuery();

            void fromNetwork(std::vector<unsigned char>::iterator & iter);
            int getHeaderSize() const;

    };
        
    class ModbusWriteQuery : public ModbusQuery
    {
        public:

            size_t   m_byteCount;
            ByteVector      m_registersValue;

            ModbusWriteQuery();

            void fromNetwork(std::vector<unsigned char>::iterator & iter);
            int getHeaderSize() const;
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
            
            unsigned char   m_byteCount;
            ByteVector m_registersValue;

            ModbusReadResponse();

            void toNetwork(std::vector<unsigned char> & packet);
    };
        
    class ModbusWriteResponse : public ModbusResponse
    {
        public:
            unsigned short  m_startAddress;
            unsigned short  m_registersQuantity;

            ModbusWriteResponse();

            void toNetwork(std::vector<unsigned char> & packet);
    };
        

    inline ACE_SOCK_Stream * ModbusFrame::getSocket() const
    {
        return (m_socket);
    }

    inline void ModbusFrame::setSocket(ACE_SOCK_Stream * socket)
    {
        m_socket = socket;
    }


} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // #ifndef PA_SIM_MODBUS_FRAMES_H
