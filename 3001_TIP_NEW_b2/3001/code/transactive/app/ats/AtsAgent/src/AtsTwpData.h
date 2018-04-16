/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/AtsTwpData.h $
  * @author:   Darren Sampson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This represents the ATS Train Table
  */
#ifndef ATSTWPDATA_H
#define ATSTWPDATA_H

namespace TA_IRS_App
{
	class DataPointHelper;

    class AtsTwpData : public TA_Base_Core::ReEntrantThreadLockable
    {
    public:

		/**
		  * Constructor
		  *
		  */
		AtsTwpData(unsigned long entityKey, unsigned long locationKey, DataPointHelper& dpHelper);

		virtual ~AtsTwpData();

		//
		// IEntityUpdateEventProcessor Interface implementation
		//
		void updateValidity(bool valid);
		void updateNoWashControl(bool noWashControl);
		void updateDailyRinseWashControl(bool dailyRinseWashControl);
		void updateDailyDetergentWashControl(bool dailyDetergentWashControl);
		void updateWeeklyWashControl(bool weeklyWashControl);
		void updateTrainStoppedForCabEndWash(bool trainStoppedAtEndWp);
		void updateStopWpControl(bool stopWpControl);
		void updateRestartWpControl(bool restartWpControl);

		unsigned char getTwpValue(std::string dp, bool& valid);


		//
		//
		void sendUpdates();

		//Send all Command to TWP after Ats Link Recover From failed status.
		void ReSendUpdata();

	protected:
		
		bool checkAttribute(std::string attribute);

		//TD19853
		//bool controlTwpDataPoint(std::string command, bool bstatus);
		bool controlTwpDataPoint(std::string command, bool bstatus);
		// TD19853

    private:
        // Disable default constructor, copy constructor and assignment operator
		AtsTwpData();
        AtsTwpData( const AtsTwpData& atsWashRecord);
        AtsTwpData& operator=(const AtsTwpData &);

	protected:

		bool								m_valid;						
		bool								m_noWash;			
		bool								m_doNoWashControl;
		bool								m_dailyRinseWash;		
		bool								m_doDailyRinseWashControl;
		bool								m_dailyDetergentWash;	
		bool								m_doDailyDetergentWashControl;
		bool								m_weeklyWash;		
		bool								m_doWeeklyWashControl;
		bool								m_trainStoppedForCabEndWash;	
		bool								m_doTrainStoppedForCabEndWashControl;
		bool								m_stopWp;			
		bool								m_doStopWpControl;	
		bool								m_restartWp;	
		bool								m_doRestartWpControl;

		// when TWP link is Normal,if Laster timer command have diffener with previous time.
		bool                                m_bTwpNormalChange;
		// if TWP link Recover form false.
		//bool                                m_bTwpRecoverLink;

		//Remain previous Twp Link Status.
		bool                                m_bPreTwpLink;


		unsigned long						m_entityKey;
		unsigned long						m_locationKey;

		std::map<std::string, unsigned long> 
											m_twpOutputDataPoints;
		std::map<std::string, unsigned long> 
											m_twpInputDataPoints;

		std::map<std::string, unsigned char>  m_dpnametovalue;   // changxing add TD15264
		DataPointHelper&					m_dpHelper;
    };
}

#endif
