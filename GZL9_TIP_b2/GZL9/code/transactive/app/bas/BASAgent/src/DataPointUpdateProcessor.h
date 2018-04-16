/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/DataPointUpdateProcessor.h $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#if !defined(APP_BAS_DATAPOINTUPDATEPROCESSOR_H)
#define APP_BAS_DATAPOINTUPDATEPROCESSOR_H

#include <set>
#include "ace/Singleton.h"

#include "app/bas/BASAgent/src/IDataPointUpdateObserver.h"
#include "bus/scada/datapoint_library/src/IDataPointObserverNotification.h"


namespace TA_IRS_App
{
    class DataPointUpdateProcessorImpl: public TA_Base_Bus::IDataPointObserverNotification, 
										public IDataPointUpdateObserver
    {
    public:
        friend class ACE_Singleton<DataPointUpdateProcessorImpl, ACE_Recursive_Thread_Mutex>;

        void registerDatapointUpdate(const unsigned long pkey, IDataPointUpdateObserver* pUpdateObserver);
        void deregisterDatapointUpdate(const unsigned long pkey, IDataPointUpdateObserver* pUpdateObserver);

        virtual void processDataPointUpdate ( unsigned long pkey, boost::shared_ptr<TA_Base_Bus::DpDynamicSyncUpdate> updateDetails );

        void registerBasParameterChangeEvent();
        void registerFireModeChange();

        virtual void processDataPointControlStateUpdate(const unsigned long dpEntityKey, const int nControlState, const TA_Base_Bus::DataPointControl& controlState);
        virtual void processDataPointValueUpdate(const unsigned long dpEntityKey, const TA_Base_Bus::DataPointValue& updateValue );

    private:
        DataPointUpdateProcessorImpl();
        ~DataPointUpdateProcessorImpl();
        
        DataPointUpdateProcessorImpl(const DataPointUpdateProcessorImpl&);
        const DataPointUpdateProcessorImpl& operator=(const DataPointUpdateProcessorImpl&);

        typedef std::map<unsigned long, std::set<IDataPointUpdateObserver*> > DataPointUpdateNofificationObserverMap;
        TA_Base_Core::ReEntrantThreadLockable m_dpUpdateObserverMapLock;
        DataPointUpdateNofificationObserverMap m_dpUpdateObserverMap;

        std::set<unsigned long> m_basParameterValueDPSet;
        std::set<unsigned long> m_basFireModeRecoverDPSet;
    };
	
    typedef ACE_Singleton<DataPointUpdateProcessorImpl, ACE_Recursive_Thread_Mutex> DataPointUpdateProcessor;
}

#endif //APP_BAS_DATAPOINTUPDATEPROCESSOR_H