
#ifndef SIMULATORPARSER_H
#define SIMULATORPARSER_H

#include <string>
#include <vector>
#include <map>

#include "SimulatorUseCase.h"

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
static const int SimulatorParser::CM_PDUTYPE_INDEX;
static const int SimulatorParser::CM_SOURCE_SSI_INDEX;
static const int SimulatorParser::CM_DESTINATION_SSI_INDEX;
static const int SimulatorParser::CM_PI_INDEX;
static const int SimulatorParser::CM_FLAG_INDEX;
static const int SimulatorParser::CM_MESSAGE_REF_INDEX;	// for CM-BROADCAST and CM-DATA only
static const int SimulatorParser::CM_DATA_AREA_INDEX;
static const int SimulatorParser::CM_DATA_TL_LENGTH_INDEX;

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