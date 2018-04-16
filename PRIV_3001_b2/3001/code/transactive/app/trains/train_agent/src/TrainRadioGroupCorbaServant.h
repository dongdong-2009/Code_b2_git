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
#if !defined(TrainRadioGroupCorbaServant_H)
#define TrainRadioGroupCorbaServant_H


#include "bus/trains/TrainAgentCorba/idl/src/ITrainRadioGroupCorba.h"

#include "core/corba/src/ServantBase.h"


namespace TA_IRS_App
{
    class IRadioGroupCoordinator;
}


namespace TA_IRS_App
{

    /**
     * This is the corba servant for the ITrainRadioGroupCorba interface
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:33 PM
     */
    class TrainRadioGroupCorbaServant : public TA_Base_Core::ServantBase,
                                        public POA_TA_Base_Bus::ITrainRadioGroupCorba
    {

    public:

        /**
         *
         * @param radioGroupCoordinator    Used to delegate requests to
         */
        TrainRadioGroupCorbaServant( IRadioGroupCoordinator& radioGroupCoordinator );


        /**
         * Destructor
         */
        virtual ~TrainRadioGroupCorbaServant();


        /**
         * Gets the train radio groups available to choose from.
         * They each indicate what areas they cover.
         */
        TA_Base_Bus::ITrainRadioGroupCorba::TrainRadioGroupList* getTrainRadioGroups();


    private:


        /**
         * Private default constructor
         */
        TrainRadioGroupCorbaServant();


        /**
         * Used to delegate calls to
         */
        IRadioGroupCoordinator& m_radioGroupCoordinator;

    };

} // TA_IRS_App

#endif // !defined(TrainRadioGroupCorbaServant_H)
