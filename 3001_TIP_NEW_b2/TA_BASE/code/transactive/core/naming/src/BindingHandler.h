#if !defined(BindingHandler_6E7C4003_609A_422f_B72F_119D019379D6__INCLUDED_)
#define BindingHandler_6E7C4003_609A_422f_B72F_119D019379D6__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/naming/src/BindingHandler.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2014/08/12 13:46:25 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This is a locally created object that wrappers the sending
  * and receiving of BindingRequestMessages
  * The executing thread will block querying this object.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <time.h>
#include <string>
#include "core/naming/IDL/src/BindingRequestOriginatorCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/Condition.h"

// yanrong++: CL-21230
#include "core/threads/src/IWorkItem.h"
// ++yanrong

//chenlei-- CL-21230
//Already defined in core/utilities/src/RunParams.h, no need to define here
//#define RPARAM_RESOLVE_TIMEOUT_MS "ResolveTimeoutMs"
//--chenlei

#include <sys/timeb.h>

namespace TA_Base_Core
{
	class BindingRequestMessageSender;
    class BindingHandler;
	class NSResovleWorkItem;
    class NamingThreadPoolSingleton;

	class ObjectRepositoryInfo
	{
	public:
		/** 
		  * ObjectRepositoryInfo
		  *
		  * Create a new ObjectRepositoryInfo with the given agent name
		  *
		  * @param std::string& agentName the agentname we are interested in
		  */
		ObjectRepositoryInfo( const std::string& agentName );

		/** 
		  * ~ObjectRepositoryInfo
		  *
		  * Destructor
		  */
		~ObjectRepositoryInfo();

		/** 
		  * getObject
		  *
		  * get an object from this repository
		  *
		  * @return CORBA::Object_ptr the returned object
		  *
		  * @param std::string& objectName the object to find
		  * 
		  * @exception NameNotFoundException thrown if unable
		  *					to resolve the ObjectRepository on the agent
		  */
		CORBA::Object_ptr getObject( const std::string& objectName );

		/** 
		  * setObjectRepository
		  *
		  * A non-threadguarded setObjectRepository (as called by BindingHandler).
		  * This will put the new repository in place as well as set the new timeToLive
		  *
		  * @param  NamedObjectRepositoryCorbaDef_ptr repository the new repository
		  * @param unsigned long timeToLive the new timeToLive
		  */
		void setObjectRepository( NamedObjectRepositoryCorbaDef_ptr repository, unsigned long timeToLive );

		/** 
		  * setObjectRepository_r
		  *
		  * A re-entrant aware setObjectRepository, as called from Naming.
		  *
		  * @param  NamedObjectRepositoryCorbaDef_ptr repository the new repository
		  * @param unsigned long timeToLive the new timeToLive
		  */
		void setObjectRepository_r( NamedObjectRepositoryCorbaDef_ptr repository, unsigned long timeToLive );

		/** 
		  * invalidate
		  *
		  * Invalidate this repository and set the time to live
		  * back to DEFAULT_TTL
		  */
		void invalidate();

		/** 
		  * getAgentName
		  *
		  * Get the name of the agent hosting this repository
		  *
		  * @return std::string the agent name
		  */
		std::string getAgentName() const;

	private:
		/** 
		  * resolveObjectRepository
		  *
		  * Resolve the object repository if it is the first time
		  * or if it has expired.
		  */
		void resolveObjectRepository();

		/** 
		  * resetExpiry
		  *
		  * Reset the expiry time for this cached object
		  */
		void resetExpiry( bool isExpired = false );

		// yanrong++: CL-21230
		/**
		  * nsResolve()
		  *
		  * Use NS resolve target agent servant
		  * @return The resolving result. Return true if resolve successfully otherwise return false; 
		  * Note: If this call return false we should send BindingRequestMsg to resolve the named object again.
		  */
		bool nsResolve(); // Interoperable Naming Service (INS)
		void nsResolveProcess(IWorkItem* item, const std::string& nsRefString);

		// gongzong++ CL-21311
		IWorkItemPtr queueNSResovleWorkItem(const std::string& nsRefString, unsigned long resolveTimeout);
		volatile int m_doubleFailedCounter;
		volatile bool m_bHasSuccess;
		NonReEntrantThreadLockable nslock;
		// ++gongzong

        //void pushFailedNsString(const std::string& nsstring);
        //void clearFailedNsString();
        //unsigned long getFailedNsStringCount();

		Condition		m_nsResolveCondition; // gongzong++ CL-21311 situation do not allow we use "Condition"
		std::string		m_primaryNSString;
		std::string		m_secondaryNSString;

		friend class NSResovleWorkItem;
		// ++yanrong

		/**
		* Store the agentname of the repository
		*/
		std::string m_agentName;

		/**
		* Store the repository
		*/
		NamedObjectRepositoryCorbaDef_ptr m_resolvedRepository;

		/**
		* Store the expiry time for this object
		*/
		time_t m_expiryTime;

		/**
		* Store the timeToLive
		*/
		unsigned long m_timeToLive;

		/**
		* Has resolution been attempted yet
		*/
		bool m_firstResolution;

        BindingHandler *m_bindingHandler;

		/**
		* protect the resolution
		*/
		ReEntrantThreadLockable m_lock;

		//chenlei++ CL-21230
		//guarantee that all name resolving requests for one agent are serial
		ReEntrantThreadLockable m_repoInfoLock;
		//++chenlei CL-21230

        TA_Base_Core::NamingThreadPoolSingleton* m_namingThreadPoolSingleton;
	};

	// yanrong++: CL-21230: WorkItem for NS resolving
	class NSResovleWorkItem: public TA_Base_Core::IWorkItem
	{
	public:
		NSResovleWorkItem( ObjectRepositoryInfo& callback, const std::string& nsString, unsigned long resolveTimeout );
		
		virtual void executeWorkItem();
		
	private:
		ObjectRepositoryInfo& m_callback;
		const std::string m_nsString;
		timeb m_startTime;
		unsigned long m_timeout;
	};
	// ++yanrong


	class BindingHandler : 
        public virtual POA_TA_Base_Core::BindingRequestOriginatorCorbaDef,
		public virtual TA_Base_Core::ServantBase
	{

	public:
		/**
		* constructor
		*
		* Construct a binding handler to resolve an object
		*/
		BindingHandler();

		/**
		* resolve
		*
		* Attempts to resolve the given object repository.  The calling thread will be
        * blocked until the callback is received or the timeout expires.
		*
		* @param	reposInfo	Details about the repository to resolve
		* @exception	NameNotFoundException if the name cannot be found
		*/
		void resolveObjectRepository( ObjectRepositoryInfo* reposInfo );

        /**
		* callback
		*
	    * this method is called by a binding request listener to deliver the requested
        * object reference
		*
		* @param	requestedObject	the resolved object. MUST NOT be NULL.
		* @param	timeToLive	The length of time the object will remain valid
		*				 in the cache (default 600 secs)
        */
		virtual void callback( NamedObjectRepositoryCorbaDef_ptr requestedObject, CORBA::ULong timeToLive );

	private:
        // NOT CLONEABLE
		BindingHandler(const BindingHandler& theBindingHandler);

		// Caches the resolved repository locally with the expiry time.
		ObjectRepositoryInfo* m_reposInfo;

		// Sender used to send binding request messages to object repositories.
		std::auto_ptr<BindingRequestMessageSender> m_bindingRequestSender;

        // Block thread execution until callback received or timeout expires.
        Condition m_condition;
	};


}; // TA_Base_Core
#endif // !defined(BindingHandler_6E7C4003_609A_422f_B72F_119D019379D6__INCLUDED_)
