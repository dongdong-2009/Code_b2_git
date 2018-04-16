/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/PIDRightsManager.h $
* @author:  Wu Min Tao
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2009/12/15 19:31:45 $
* Last modified by:  $Author: grace.koh $
*
*/

#pragma once

#include "bus\signs\pids_agent\IDL\src\IPIDSManagerCorbaDef.h"
#include <string>
#include <map>

namespace TA_IRS_App
{

//class CPidManagerDialog;
class PIDSelectionManager
{
public:
	typedef enum
	{
		NONE,
		SELECT_ALL,
		DESELECT_ALL,
		SELECT_STATION_ALL,
		DESELECT_STATION_ALL,
		SELECT_STATION_ONE_TRACK,
		DESELECT_STATION_ONE_TRACK,
		SELECT_TRACK_ALL,
		DESELECT_TRACK_ALL,
		SELECT_TRAIN,
		DESELECT_TRAIN
	}Euoperator;
   

	//Return m_pidsList,because m_pidsList is not deconstruct,here return reference type
    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList& getPids();

	Euoperator parseString(std::string strOperate, std::string strValue);

    

	//Select all stations,all tracks,include train
    void selectAll();
    void deselectAll();  

	//Select one station all tracks
	void selectStationAll(std::string strStationName);
	void deselectStationAll(std::string strStationName);

	//Select one station,one track
	void selectStationOneTrack(std::string strStationName, std::string strTrack);	
	void deselectStationOneTrack(std::string strStationName, std::string strTrack);

	//Select one track all stations
	void selectTrackAll(std::string strTrack);
	void deselectTrackAll(std::string strTrack);

	//Select train
	void selectTrain();
	void deselectTrain();

	void setTrainSel(bool isSel);

	bool isTrainSelect();

	//set pidmanagerdiog send msg button state
	void setEnable();
	void setDisable();
	
	static PIDSelectionManager* getInstance();

	//Get location from m_locationNameStationIdMap,to avoid mem leak
	static unsigned long getStationIdFromName(std::string strName);  

	//help function,gen a list with all stations
	static TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList getAllStationsList();

    static const std::string TOKEN;

private:
    PIDSelectionManager();    
    TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList m_pidsList;

	bool m_isSelectTrain;

	//Store db access result
	static std::map<std::string,unsigned long> m_locationNameStationIdMap;
	static std::vector<std::string> m_locationsName;
    static const std::string UPTRACK;
    static const std::string DOWNTRACK;
    static const std::string CONC;
	static const std::string ALL;
	static const std::string TRAIN;
	static const std::string SELECT;
	static const std::string DESELECT;	
};

}