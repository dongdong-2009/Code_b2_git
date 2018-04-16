/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/trainborne_cctv/trainborne_cctv_access_library/src/TrainborneCCTVAccessLibrary.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This library provides a layer of abstraction for activating a train's CCTV
  * camera and displaying it in a GUI.
  */


#ifndef TRAINBORNE_CCTV_ACCESS_LIBRARY
#define TRAINBORNE_CCTV_ACCESS_LIBRARY


#include <string>
#include "bus/trains/TrainAgentCorbaDef/IDL/src/ITrainCorbaDef.h"
#include "core/naming/src/FlexibleTimeoutNamedObject.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"



namespace TA_IRS_Bus
{

    class TrainborneCCTVAccessLibrary
    {

    public:

        typedef TA_Base_Core::FlexibleTimeoutNamedObject
        <
            TA_Base_Bus::ITrainCorbaDef,
            TA_Base_Bus::ITrainCorbaDef_ptr,
            TA_Base_Bus::ITrainCorbaDef_var
        > TrainAgentNamedObject;

        
        /**
         *  Constructor.
         *
         *  @param  locationKey  the location of the agents that this
         *                       library communicates with.
         *
         *  @exception DataException
         *  @exception DatabaseException
         */
        TrainborneCCTVAccessLibrary(
            unsigned long locationKey);


        /**
         *  Destructor
         */
        ~TrainborneCCTVAccessLibrary();


        /**
         *  activatePECCamera
         *  
         *  @param trainId    the id of the train on which the camera resides
         *  @param carId      the id of the car in the train
         *  @param pecId      the id of the PEC point in the car
         *  @param sessionId  the user's session id
         */
        activatePECCamera(
            unsigned short trainId,
            unsigned short camId,
            const std::string& sessionId);


    protected:

        /**
         *  initialise
         *
         *  Initialises the names of references to the agents that this
         *  library communicates with.
         *
         *  @param  locationKey  the location of the agents that this
         *                       library communicates with.
         *
         *  @exception DataException
         *  @exception DatabaseException
         */
        virtual void TrainborneCCTVAccessLibrary::initialise(
            unsigned long locationKey);


    private:

        // A reference to the train agent CORBA interface
        TrainAgentNamedObject m_trainAgent;
		TA_Base_Core::IConsole* m_console;


    };

}// TA_IRS_Bus


#endif // TRAINBORNE_CCTV_ACCESS_LIBRARY

