#ifndef STATIONPASYNCTISPROXY_H
#define STATIONPASYNCTISPROXY_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StationPaSyncTisProxy.h $
  * @author  A. Parker
  * @version $Revision: #4 $
  * Last modification: $Date: 2014/03/18 $
  * Last modified by:  $Author: huangjian $
  *
  *
  */


#include <string>
#include "ace/Mutex.h"

namespace TA_Base_Core
{
    class STISDestinationList;
}

namespace TA_IRS_App
{

/**
 * This class manages the state of broadcasts within the PA Agent.
 */
class StationPaSyncTisProxy
{
public:
    virtual ~StationPaSyncTisProxy();
    static StationPaSyncTisProxy& getInstance();
	static void destoryInstance();

public:
	void syncTisBroadcast( const std::vector<unsigned long>& vecZoneKeys, const std::vector<unsigned long>& vecMsgKeys, const unsigned long startTime, const unsigned long endTime, const std::string& strSessionId, int& globleState );
	void verifyTisParamters( const std::vector<unsigned long>& vecZoneKeys, const std::vector<unsigned long>& vecMsgKeys );
    void executeTisBroadcast( const int nTisLibSection, const unsigned long ulTisLibVersion, const unsigned short usTisMsgTag, const unsigned long startTime, const unsigned long endTime, const TA_Base_Core::STISDestinationList& lstTisDestinations, const std::string& strSessionId );

protected:
    StationPaSyncTisProxy();
    StationPaSyncTisProxy( StationPaSyncTisProxy& );
    StationPaSyncTisProxy& operator=( StationPaSyncTisProxy& );
    void fetchTisMsgParameters( const unsigned long ulMsgKey, int& nTisLibSection, unsigned long& ulTisLibVersion, unsigned short& usTisMsgTag );
	void fetchTisZones( const std::vector<unsigned long>& vecZoneKeys, TA_Base_Core::STISDestinationList& lstTisDestinations );
	void getEquipmentComponentAndVerify( const std::string& equipmentName, std::string& locationName, std::string& pidName );

private:
	static StationPaSyncTisProxy* s_pInstance;
    static ACE_Mutex s_mtxInstanceLock;
};

} // namespace TA_IRS_App

#endif // STATIONPASYNCTISPROXY_H

