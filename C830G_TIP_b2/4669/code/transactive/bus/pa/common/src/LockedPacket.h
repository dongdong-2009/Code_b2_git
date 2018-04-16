/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/common/src/LockedPacket.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Wraps a data packet in such a way that it can be returned from a function call
  * with a reference to some packet data, and retain a lock for the lifetime of the
  * object, allowing the packet lock to be released automatically when all copies
  * of this object are released (client responsible for ensuring data packet
  * not modified whilst lock is held)
  *
  */

#if !defined(LockedPacket_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
#define LockedPacket_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_

#include "core/synchronisation/src/IThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_Bus
{

    template <class Type> class LockedPacket
    {
    public:
    
        /**
         * LockedPacket (constructor)
         * 
         * By passing in the synchronizing lock, we maintain the lock until object destruction
         * the data is retained as a reference, it is up to the clients using this class
         * to ensure it isn't touched while the lock is held
         *
         * @param lock the lock controlling acess to data
         *
         * @param data the data that should not be modified while lock is held
         *
         */
        LockedPacket(TA_Base_Core::IThreadLockable& lock, Type& data)
            : 
            m_apSharedLock(new TA_Base_Core::ThreadGuard(lock)),    
            m_packetData(data)
            {}
    
    
        /**
         * LockedPacket (constructor)
         * 
         * Same as primary constructor, instead accepting an existing lock
         *
         * @param existingLock an existing lock object
         *
         * @param data the data that should not be modified while lock is held
         *
         */
        LockedPacket(std::auto_ptr<TA_Base_Core::ThreadGuard>& existingLock, Type& data)
            : 
            m_apSharedLock(existingLock),    
            m_packetData(data)
            {}
    
    
        /**
         * LockedPacket (copy constructor)
         *
         * Copy constructor required so we can return this object from
         * a function (when client code requesting the packet)
         * The shared lock pointer is (through auto_ptr) incremented with each
         * copy made, so the thread lock is only released when all instances
         * are destroyed (hence the packet is locked until all instances destroyed)
         *
         */
        LockedPacket( const LockedPacket& theLockedPacket)
            : 
            m_apSharedLock(const_cast<LockedPacket&>(theLockedPacket).m_apSharedLock),
            m_packetData(const_cast<LockedPacket&>(theLockedPacket).m_packetData)
            {}
           
        /**
         * ~LockedPacket (destructor)
         *
         * The auto_ptr will automatically clean up the shared lock as appropriate
         * when the correct reference count is reached
         */
        ~LockedPacket() {}
        
        /**
         * getLockedData
         *
         * @return the packet data in native form, which is locked for the lifetime of this object
         *
         */
        const Type& getLockedData() const { return m_packetData; }
    
        /**
         * getMutableData
         *
         * @return the packet data in native form, which is locked for the lifetime of this object
         *
         */
        Type& getMutableData() { return m_packetData; }
    
    private:
    
        // Assignment not used so it is made private
        LockedPacket& operator=(const LockedPacket&);
                
        // Auto pointer for the lock, so only released when all copies of this
        // class have been destroyed
        std::auto_ptr<TA_Base_Core::ThreadGuard>    m_apSharedLock;
    
        // Reference to the data 
        Type& m_packetData;
    };

} // end namespace TA_IRS_Bus
    
#endif // !defined(LockedPacket_3F7C474E_D4B5_4966_A962_1414544E17A6__INCLUDED_)
    