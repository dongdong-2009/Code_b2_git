
#pragma warning(disable:4786)

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ios>

#include "SimulatorHelper.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ThreadGuard.h"
//#include "app/trains/train_agent/TrainProtocolLibrary/src/FrameCheckSequence.h"
#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"

#include "core/utilities/src/DebugUtil.h"

#define SDS 4669
#define SDSTL 4712
#define DEFAULT_INCOMING_SDS SDSTL

using namespace TA_IRS_Bus;
using namespace TA_Base_Core;
using TA_Base_Core::DebugUtil;

SimulatorHelper* SimulatorHelper::s_instance = 0;
TA_Base_Core::NonReEntrantThreadLockable SimulatorHelper::m_singletonLock;

SimulatorHelper& SimulatorHelper::getInstance()
{
	ThreadGuard guard( m_singletonLock );
	
	if (s_instance == NULL)
	{
		s_instance = new SimulatorHelper();
	}
	return *s_instance;
}

#if 0
SimulatorHelper::SimulatorHelper(SimulatorUseCaseList &ucList)
: m_ucList(ucList)
{
}
#endif

SimulatorHelper::SimulatorHelper()
:m_verbose(true)
{
}

SimulatorHelper::~SimulatorHelper()
{
}

SimulatorUseCaseList& SimulatorHelper::getUseCaseListRef()
{
	return m_ucList;
}

void SimulatorHelper::loadTable(const std::string& filename)
{
	SimulatorParser p(filename);
	p.parse();
	
	SimulatorUseCaseList& x = p.getUseCases();
	std::copy(x.begin(), x.end(), std::back_inserter(m_ucList));
}

void SimulatorHelper::clearTable()
{
	m_ucList.clear();
}

void SimulatorHelper::showTable()
{
	std::cout << "UC ID\tUC name" << std::endl;
	SimulatorUseCaseList& ucList = SimulatorHelper::getUseCaseListRef();
	for (unsigned int uc = 0; uc < ucList.size(); ++uc)
	{
		std::cout << uc << "\t" << ucList[uc].name << std::endl;
	}
}

void SimulatorHelper::setVerbose(bool verbose)
{
	m_verbose = verbose;
}

void SimulatorHelper::showEntry(unsigned int entry)
{
	if (entry >= m_ucList.size())
	{
		std::cout << "Error: no entry [" << entry << "]" << "with usecase size [" << m_ucList.size() << "]" << std::endl;
		return;
	}
	std::cout << m_ucList[entry] << std::endl;
}


SimulatorUseCaseList SimulatorHelper::findUseCases(std::vector<unsigned char> &data)
{
	/// @todo refactor this method, it has gotten quite awkward...
	SimulatorUseCaseList ucMatch;
	SimulatorUseCaseList ucMulti;
	unsigned int countNonContinues = 0;
	bool first = true;

	/// @todo std::find_if() only returns *first* in range, not an iterator pointing to the range - develop a custom iterator to return a subset
#if 0 
	std::copy(
		std::find_if(SimulatorHelper::getInstance().getUseCaseListRef().begin(),
			SimulatorHelper::getInstance().getUseCaseListRef().end(),
			SimulatorHelper::FindUseCases(SimulatorUseCase::Mask(data)) ),
		SimulatorHelper::getInstance().getUseCaseListRef().end(),
		std::back_inserter(ucMatch) );
#endif
	for (SimulatorUseCaseList::iterator i = std::find_if(SimulatorHelper::getInstance().getUseCaseListRef().begin(), 
			SimulatorHelper::getInstance().getUseCaseListRef().end(), 
			SimulatorHelper::FindUseCases(SimulatorUseCase::Mask(data)) ); 
		i != SimulatorHelper::getInstance().getUseCaseListRef().end(); 
		i = std::find_if(++i, SimulatorHelper::getInstance().getUseCaseListRef().end(), 
				SimulatorHelper::FindUseCases(SimulatorUseCase::Mask(data)) )
		)
	{
		SimulatorUseCaseList::iterator match = i;
			
		if (match != SimulatorHelper::getInstance().getUseCaseListRef().end())
		{
			if ((*match).continueMode == false && (*match).continueOnlyIfFirst == false)
			{
				countNonContinues++;
			}
			else if ((*match).continueOnlyIfFirst == true && first == false)
			{
				// skip
				//std::cout << "Skipping [" << (*match).name << "]" << std::endl;
				continue;
			}
			if (countNonContinues <= 1) // prune out all but first non-continue 
			{
				ucMatch.push_back(*match);
				if (countNonContinues == 1)
				{
					ucMulti.push_back(*match);
				}
			}
			else 
			{
				ucMulti.push_back(*match);
			}
		}
		if (true == first)
		{
			first = false; // no longer first iteration
		}
	}
	//std::cout << "Matches size is " << ucMatch.size() << std::endl;
	//std::cout << "Multi size is " << ucMulti.size() << std::endl;
	if (countNonContinues > 1)
	{
		std::cout << "Warning: Multiple non-continuation matches found, check configuration..." << std::endl;
		for (SimulatorUseCaseList::iterator i = ucMulti.begin(); i != ucMulti.end(); ++i)
		{
			std::cout << "\t'" << (*i).name << "'" << std::endl;
		}
	}
	return ucMatch;
}

