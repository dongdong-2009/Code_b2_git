/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/RadioSimulatorLibrary/src/SimulatorParser.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */


#ifndef SIMULATORPARSER_H
#define SIMULATORPARSER_H

#include "bus/radio/RadioSimulatorLibrary/src/SimulatorUseCase.h"

#include "core/defaults/src/DefaultEnvironment.h"

#include <string>
#include <vector>

namespace TA_IRS_Bus
{
    struct SimulatorParserException {};

    class SimulatorParser
    {
    public:
        
        typedef std::pair<std::string, std::string > NVpair;
        SimulatorParser(const std::string & Name = "configSimulator.ini");
        virtual ~SimulatorParser();
        void parse();
        SimulatorUseCaseList& getUseCases();

    private:
        NVpair parseLine(const std::string &text, bool removeValueWS = true);
        std::string getValue(const std::string &text);        
        std::string removeWhitespace(std::string text); 
		std::string convertQuoted(std::string text);
		std::vector<unsigned char> generateMask(const std::string& theText) const;
		std::vector<unsigned char> generatePattern(const std::string& theText) const;
        std::vector<unsigned char> chr2hex(const std::string& text) const;


        SimulatorUseCaseList m_useCases;
        
        std::vector<std::string > m_allLines;


    };

}

#endif // SIMULATORPARSER_H