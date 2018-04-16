#ifndef PA_SIM_MODBUS_TABLE_MANAGER_H
#define PA_SIM_MODBUS_TABLE_MANAGER_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimStandardTCPModbusTableManager.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Declaration file for PASimTablePATrain class
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/src/PASimTable.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimStandardModbusTcpTable.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableEnum.h"

namespace TA_IRS_App
{
     class AbstractScenario;
namespace PA_Sim
{
    /**
      * @class PASimTablePATrain
      *
      * PASimTablePATrain is a derived table class.
      *
      * TABLE PATrain 
      *
      */

    class PASimStandardTcpModbusTablesManager
    {

    public:
        /**
          * Preferred constructor
          * @param parent the owner of this object
          */
        PASimStandardTcpModbusTablesManager(PASimTableQueryProcessor * parent, LocationType loc);

        /**
          * Destructor
          */
        ~PASimStandardTcpModbusTablesManager();

        bool doInitialize();
        bool addTable(PASimTableStandardModbus** header, PASimTableStandardModbus* newNode);
        void destroyTree(PASimTableStandardModbus* header);
        //void inorderTravel(PASimTableStandardModbus);

        PASimTable* find(int startAddress);
        PASimTableStandardModbus* search(PASimTableStandardModbus* node, int address);
        void setScenario(AbstractScenario* p);
        AbstractScenario* getScenario();
        PASimTableQueryProcessor* getQueryProcessor();
        LocationType getLocationType();

        void getMatrixRoutingTableAddressIsOne(std::vector<int>& address);
        void getPAZoneStatusTableAddressIsOne(std::vector<unsigned long>& address);
        void getSpeakerLineTableAddressIsOne(std::vector<int>& address);
        void resetMatrixRoutingTable();
        int  getRegisterValue(int address);
        int  setRegisterValue(int address, int val);
		int  setMultiRegistersValues(int startAddress, int endAddress, ByteVector& data);

    private:

        PASimTableQueryProcessor* m_PASimTableQueryProcessor;
        LocationType m_location;
        PASimTableStandardModbus* m_tableBTreeHeader;
        
        AbstractScenario* m_scenario;

    }; // class PASimStandardTcpModbusManager

} // namespace PA_Sim
} // namespace TA_IRS_App
#endif // #ifndef PA_SIM_MODBUS_TABLE_MANAGER_H