// ? unused
SimulatorUseCaseList::iterator SimulatorHelper::findUseCase(std::vector<unsigned char> &data)
{
	SimulatorUseCaseList::iterator useCaseIter = m_ucList.end();
	SimulatorUseCaseList matchesFound;

    SimulatorUseCase::Mask mData(data);
    for (SimulatorUseCaseList::iterator i = m_ucList.begin(); i != m_ucList.end(); ++i)
    {
//        if ( ((*i).mask & data) == data)
//		if ( mData.applyMask((*i).mask) == mData)
		if ( (*i).matchesData(mData) == true)
		{ 
			if (useCaseIter == m_ucList.end())
			{
				useCaseIter = i;
			}
			matchesFound.push_back(*i);
			//std::cout << ">>mask<< " << (*i).mask << std::endl
			//	<< ">>data<< " << mData << std::endl
			//	<< ">>data.applyMask<< " << mData.applyMask((*i).mask) << std::endl
			//	<< ">> match << " << ( mData.applyMask((*i).mask) == mData) << std::endl;
        }
    }
//	useCaseIter = matchesFound.begin();
	if (matchesFound.size() > 1) 
	{
		std::cout << "Warning: Multiple matches found, check configuration..." << std::endl;
		for (SimulatorUseCaseList::iterator i = matchesFound.begin(); i != matchesFound.end(); ++i)
		{
			std::cout << "\t'" << (*i).name << "'" << std::endl;
		}
	}
    return useCaseIter;
}

void SimulatorHelper::displayLoggable(const SimulatorUseCase& uc)
{
	if (uc.ilog.size() == 0) {
		return;
	}

	std::cout << "Displaying selected field values..." << std::endl;

	for (std::map<std::string, SimulatorUseCase::Var>::const_iterator i = uc.ilog.begin(); i != uc.ilog.end(); ++i)
	{
		SimulatorUseCase::Mask field;
		if (uc.verboseMode >= 9) std::cout << "Composing Ilog[" << (*i).first << "]" << std::endl;
		// basic checks first...
		if (uc.data.size() < (*i).second.inpos+(*i).second.length)
		{
			std::cout << "Warning: Config for Ilog [" << (*i).first << "] incorrect, not displaying" << std::endl;
			continue;
		}
		std::copy( &uc.data[(*i).second.inpos],
			&uc.data[(*i).second.inpos+(*i).second.length], 
			std::back_inserter(field));

		std::cout << "\t[" << (*i).first << "] = [";
		if ((*i).second.render == 'h')
		{
			std::cout << field.renderAsHexString();
#if 0
			// copied from SimulatorUseCase::renderAsHexString()
			std::stringstream s;
			s << "(" << std::setw(3) << field.size() << ":" << "0x... ";
			if (field.size() > 0) 
			{
				for (unsigned int i = 0; i < field.size(); ++i)
				{
					s.setf(std::ios_base::hex, std::ios_base::basefield);
					s << std::setw(2) << std::setfill('0') << (int)field[i] 
						<< ( ((i+1) % 4) == 0 ? " ": "");
				}
				
			} 
			s << " )" << std::ends;
			std::cout << s;
#endif
		} 
		else if ((*i).second.render == 'a')
		{
			std::cout << field.renderAsPrintableString();
		}
		else if ((*i).second.render == 'n')
		{
			// assume little endian
			if (field.size() == 1) 
			{
				std::cout << static_cast<unsigned int>(field[0]);
			}
			else if (field.size() == 2) 
			{
				std::cout << static_cast<unsigned int>(field[0] + (field[1] << 8));
			}
			else if (field.size() == 4) 
			{
				std::cout << static_cast<unsigned int>(field[0] + (field[1] << 8) + (field[2] << 16)+ (field[3] << 24));
			}
			else 
			{ 
				std::cout << "<incorrectly configured length, cannot display value>";
			}
		}
		else 
		{
			std::cout << "<unknown type> " << field.renderAsHexString();
		}
		std::cout << "]" << std::endl;
	}
}

