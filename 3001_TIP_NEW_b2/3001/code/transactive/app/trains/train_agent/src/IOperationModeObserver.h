/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  adamr
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */
#if !defined(IOperationModeObserver_H)
#define IOperationModeObserver_H

namespace TA_IRS_App
{

    /**
     * This is an interface used to notify observers that agent operation mode has changed.
     * This is train agent operation mode, not to be confused with Managed Agent operation states.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:42 PM
     */
    class IOperationModeObserver
    {

    public:

        /**
         * This is called when the train agent has acquired the control of a location.
         * All trains within the location are now under this agent's control.
         *
         * @param locationKey    This is the location key of the newly acquired location
         */
        virtual void locationControlAcquired( unsigned long locationKey ) = 0;


        /**
         * This is called when the train agent has released the control of a location.
         * All trains within the location are now under another agent's control.
         *
         * @param locationKey    This is the location key of the recently released location
         */
        virtual void locationControlReleased( unsigned long locationKey ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IOperationModeObserver_H)
