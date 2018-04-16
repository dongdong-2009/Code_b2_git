/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/RadioSimulatorLibrary/src/testsim.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */


#pragma warning(disable:4786)

#include <iostream>
#include "SimulatorParser.h"

using namespace TA_IRS_Bus;

int main(int argc, char ** argv)
{
    std::cout << "Starting..." << std::endl;

    SimulatorParser p("./configSimulator.ini");
    p.parse();

    SimulatorUseCaseList & ucList = p.getUseCases();

    for (SimulatorUseCaseList::iterator i = ucList.begin(); i != ucList.end(); ++i)
    {
        std::cout << "************" << std::endl << (*i);
    }

#if 0
    for (SimulatorUseCaseList::iterator i = ucList.begin(); i != ucList.end(); ++i)
    {
        std::cout << "************" << std::endl
            << "Name [" << (*i).name << "]" << std::endl;

        std::cout << "Mask: [" << std::flush;
        for (SimulatorUseCase::Mask::iterator m = (*i).mask.begin(); m != (*i).mask.end(); ++m)
        {
            std::cout << (int)(*m) << ",";
        }
        std::cout << "]" << std::endl;

        for (unsigned int j = 0; j < (*i).resp.size(); ++j) 
        {
            std::cout << "Resp: [" << j << "][" << std::flush;
            for (SimulatorUseCase::Mask::iterator m = (*i).resp[j].begin(); m != (*i).resp[j].end(); ++m)
            {
                std::cout << (int)(*m) << ",";
            }
            std::cout << "]" << std::endl;
            std::cout << "Wait: [" << j << "][" << (*i).delay[j] << "]" << std::endl;
        }
    }
#endif

	std::vector<unsigned char> a_raw; // mask
	a_raw.push_back(255);
	a_raw.push_back(0);
	a_raw.push_back(7);
	a_raw.push_back(13);
	SimulatorUseCase::Mask aMask(a_raw); // data
	std::vector<unsigned char> b_raw;
	b_raw.push_back(17);
	b_raw.push_back(0);
	b_raw.push_back(7);
//	b_raw.push_back(13);
//	b_raw.push_back(13);
	SimulatorUseCase::Mask bMask(b_raw);
	std::cout << "A mask: " << aMask << std::endl;
	std::cout << "B data: " << bMask << std::endl;
//	std::cout << "B.applyA: " << (bMask.matchesData(aMask)) << std::endl;
//	std::cout << "B.applyA == B: " << ((bMask.matchesData(aMask)) == bMask) << std::endl;

	std::cout << "As hex: " << aMask.renderAsHexString() << std::endl;
	std::cout << "As printable: " << aMask.renderAsPrintableString() << std::endl;

	{ // exact match, no wild
		SimulatorUseCase aUC;
		aUC.mask.push_back(0);
		aUC.mask.push_back(1);
		aUC.mask.push_back(2);
		aUC.mask.push_back(3);
		aUC.mask.push_back(7);
		aUC.mask.push_back(17);
		aUC.mask.push_back(255);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);

		SimulatorUseCase::Mask aMatch;
		aMatch.push_back(0);
		aMatch.push_back(1);
		aMatch.push_back(2);
		aMatch.push_back(3);
		aMatch.push_back(7);
		aMatch.push_back(17);
		aMatch.push_back(255);

		SimulatorUseCase::Mask aNoMatch;
		aNoMatch.push_back(0);
		aNoMatch.push_back(1);
		aNoMatch.push_back(4); // diff
		aNoMatch.push_back(2); // diff
		aNoMatch.push_back(15); // diff
		aNoMatch.push_back(17);
		aNoMatch.push_back(255);

		std::cout << "UC A [Exact match, no wild]" << aUC << std::endl;
		std::cout << "aMatch " << aMatch << std::endl
			<< "\tresult (1) " << aUC.matchesData(aMatch) << std::endl;
		std::cout << "aNoMatch " << aNoMatch << std::endl
			<< "\tresult (0) " << aUC.matchesData(aNoMatch) << std::endl;
		
	}

 	{ // match, with wild
		SimulatorUseCase aUC;
		aUC.mask.push_back(0);
		aUC.mask.push_back(1);
		aUC.mask.push_back(2);
		aUC.mask.push_back(3);
		aUC.mask.push_back(7);
		aUC.mask.push_back(17);
		aUC.mask.push_back(255);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0x00);
		aUC.pattern.push_back(0x00);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);

		SimulatorUseCase::Mask aMatch;
		aMatch.push_back(0);
		aMatch.push_back(1);
		aMatch.push_back(2); // wild
		aMatch.push_back(3); // wild
		aMatch.push_back(7);
		aMatch.push_back(17);
		aMatch.push_back(255);

		SimulatorUseCase::Mask bMatch;
		bMatch.push_back(0);
		bMatch.push_back(1);
		bMatch.push_back(4); // wild
		bMatch.push_back(7); // wild
		bMatch.push_back(7);
		bMatch.push_back(17);
		bMatch.push_back(255);

		SimulatorUseCase::Mask aNoMatch;
		aNoMatch.push_back(0);
		aNoMatch.push_back(1);
		aNoMatch.push_back(4); // wild
		aNoMatch.push_back(2); // wild
		aNoMatch.push_back(15); // diff
		aNoMatch.push_back(17);
		aNoMatch.push_back(255);

		std::cout << "UC A [match, with wild]" << aUC << std::endl;
		std::cout << "aMatch " << aMatch << std::endl
			<< "\tresult (1) " << aUC.matchesData(aMatch) << std::endl;
		std::cout << "bMatch " << bMatch << std::endl
			<< "\tresult (1) " << aUC.matchesData(bMatch) << std::endl;
		std::cout << "aNoMatch " << aNoMatch << std::endl
			<< "\tresult (0) " << aUC.matchesData(aNoMatch) << std::endl;
		
	}

	{ // input size smaller than mask - always fails
		SimulatorUseCase aUC;
		aUC.mask.push_back(0);
		aUC.mask.push_back(1);
		aUC.mask.push_back(2);
		aUC.mask.push_back(3);
		aUC.mask.push_back(3);
		aUC.mask.push_back(3);
		aUC.mask.push_back(3);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0x00);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0x00);
		aUC.pattern.push_back(0x00);
		aUC.pattern.push_back(0x00);

		SimulatorUseCase::Mask aNoMatch;
		aNoMatch.push_back(0);
		aNoMatch.push_back(1);
		aNoMatch.push_back(15);
		aNoMatch.push_back(3);

		std::cout << "UC A [size diff, input smaller]" << aUC << std::endl;
		std::cout << "aNoMatch " << aNoMatch << std::endl
			<< "\tresult (0) " << aUC.matchesData(aNoMatch) << std::endl;
		
	}


	{ // input size bigger than mask
		SimulatorUseCase aUC;
		aUC.mask.push_back(0);
		aUC.mask.push_back(1);
		aUC.mask.push_back(2);
		aUC.mask.push_back(3);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0xff);
		aUC.pattern.push_back(0x00);
		aUC.pattern.push_back(0xff);

		SimulatorUseCase::Mask aMatch;
		aMatch.push_back(0);
		aMatch.push_back(1);
		aMatch.push_back(15);
		aMatch.push_back(3);
		aMatch.push_back(7);
		aMatch.push_back(17);
		aMatch.push_back(255);

		std::cout << "UC A [size diff, input bigger]" << aUC << std::endl;
		std::cout << "aMatch " << aMatch << std::endl
			<< "\tresult (1) " << aUC.matchesData(aMatch) << std::endl;
		
	}

	return 0;
}