/*
void SimulatorHelper::sendMessage(const SimulatorUseCase& uc, ISimulatorSender& sender)
{
	bool clearToSend = true;

    if (uc.resp.size() == 0)
    {
//        std::cout << "Error: Cannot generate response for [" << uc.name << "]" << std::endl;
		std::cout << "OK: Drained message [" << uc.name << "], no reply sent" << std::endl;
        return;
    }

// 	std::stringstream myMessage;

    /// @todo Handle generated output vars...

    for (unsigned int respNo = 0 ; respNo < uc.resp.size(); ++respNo)
    {
        std::cout << "Waiting " << uc.delay[respNo] << "ms...\r";
        TA_Base_Core::Thread::sleep(uc.delay[respNo]); // wait before sending

//		const unsigned int bufferMax = uc.resp[respNo].size();
		const unsigned int bufferMax = uc.respSize[respNo];
        char * buffer = new char[bufferMax];
        
        // Just copy the response as an initial starting point, then tweak it later...
        std::fill(buffer,buffer+bufferMax,0);
        std::copy(uc.resp[respNo].begin(), uc.resp[respNo].end(), buffer);

        // Let's mangle the buffer with the Ivar's
        for (std::map<SimulatorUseCase::Token, SimulatorUseCase::Var>::const_iterator i = uc.ivar[respNo].begin(); i != uc.ivar[respNo].end(); ++i) 
        {
            if (uc.verboseMode >= 9) std::cout << "Mapping Ivar[" << (*i).first << "]" << std::endl;
            // basic checks first...
            if (uc.data.size() < (*i).second.inpos+(*i).second.length ||
                bufferMax < (*i).second.outpos+(*i).second.length)
            {
                std::cout << "Error: either received data or output buffer too small" << std::endl;
				clearToSend = false;
                continue;
            }
            std::copy( &uc.data[(*i).second.inpos],
                &uc.data[(*i).second.inpos+(*i).second.length], 
                buffer+((*i).second.outpos));
        }

        // Then the Ovar's
        for (std::map<SimulatorUseCase::Token, SimulatorUseCase::Var>::const_iterator j = uc.ovar[respNo].begin(); j != uc.ovar[respNo].end(); ++j) 
        {
            if ((*j).first == '#') // do this last, as it depends on all data being present
            {
                continue;
            }

            if (uc.verboseMode >= 9)  std::cout << "Composing Ovar[" << (*j).first << "]" << std::endl;

            // basic checks first...
            if (bufferMax < (*j).second.outpos+(*j).second.length)
            {
                std::cout << "Error: output buffer too small" << std::endl;
				clearToSend = false;
                continue;
            }

            std::cout << "Error: Don't know how to generate token [" << (*j).first << "], so ignoring it." << std::endl; 

        }

        // we previously dodged the '#' token to complete it last, so deal with it if it is present
        std::map<SimulatorUseCase::Token, SimulatorUseCase::Var>::const_iterator
                sumIter = uc.ovar[respNo].find('#');

        if (sumIter != uc.ovar[respNo].end())
        {
            SimulatorUseCase::Var var = (*sumIter).second;
            const int datastart = var.inpos;
            const int datalen   = var.length;
            const int crcPos    = var.outpos;

            // vector given to FramCheckSequence::calculate
            std::vector<unsigned char> cc;

            // fill with bytes according to the ovar specs
            std::copy(  &buffer[datastart],            // start at data position
                        &buffer[datastart+datalen],    // end at last databyte
                        std::back_inserter(cc));

            unsigned short calcCRC = TA_IRS_Bus::FrameCheckSequence::calculate(cc, cc.size());

            if (uc.verboseMode >= 9) std::cout << "Train Checksum: " << (0xff & calcCRC) << "," << ((0xff00 & calcCRC)>>8) << std::endl;

            // stuff the crc into specified position, little-endian
            buffer[crcPos]   = (unsigned char)((0xff   & calcCRC));
            buffer[crcPos+1] = (unsigned char)((0xff00 & calcCRC)>>8);
        }


        if (true == clearToSend)
		{
			std::cout << "Sending [" << respNo << "][" << uc.name << "]" << std::endl;
			if (uc.verboseMode >= 1) 
			{
				std::vector<unsigned char> x;
				std::copy(buffer,buffer+bufferMax,std::back_inserter(x));
				std::cout << "Detailed Response: [" << SimulatorUseCase::Mask(x);
				if (uc.verboseMode >= 2)
				{
					std::cout << uc.data.renderAsPrintableString();
				}
				std::cout << "]" << std::endl;
			}


			// Send out to session observers
			sender.simulatorSendEvent(buffer,bufferMax);
		}
		else 
		{
			std::cout << "Error: Could not send message due to errors composing it." << std::endl;
		}

		delete[] buffer;
		buffer = NULL;
    }


// 	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"%s",myMessage.str().c_str());

	std::cout << std::endl;
}
*/