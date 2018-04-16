
#pragma warning(disable:4786)

#include <iostream>
#include <iomanip>
#include <ios>
#include <sstream>

#include "SimulatorUseCase.h"

using namespace TA_IRS_Bus;


SimulatorUseCase::SimulatorUseCase()
{
}

SimulatorUseCase::Mask::Mask()
{
}

// New improved and working properly for unusual matches like 1,3,7,15,31,63,127,255 ;)
// We calculate a match by: (lhs xor rhs) and mask
bool SimulatorUseCase::matchesData(const SimulatorUseCase::Mask & rhs) const
{
	/// @note lhs is the use-case and rhs is the input data, always!
	Mask lhs( (*this).mask );

    if ((*this).verboseMode >= 9) std::cout << "lhs:" << lhs.size() << ": " << lhs << std::endl << "rhs:" << rhs.size() << ": " << rhs << std::endl;

    if (lhs.size() == 0)
	{
		// mask.data empty always fails 
		return false;
	}
	else if (rhs.size() < lhs.size())
    {
        //std::cout << "Error: size mismatch" << std::endl;
        return false; // empty
    }

    for (unsigned int i = 0; i < lhs.size();  ++i)
    {
        lhs[i] ^= rhs[i];
		lhs[i] &= (*this).pattern[i];
    }
    for (unsigned int j = rhs.size(); j < lhs.size(); ++j)
    {
        // leave as is
    }
	// We could have returned from for loop above, but we would miss chance to 
	// show the full result for debugging purposes
	if ((*this).verboseMode >= 9) std::cout << "SimulatorUseCase::applyMask() => " << lhs << std::endl;
    for (unsigned int k = 0; k < lhs.size();  ++k)
    {
		if (lhs[k] != 0)
		{
			return false;
		}
	}
	return true;
}


bool SimulatorUseCase::matchesData(const std::vector<unsigned char> & rhs) const
{
    return (*this).matchesData(Mask(rhs));
}


#if 0 // old implementation
SimulatorUseCase::Mask SimulatorUseCase::Mask::applyMask (const SimulatorUseCase::Mask & rhs)
{
	Mask lhs(*this);

//    std::cout << "lhs:" << lhs.size() << ": " << lhs << std::endl
//        << "rhs:" << rhs.size() << ": " << rhs << std::endl;

    if (rhs.size() == 0 || lhs.size() < rhs.size())
    {
//        std::cout << "Error: size mismatch" << std::endl;
        return Mask(); // empty
    }



    // do this as a foreach ??
    for (unsigned int i = 0; i < rhs.size();  ++i)
    {
        lhs[i] &= rhs[i];
    }
    for (unsigned int j = rhs.size(); j < lhs.size(); ++j)
    {
        // leave as is
    }
//    std::cout << "==>:" << lhs.size() << ": " << lhs << std::endl;

    return lhs;
}

SimulatorUseCase::Mask SimulatorUseCase::Mask::applyMask (const std::vector<unsigned char> & rhs)
{
    return (*this).applyMask(Mask(rhs));
}
#endif

std::string SimulatorUseCase::Mask::renderAsHexString() const
{
	std::stringstream s;

	s << "(" << std::setw(3) << size() << ":" << "0x... ";
	if (size() > 0) 
	{
		for (unsigned int i = 0; i < size(); ++i)
		{
			s.setf(std::ios_base::hex, std::ios_base::basefield);
			s << std::setw(2) << std::setfill('0') << (int)(*this)[i] 
				<< ( ((i+1) % 4) == 0 ? " ": "");
		}
		
	} 
	s << " )" << std::ends;
	return s.str();	
}

std::string SimulatorUseCase::Mask::renderAsPrintableString() const
{
	std::stringstream s;

	s << "(" << std::setw(3) << size() << ":" << " ";
	if (size() > 0) 
	{
		for (unsigned int i = 0; i < size(); ++i)
		{
			if ((*this)[i] < 32 || (*this)[i] >= 127 )
			{
				s << ".";
			}
			else 
			{
				s << (char)(*this)[i];
			}
			s << ( ((i+1) % 4) == 0 ? " ": "");
		}
		
	} 
	s << " )" << std::ends;
	return s.str();	
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_Bus::SimulatorUseCase::Mask & mask)
{
	os << mask.renderAsHexString();// << mask.renderAsPrintableString();
#if 0
    os << "[";
    for (SimulatorUseCase::Mask::const_iterator m = mask.begin(); m != mask.end(); ++m)
    {
        os << (int)(*m) << ",";
    }
    os << "]";
#endif
    return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_Bus::SimulatorUseCase::Var & var)
{
    os << "[in:" << var.inpos << ",out:" << var.outpos << ",len:" << var.length << "]";
    return os;
}

