
#ifndef SIMULATORUSECASE_H
#define SIMULATORUSECASE_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include "CMData.h"

namespace TA_IRS_Bus 
{
    
    class SimulatorUseCase
    {
    public:
        SimulatorUseCase();

//        typedef std::vector<unsigned char> Mask;
        class Mask : public std::vector<unsigned char>
        {
        public:
            Mask();
            explicit Mask(const std::vector<unsigned char>& copy)
            {
                //*this = copy;
                clear();
                std::copy(copy.begin(), copy.end(), std::back_inserter(*this));
            }
#if 0
            Mask applyMask (const Mask & rhs);
            Mask applyMask (const std::vector<unsigned char> & rhs);
#endif
			std::string renderAsHexString() const;
			std::string renderAsPrintableString() const;
        };

        bool matchesData (const Mask & rhs) const;
        bool matchesData (const std::vector<unsigned char> & rhs) const;

		typedef std::vector<Mask> MaskCollection;
        typedef char Token;

        struct Var 
        {
            enum VarType { InOut, Out };
			enum RenderType { HEX = 'h', ASCII = 'a', HEX_ASCII = 'x', NUMERIC = 'n' }; // ? instead of numeric, why not little/big endian or even byte order eg. B4321, B1234, B3421

            VarType type;
            int inpos;
            int outpos;
            unsigned int length;
			RenderType render;
            // function pointer to render'er
        };
        typedef std::vector<std::map<Token, Var> > VarCollection;

        std::string name;
        Mask data; /// raw input received - used for variable substitution
		Mask pattern; /// pattern to match (with wild cards set to zero - although this is not significant)
        Mask mask; /// mask representing wild cards ( f significant, 0 wild )
        VarCollection ivar;
		std::map<std::string, Var> ilog; /// dumping of selected recv'd data

        // multiple resp
        MaskCollection resp;
		std::vector<unsigned int> respSize; /// to record size of buffer to send, if resp not big enough, pad with zeros to the specified size.
        VarCollection ovar;
        std::vector<unsigned int> delay;

		unsigned int verboseMode; /// flag to indicate verbosity of received messages
		bool continueMode; /// flag to indicate after matching this case, allow subsequent cases a chance to match
		bool continueOnlyIfFirst; /// flag to allow a matching case to be skipped if it is not the first in a continuation chain

		CMData cmData;
    };

    typedef std::vector<SimulatorUseCase> SimulatorUseCaseList;

    //SimulatorUseCase::Mask operator& (SimulatorUseCase::Mask & rhs);
}

// helpers
std::ostream & operator<< (std::ostream & os, const TA_IRS_Bus::SimulatorUseCase::Mask & mask);
std::ostream & operator<< (std::ostream & os, const TA_IRS_Bus::SimulatorUseCase::Var & var);
std::ostream & operator<< (std::ostream & os, const std::map<TA_IRS_Bus::SimulatorUseCase::Token, TA_IRS_Bus::SimulatorUseCase::Var> & var);
//std::ostream & operator<< (std::ostream & os, const TA_Bus::SimulatorUseCase::VarCollection & var);
std::ostream & operator<< (std::ostream & os, const TA_IRS_Bus::SimulatorUseCase & uc);


#endif // SIMULATORUSECASE_H