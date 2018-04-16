
#ifndef RADIO_PDS_MANAGER_H
#define RADIO_PDS_MANAGER_H

/* The source code in this file is the property of 
* ComBuilder Pte. Ltd and is not for redistribution
* in any form.
* Source:   $RadioPDSManager$
* @author:  Lucky
* @version: $Revision$
*
* Last modification: $Date$
* Last modified by:  $Author$
* 
* RadioPDSManager module manage and start all of the needed component 
* on PDS system.
* Manage the result of the requested operation and transfer the result thru SendMessage 
* Manage the call request from the train Thru IDataCall interface
* Create PDSDatagramServer that will manage the UDP service for PDS
*/

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEvent.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IDataCall.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSTTFObserver.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSHandler.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSSender.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IRadioPDSReceiver.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSEvent.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/PDSDatagramServer.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IRadioPDSStatusCallback.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/CommonTypes.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/exceptions/src/TransactiveException.h"

#include <map>
#include <queue>


namespace TA_IRS_App
{

	class RadioPDSManagerException : public TA_Base_Core::TransactiveException
	{
	public:
		RadioPDSManagerException( const std::string& errorText ) throw();
		virtual ~RadioPDSManagerException() throw();
	};

	class RadioPDSManager : public TA_Base_Core::Thread,
							public IDataCall,
							public IRadioPDSReceiver,
							public IPDSTTFObserver

	{

		public:
			// check if the thread is running or not
			bool IsRunning();
			
			/**
			 * Constructor
			 */
			RadioPDSManager(std::string strHost, std::string strPort, const TA_IRS_App::CommonTypes::SRadioPDSTimer& radioPDSTimer,IRadioPDSStatusCallback * radioPDSResCallback);

			/**
			 * Destructor
			 */
			virtual ~RadioPDSManager();

			/**
			 * Implementation for the IDataCall
			 */
			
			/**
			 * This sets up a data call to transfer from given data to the given TSI
			 *
			 * @param trainITsi The TSI to send the data to
			 * @param data    The raw bytes to send
			 */
			virtual void makeDataCall( const long & lTrainID, const TA_IRS_App::CommonTypes::RadioPDSMessage& data );

			/**
			 * This ends a data call in progress to the given ITSI.
			 */
			virtual void cancelDataCall( const long & lTrainID );

			/**
			 * Implementation for the IRadioPDSEventProcessor
			 */

			/**
			 * This is will process the incoming events.
			 * 
			 * @param event
			 */
			virtual void processEvent( TA_IRS_App::IPDSEvent * event );

			/**
			 * Implementation for the IPDSTTFObserver
			 */

			/**
			 * This will Notify if the Download is successful or fail
			 *
			 * @param bStatus
			 */
			virtual void NotifyTTSStatus( const long & lTrainID, bool success, const std::string& failReason );

			/**
			 * Notification received from Train Agent of EndofTtisDownload status
			 */
			virtual void NotifyEndOfTtisDownloadStatus( const unsigned long& lTrainID, unsigned short& sStatus );

			/**
			 * Overriding Thread
			 */
			virtual void run();

			/**
			 * Terminate this thread and all transfers
			 */
			virtual void terminate();

		private:
			
			/**
			 * Remove the CRadioPDSHandler object in a map with the given tsi
			 *
			 * @param tsi The reference of the object to be deleted.
			 */
			void removePDSHandlerObject( const long & lTrainID );

				/**
			 * This will end a call that is in progress. It will also remove the call from the ongoing calls list in the status manager.
			 *
			 * @param callId
			 */
			virtual void endDataCall( const long & lTrainID );

			
			TA_Base_Core::ReEntrantThreadLockable          m_transferListLock;
			TA_Base_Core::Semaphore                        m_semaphore;
			bool                                           m_running;
			// This is a list of finished transfers to remove from the list
			std::queue< long >							   m_removeList;
			TA_Base_Core::ReEntrantThreadLockable          m_removeListLock;

			//Timer
			TA_IRS_App::CommonTypes::SRadioPDSTimer m_radioPDSTimer;


			struct STransferProc
			{
				IRadioPDSSender	*                              radioPDSSender;
				CRadioPDSHandler*                              radioPDSHandler;

				STransferProc (IRadioPDSSender * pdsSender,CRadioPDSHandler * pdsHandler) 
				{
					radioPDSSender = pdsSender;
					radioPDSHandler = pdsHandler;
				}

				~STransferProc () 
				{
					delete radioPDSSender;
					delete radioPDSHandler;

					radioPDSSender = NULL;
					radioPDSHandler =  NULL;
				}
			};
		
			std::map < long, STransferProc * >			m_mapTranferRef;
			
			CPDSDatagramServer						  * m_pdsDatagramServer;
			IRadioPDSStatusCallback					  * m_radioResultCallback;

			static const unsigned long STATUS_FAILED;
	};
}

#endif //END of RADIO_PDS_MANAGER_H