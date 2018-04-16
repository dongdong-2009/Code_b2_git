/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/StationStatusHelper.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * StationStatusHelper contains helper functions for all functionality
  * common between station status dialogs (because the hierarchy isn't
  * split up to allow sharing of station specific functionality)
  *
  */

#if !defined(AFX_StationStatusHelper_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define AFX_StationStatusHelper_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include <string>

#include "app/pa/PAManager/src/GenericStatusModel.h"
#include "app/pa/PAManager/src/ListCtrlSort.h"


class StationStatusHelper
{
public:

    /**
     * addToStationReportLists
     *
     * @param failuresList the list control to receive the failure broadcast items
     *
     * @param progressList the list control to receive the successful broadcast items
     *
     * @param zoneProgresses the sequence of elements to be displayed in the controls
     *
     * @param isOCC true if it is at OCC
     *
     * @return false if there were any errors while adding items to lists
     *
     */
    static bool addToStationReportLists(
                    bool isOCC,
                    CListCtrlSort& failuresList, 
                    CListCtrlSort& progressList,
                     const TA_Base_Bus::IPAAgentCorbaDef::LocalProgressSeq& zoneProgresses);
    /**
     * retryStationBroadcast
     *
     * Retries a train specific broadcast, according to supplied parameters
     * 
     * @param agent the agent used to send out the request
     *
     * @param broadcastId the identifier of broadcast to be retried
     *
     * @param lockedProgressReport the progress report containing trains the request is
     *          to be done for
     *
     * @param mode the mode of the broadcast (ie retry all / retry failed)
     *
     * @exception BroadcastIdNotFound local copy of broadcast ID must be out of date
     *
     * @exception BadParameterException there were unexpected (malformed) input parameters
     *
     */
    static void retryStationBroadcast(PAAgentNamedObject& agent,
                                      const std::string& broadcastId,
                                      const LockedPacketProgressReport& lockedProgressReport,
                                      const GenericStatusModel::ERetryMode& mode);

    /**
     * setupReportWindowList
     *
     * Updates the specified list with some default properties to define a report
     * style window (that is, the Broadcast Progress and Broadcast Failures lists)
     * 
     * @param targetList the list to update the properties of
     *     
     * @param isFailureList true if the list being setup is (specifically) the failures list
     *
     * @param isOCC true if it is at OCC
     *
     */
    static void setupReportWindowList(
                                        bool isOCC,
                                        CListCtrlSort& targetList,                             
                                        bool isFailureList);
private:
       
    /**
     *
     * collectStationZones
     * 
     * Returns all zones within the current broadcast group, matching input parameters
     *
     * @param lockedProgressReport the progress report containing zones the request is
     *          to be done for
     *
     * @param includeFailedOnly set to true to include include zones that have a failed state
     *          
     * @return a list of zones keys, where for each zone, it's last known state does not match any of the states
     *          present in the input argument array
     *     
     */
    static TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage 
        collectStationZones(const LockedPacketProgressReport& lockedProgressReport,
                            bool includeFailedOnly);

    /**
     * addStationReportEntry
     *
     * Adds a new entry to the 'report' windows (items in list controls provided
     * as arguments)
     *
     * @param zoneProgress current status details of the zone
     *
     * @param failuresList reference to the failures list control on the dialog
     * 
     * @param progressList reference to the progress list control on the dialog
     *
     * @param isOCC true if it is at OCC
     *
     * @return false if there were any errors while adding items to lists
     *
     */
    static bool addStationReportEntry(
                                    bool isOCC,
                                    const TA_Base_Bus::IPAAgentCorbaDef::LocalProgress& zoneProgress,
                                    CListCtrlSort& failuresList,
                                    CListCtrlSort& progressList );

    /**
     * getStationStatusString
     *     
     * @param status the current zone status
     *
     * @return string representation of this status
     *
     */
    static const std::string StationStatusHelper::getStationStatusString(
                                            const TA_Base_Bus::IPAAgentCorbaDef::ELocalState& status);

};

#endif /* AFX_StationStatusHelper_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_ */
