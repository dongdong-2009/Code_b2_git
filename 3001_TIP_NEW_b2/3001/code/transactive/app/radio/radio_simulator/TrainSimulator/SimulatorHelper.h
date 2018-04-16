
#ifndef SIMULATORHelper_H
#define SIMULATORHelper_H

#include <string>
#include <vector>
#include <map>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "app/radio/radio_simulator/TrainSimulator/SimulatorParser.h"
#include "app/radio/radio_simulator/TrainSimulator/SimulatorUseCase.h"
//#include "ISimulatorSender.h"

namespace TA_IRS_Bus 
{
    
    class SimulatorHelper
    {
    public:
//        SimulatorHelper(SimulatorUseCaseList &ucList);
//        virtual ~SimulatorHelper();
		static SimulatorHelper& getInstance();

		SimulatorUseCaseList& getUseCaseListRef();
		void loadTable(const std::string& filename);
		void clearTable();
		void showTable();
		void showEntry(unsigned int entry);
		void setVerbose(bool verbose);
 

        /** 
         * Function object adapter conforming to a stl Predicate
         */
        class FindUseCases : public std::unary_function<SimulatorUseCase&, bool > 
        {
        public:
            FindUseCases(const SimulatorUseCase::Mask& mask) : m_mask(mask) {};
            result_type operator() (argument_type uc) { return uc.matchesData(m_mask) == true; };
        private:
            const SimulatorUseCase::Mask& m_mask;
        };

		SimulatorUseCaseList findUseCases(std::vector<unsigned char> &data);

        /**
         * Find first use case from the collection which matches the specified data.
         */
        SimulatorUseCaseList::iterator findUseCase(std::vector<unsigned char> &data);        
        
		void displayLoggable(const SimulatorUseCase& uc);

        //void sendMessage(/*const SimulatorUseCase::Mask& pData, */
        //    const SimulatorUseCase& uc,
        //    /*void(ISimulatorSender::*func)(const char * event, unsigned long eventLength),*/
        //    ISimulatorSender& sender);

    private:
//        SimulatorUseCaseList& m_ucList;

		SimulatorUseCaseList m_ucList;
		bool m_verbose;

		static SimulatorHelper* s_instance;
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

		SimulatorHelper();
        virtual ~SimulatorHelper();
    };


}

#endif // SIMULATORHelper_H