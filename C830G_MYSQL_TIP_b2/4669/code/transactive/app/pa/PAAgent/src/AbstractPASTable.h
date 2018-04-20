/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/AbstractPASTable.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(AbstractPASTable_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define AbstractPASTable_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "core/types/src/ta_types.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

/**
 * Super class for all Table classes to inherit.
 */
class AbstractPASTable
{
public:
    /** Destructs an instance of this class. */
    virtual ~AbstractPASTable() 
    {
        // ++ AZ bug774 (TD13071)
        delete [] m_buffer;
        // ++ AZ
    };


    enum EPasWriteErrors
    {
        PAS_ERR_OK                      = 0,
        PAS_ERR_GENERIC                 = 1,
        PAS_ERR_BUSY_MESSAGE_SEQUENCE   = 2,
        PAS_ERR_BUSY_SOURCE             = 3,
        PAS_ERR_BUSY_RECORDING          = 4,
        PAS_ERR_BUSY_BROADCASTING       = 5,
        PAS_ERR_INVALID_ANNOUNCE        = 6,
        PAS_ERR_NO_FLAG                 = 7,
        PAS_ERR_NO_FIRE                 = 8
    };

protected:
    /**
     * Constructs an instance of this class.
     * @param tableNumber The table number of the table the instance will store.
     * @param bufferSize  The size of the buffer for this table.
     */
    AbstractPASTable( unsigned long tableNumber, unsigned int bufferSize );

private:

    AbstractPASTable( const AbstractPASTable& theAbstractPASTable);
    

protected:

    /** The table number of the PAS table this instance will represent. */
    const unsigned long                    TABLE_NUMBER;
    /** The buffer size. */
    const unsigned long                    BUFFER_SIZE;
    /** Thread lock to protect the buffer. */
	
	// Maochun Sun++
	// TD13413
    //TA_Base_Core::NonReEntrantThreadLockable m_lock;
	TA_Base_Core::ReEntrantThreadLockable m_lock;
	// ++Maochun Sun
	// TD13413

    /** The buffer that will contain all the data from the PAS. */
    ta_uint8*                              m_buffer;
};

#endif // !defined(AbstractPASTable_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

