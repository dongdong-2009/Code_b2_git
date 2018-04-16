/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/radio/RadioSimulatorLibrary/src/SimulatorHelper.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */


#ifndef SimulatorHelper_H
#define SimulatorHelper_H

#include "bus/radio/RadioSimulatorLibrary/src/SimulatorParser.h"
#include "bus/radio/RadioSimulatorLibrary/src/SimulatorUseCase.h"
#include "bus/radio/RadioSimulatorLibrary/src/ISimulatorSender.h"

#include "core/defaults/src/DefaultEnvironment.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include <string>
#include <vector>

namespace TA_IRS_Bus 
{
    class SimulatorHelper
    {
    public:

        /**
         * Get singleton instance
         */
        static SimulatorHelper& getInstance();

        
        /**
         * Get the list of usecases
         */
        SimulatorUseCaseList& getUseCaseListRef();

        
        /**
         * Load use cases from a file
         */
        void loadTable( const std::string& filename );


        /**
         * Clear use cases
         */
        void clearTable();


        /**
         * display use cases
         */
        void showTable();


        /**
         * show a use case
         */
        void showEntry( unsigned int entry );


        /**
         * Set verbosity level
         */
        void setVerbose( bool verbose );


        /**
         * Find the use cases that match the imcoming data
         */
        SimulatorUseCaseList findUseCases( std::vector<unsigned char>& data );


        /**
         * Get the log parameters from the use case
         */
        std::string displayLoggable( const SimulatorUseCase& uc );


        /**
         * Send the use case response
         */
        void sendMessage( const SimulatorUseCase& uc,
                          ISimulatorSender& sender );

    private:

        /**
         * Private constructor and destructor for singleton
         */
        SimulatorHelper();
        virtual ~SimulatorHelper();


        /**
         * Static members for singleton
         */
        static SimulatorHelper* s_instance;
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;


        /**
         * List of uses cases
         */
        SimulatorUseCaseList m_ucList;

        
        /**
         * Whether logging is verbose
         */
        bool m_verbose;


        /** 
         * Function object adapter conforming to a stl Predicate
         */
        class FindUseCases : public std::unary_function< SimulatorUseCase&, bool > 
        {
        public:
            FindUseCases( const SimulatorUseCase::Mask& mask ) : m_mask(mask) {};
            result_type operator() ( argument_type uc ) { return uc.matchesData( m_mask ) == true; };
        private:
            const SimulatorUseCase::Mask& m_mask;
        };
    };
}

#endif // SimulatorHelper_H