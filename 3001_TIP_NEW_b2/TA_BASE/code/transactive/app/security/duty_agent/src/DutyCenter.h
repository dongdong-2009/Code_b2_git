#ifndef SECURITY_DUTY_CENTER_H__
#define SECURITY_DUTY_CENTER_H__

namespace TA_Base_App
{
	using namespace TA_Base_Bus;

	class DutyCenter
	{	
		enum StationStatus
		{
			Unknown=0,
			OnLine=1,
			OffLine=2,
		};
	public:
		DutyCenter(TA_Base_Bus::DutyPeerTaskManager& dutyPeerTaskManager);
		~DutyCenter();
	public:
		void	stationOnLine(unsigned long locaitonKey);
		void	stationOffLine(unsigned long locationKey);
		bool	isStationOnLine(unsigned long locationKey);
		TA_Base_Bus::DASType::LocationSet		getOffLineStations();
	private:
		void	broadcastStationStatus(unsigned long locationKey, StationStatus status);
		std::auto_ptr<IDutyPeer>		getLocalDutyPeer(unsigned long locationKey);
		std::auto_ptr<IDutyPeer>		getRemoteDutyPeer(unsigned long locationKey);
	private:
		//TA_Base_Core::ReEntrantThreadLockable 	m_lock;
		std::map<unsigned long,StationStatus>	m_stationStatusMap;
		TA_Base_Bus::DutyPeerTaskManager&		m_dutyPeerTaskManager;
		std::auto_ptr<TA_Base_Core::CommsMessageSender> m_commsSender;
	};
}

#endif