#if 0
std::ostream & operator<< (std::ostream & os, const TA_IRS_Bus::SimulatorUseCase::VarCollection & var)
{
    using namespace TA_IRS_Bus;
    for (unsigned int i = 0; i != var.size(); ++i) 
    {
        os << "\tOvar: [" << i << "]" << std::endl;
        // now search through map
        for (std::map<SimulatorUseCase::Token, SimulatorUseCase::Var>::const_iterator j = var[i].begin(); j != var[i].end(); ++j) 
        {
            os << "\t\tToken: " << (*j).first << " OutputPosition " << (*j).second.position 
                << " Length " << (*j).second.length << std::endl;
        }
    }
    return os;
}
#endif

std::ostream & operator<< (std::ostream & os, const std::map<TA_IRS_Bus::SimulatorUseCase::Token, TA_IRS_Bus::SimulatorUseCase::Var> & var)
{
    using namespace TA_IRS_Bus;
    // now search through map
    for (std::map<SimulatorUseCase::Token, SimulatorUseCase::Var>::const_iterator j = var.begin(); j != var.end(); ++j) 
    {
        os << "\t\tToken: " << (*j).first;
        if ((*j).second.type == TA_IRS_Bus::SimulatorUseCase::Var::InOut) 
        {
            os << " InputPosition " << (*j).second.inpos
                << " OutputPosition " << (*j).second.outpos
                << " Length " << (*j).second.length << std::endl;
        }
        else if ((*j).second.type == TA_IRS_Bus::SimulatorUseCase::Var::Out) 
        {
            os << " OutputPosition " << (*j).second.outpos
                << " Length " << (*j).second.length << std::endl;
        }
        else 
        {
            os << "<Unknown type>" << std::endl;
        }
    }
    return os;
}

// Ilog helper
std::ostream & operator<< (std::ostream & os, const std::map<std::string, TA_IRS_Bus::SimulatorUseCase::Var> & var)
{
    using namespace TA_IRS_Bus;
    // now search through map
    for (std::map<std::string, SimulatorUseCase::Var>::const_iterator j = var.begin(); j != var.end(); ++j) 
    {
        os << "\t\tLabel: " << (*j).first;
		os << " InputPosition " << (*j).second.inpos
			<< " Length " << (*j).second.length 
			<< " RenderAs " << static_cast<char>((*j).second.render)
			<< std::endl;
    }
    return os;
}

std::ostream & operator<< (std::ostream & os, const TA_IRS_Bus::SimulatorUseCase & uc)
{
    os << "Simulator UseCase [" << uc.name << "]" << std::endl;
 
	os << "\tVerbose: " << uc.verboseMode << " Continue: " << uc.continueMode 
		<< " Skip: " << uc.continueOnlyIfFirst << std::endl;
    os << "\tMask: " << uc.mask << uc.mask.renderAsPrintableString() << std::endl;
    os << "\tPatt: " << uc.pattern << uc.mask.renderAsPrintableString() << std::endl;
	os << "\tIlog: " << uc.ilog << std::endl;
    for (unsigned int j = 0; j < uc.resp.size(); ++j) 
    {
        os << "\tResp: [" << j << "]" << uc.resp[j] << std::endl;
		os << "\tRespSize: [" << j << "]" << uc.respSize[j] << std::endl;
        os << "\tIvar: [" << j << "]" << uc.ivar[j] << std::endl;
        os << "\tOvar: [" << j << "]..." << uc.ovar[j] << std::endl;
        os << "\tWait: [" << j << "]..." << uc.delay[j] << std::endl;
    }

//    os << "\tVars: []" << std::endl;
    return os;
}

