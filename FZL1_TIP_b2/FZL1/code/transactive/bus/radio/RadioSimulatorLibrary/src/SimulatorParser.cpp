/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/RadioSimulatorLibrary/src/SimulatorParser.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  *
  */


#pragma warning(disable:4786)

#include "bus/radio/RadioSimulatorLibrary/src/SimulatorParser.h"
#include "bus/radio/RadioSimulatorLibrary/src/SimulatorUseCase.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/ThreasdafeScreenOutput.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <locale>

using TA_Base_Core::RunParams;

namespace
{
    struct Uppercase : public std::unary_function< char,  char>
    {
        result_type operator()( argument_type c ) const { return static_cast<result_type>( std::toupper( static_cast<argument_type>(c), std::locale() ) ); }
    };

    //
    // Variables, functions and Macros to assist with parse verbosity
    //
    typedef enum eParseVerbosity
    {
        pv_low      = 1,
        pv_medium   = 3,
        pv_high     = 5,
        pv_debug    = 7,
        pv_default  = pv_low
    };

    static int parserVerbosity = pv_default;
    
    static bool parserVerbosityInitialised = false;
    
    static void InitialiseParserVerbosity()
    {
        if ( !parserVerbosityInitialised )
        {
            std::string verbose_str = RunParams::getInstance().get("ParserVerbosity");
            if ( ! verbose_str.empty() )
            {
                std::string verbose_str = RunParams::getInstance().get("ParserVerbosity");
                if (verbose_str == "OFF")
                {
                    parserVerbosity = 0;
                }
                else if (verbose_str == "ON")
                {
                    parserVerbosity = 9;
                }
                else
                {
                    parserVerbosity = atoi(verbose_str.c_str());
                }
            }

            parserVerbosityInitialised = true;
        }
    }

#define START_VERBOSITY(x) if (parserVerbosity>=x) {
#define END_VERBOSITY }

}

namespace TA_IRS_Bus
{
    SimulatorParser::SimulatorParser(const std::string & Name)
    {
        InitialiseParserVerbosity();

        const int lineSize = 1024;
        std::string fileName = Name;
    
        // Open file.
        std::ifstream file( fileName.c_str() );
    
        // Throw exception if it does not exist.
        if ( ! file )
        {
            throw SimulatorParserException();
        }
    
        char inBuf[lineSize];
    
        // Read in all lines and add them to a vector
        // Suppress comments and blank lines while we are here
        while ( file.getline ( inBuf, lineSize ) )
        {
            char * crPtr = strchr( inBuf, '\r' );
            if( 0 != crPtr )
            {
                *crPtr = '\0';
            }
            std::string line = std::string( inBuf );
            if (line == "" || 
                line.compare(0,2,"//") == 0 ||
                line.compare(0,1," ") == 0 ||
                line.compare(0,1,"\t") == 0 
                )
            {
                // suppress
    //            START_VERBOSITY(pv_)
    //            COUT( "Filter: [" << line << "]" << std::endl );
    //            END_VERBOSITY
            }
            else 
            {
                m_allLines.push_back( line );
            }
        }
    
        // close file
        file.close();   
    }


    SimulatorParser::~SimulatorParser()
    {
    }


    void SimulatorParser::parse()
    {
	    unsigned int verbose = 0;
        
        for (std::vector<std::string>::iterator i = m_allLines.begin(); i != m_allLines.end(); ++i)
        {
            START_VERBOSITY(pv_high)
            COUT( "Line: [" << *i << "]" << std::endl );
            END_VERBOSITY
            // Expect strict ordering...
            NVpair p = parseLine(*i, false);

		    if (p.first == "VERBOSE")
		    {
                START_VERBOSITY(pv_medium)
                COUT( "setting verbose level to... [" << p.second << "]" << std::endl );
                END_VERBOSITY

			    std::string verboseMode = removeWhitespace(p.second);
			    std::transform(verboseMode.begin(), verboseMode.end(), verboseMode.begin(), Uppercase());

			    if (verboseMode == "ON" || verboseMode == "YES" || verboseMode == "1") 
			    {
				    verbose = 1;
			    }
			    else if (verboseMode == "OFF" || verboseMode == "0")
			    {
				    verbose = 0;
			    }
			    else if (verboseMode == "2")
			    {
				    verbose = 2;
			    }
			    else if (verboseMode == "9")
			    {
				    verbose = 9;
			    }
			    else 
			    {
				    throw SimulatorParserException();
			    }
		    }
            else if (p.first == "NAME")
            {
                START_VERBOSITY(pv_medium)
                COUT( "Processing new entry... [" << p.second << "]" << std::endl );
                END_VERBOSITY

                SimulatorUseCase newUseCase;
                newUseCase.name = p.second;
			    newUseCase.verboseMode = verbose; // use current verbose setting
			    newUseCase.continueMode = false;
			    newUseCase.continueOnlyIfFirst = false; // only relevant if continueMode set

                unsigned int resp_idx = 0; // for multiple responses...
            
                bool createResp = true;
                bool finished = false;
                while (!finished) 
                {
                    ++i; if (i == m_allLines.end()) { --i; finished = true; continue; }
                    p = parseLine(*i);
                    if (p.first == "DATA") 
                    {
                        // append, so we can handle mutliple lines...
                        std::vector<unsigned char> hexMask = generateMask(p.second);
                        std::copy(hexMask.begin(), hexMask.end(), std::back_inserter(newUseCase.mask));
                        std::vector<unsigned char> hexPattern = generatePattern(p.second);
                        std::copy(hexPattern.begin(), hexPattern.end(), std::back_inserter(newUseCase.pattern));
                    } 
                    else if (p.first == "IVAR")
                    {
                        // get the mapped variables
                        // format: <token>:<inputpos>:<length>:<outputpos>
                        /// @todo do this better - use a tokenizer or something...
                        std::string::size_type offset_begin = 0;
                        std::string::size_type offset_end = p.second.find(':', offset_begin);
                        std::string token = p.second.substr(offset_begin, offset_end);
                        offset_begin = offset_end+1;
                        offset_end = p.second.find(':', offset_begin);
                        //START_VERBOSITY(pv_)
                        //COUT( "[[" << p.second.substr(offset_begin, offset_end).c_str() << "]]" << std::endl );
                        //END_VERBOSITY
                        unsigned int inpos = strtol(p.second.substr(offset_begin, offset_end).c_str(),0,0);
                        offset_begin = offset_end+1;
                        offset_end = p.second.find(':', offset_begin);
                        unsigned int length = strtol(p.second.substr(offset_begin, offset_end).c_str(),0,0);
                        offset_begin = offset_end+1;
                        offset_end = p.second.find(':', offset_begin);
                        unsigned int outpos = strtol(p.second.substr(offset_begin, offset_end).c_str(),0,0);

                        SimulatorUseCase::Var v;
                        v.type = SimulatorUseCase::Var::InOut;
                        v.inpos = inpos;
                        v.outpos = outpos;
                        v.length = length;

                        START_VERBOSITY(pv_medium)
                        COUT( "Ivar: [" << token << "] " << v << std::endl );
                        END_VERBOSITY
                        newUseCase.ivar[token[0]] = v;
                    }
                    else if (p.first == "ILOG")
                    {
                        // get the mapped variables
                        // format: <string>:<inputpos>:<length>:<h/a/n>
                        /// @todo do this better - use a tokenizer or something...
                        std::string::size_type offset_begin = 0;
                        std::string::size_type offset_end = p.second.find(':', offset_begin);
                        std::string token = p.second.substr(offset_begin, offset_end);
                        offset_begin = offset_end+1;
                        offset_end = p.second.find(':', offset_begin);
                        //START_VERBOSITY(pv_high)
                        //COUT( "[[" << p.second.substr(offset_begin, offset_end).c_str() << "]]" << std::endl );
                        //END_VERBOSITY
                        unsigned int inpos = strtol(p.second.substr(offset_begin, offset_end).c_str(),0,0);
                        offset_begin = offset_end+1;
                        offset_end = p.second.find(':', offset_begin);
                        unsigned int length = strtol(p.second.substr(offset_begin, offset_end).c_str(),0,0);
                        offset_begin = offset_end+1;
                        offset_end = p.second.find(':', offset_begin);
					    std::string formatString = p.second.substr(offset_begin, offset_end);
					    if (formatString.size() != 1) 
					    {
						    throw SimulatorParserException();
					    }
					    
                        SimulatorUseCase::Var v;
                        v.type = SimulatorUseCase::Var::InOut; // don't care
                        v.inpos = inpos;
					    v.outpos = 0; // don't care
                        v.length = length;
					    v.render = static_cast<SimulatorUseCase::Var::RenderType>(formatString[0]);

                        START_VERBOSITY(pv_medium)
                        COUT( "Ilog: [" << token << "] " << v << std::endl );
                        END_VERBOSITY
                        newUseCase.ilog[token] = v;
                    }
                    else if (p.first == "RESP")
                    {
                        if (createResp) 
                        {
                            // no new resp yet, so create
                            newUseCase.resp.push_back(SimulatorUseCase::Mask());
						    newUseCase.respSize.push_back(0);
                            std::map<SimulatorUseCase::Token, SimulatorUseCase::Var> x;
                            newUseCase.ovar.push_back(x);
                            newUseCase.delay.push_back(0);
                            createResp = false;
                        }
                        // put into Resp vector
                        std::vector<unsigned char> hex = chr2hex(p.second);
                        std::copy(hex.begin(), hex.end(), std::back_inserter(newUseCase.resp[resp_idx]));
                    }
				    else if (p.first == "RESPSIZE") 
				    {
					    newUseCase.respSize[resp_idx] = strtol(p.second.c_str(),0,0);
                        START_VERBOSITY(pv_medium)
					    COUT( "RespSize: " << newUseCase.respSize[resp_idx] << std::endl );
                        END_VERBOSITY
				    }
                    else if (p.first == "OVAR")
                    {
                        // get the generated variables
                        // format: <token>:<startpos>:<length>
                        /// @todo do this better - use a tokenizer or something...
                        std::string::size_type offset_begin = 0;
                        std::string::size_type offset_end = p.second.find(':', offset_begin);
                        std::string token = p.second.substr(offset_begin, offset_end);
                        offset_begin = offset_end+1;
                        offset_end = p.second.find(':', offset_begin);
                        //START_VERBOSITY(pv_)
                        //COUT( "[[" << p.second.substr(offset_begin, offset_end).c_str() << "]]" << std::endl );
                        //END_VERBOSITY
                        unsigned int startpos = strtol(p.second.substr(offset_begin, offset_end).c_str(),0,0);
                        offset_begin = offset_end+1;
                        offset_end = p.second.find(':', offset_begin);
                        unsigned int length = strtol(p.second.substr(offset_begin, offset_end).c_str(),0,0);

                        SimulatorUseCase::Var v;
                        v.type = SimulatorUseCase::Var::Out;
                        v.inpos = 0;
                        v.outpos = startpos;
                        v.length = length;

                        START_VERBOSITY(pv_medium)
                        COUT( "Ovar: [" << token << "] " << v << std::endl );
                        END_VERBOSITY
                        newUseCase.ovar[resp_idx][token[0]] = v;
                    }
				    else if (p.first == "CONTINUE")
				    {
					    std::string continueMode = removeWhitespace(p.second);
					    std::transform(continueMode.begin(), continueMode.end(), continueMode.begin(), Uppercase());
					    
					    if (continueMode == "ON" || continueMode == "YES") 
					    {
						    newUseCase.continueMode = true;
					    }
					    else if (continueMode == "OFF" || continueMode == "NO")
					    {
						    newUseCase.continueMode = false;
					    }
					    else if (continueMode == "FIRST")
					    {
						    newUseCase.continueOnlyIfFirst = true;
					    }
					    else 
					    {
						    throw SimulatorParserException();
					    }
				    }
                    else if (p.first == "WAIT")
                    {
                        newUseCase.delay[resp_idx] = strtol(p.second.c_str(),0,0); // no error checking !!!
                    
					    resp_idx++; // After a Wait entry, we are finished with the response, 
                        createResp = true;
                    }
                    else if (p.first == "NAME" || p.first == "VERBOSE")
                    {
                        --i; // push it back, so we can read it next time in for loop
                        finished = true;
                    }
                    else 
                    {
                        START_VERBOSITY(pv_low)
                        COUT( "Processing completed" << std::endl );
                        END_VERBOSITY
                        finished = true;
                    }                
                }

                m_useCases.push_back(newUseCase);
            }
        }
	    // update the respSize (get default value, if we haven't set it yet)
	    // set the verbose flag
	    for (unsigned int j = 0; j < m_useCases.size(); ++j)
	    {
		    for (unsigned int k = 0; k < m_useCases[j].respSize.size(); ++k)
		    {
			    if (0 == m_useCases[j].respSize[k])
			    {
                    START_VERBOSITY(pv_medium)
				    COUT( "Usecase:" << j << " RespSize:" << k
					      << " defaulting to " << m_useCases[j].resp[k].size() << std::endl );
                    END_VERBOSITY
				    m_useCases[j].respSize[k] = m_useCases[j].resp[k].size();
			    }

			    // Some last minute sanity checking... 
			    // confirm that the resp length is the same as the length field (first 4 bytes little endian encoded)
			    const SimulatorUseCase::Mask & respX = m_useCases[j].resp[k];
			    
			    if (respX.size() != 0)  // skip - no response sent
			    {
				    unsigned int configuredLength = m_useCases[j].respSize[k];
				    unsigned int parsedLength = ((unsigned int)(respX[3]) << 24) + (respX[2] << 16) + (respX[1] << 8) + respX[0];
				    if ( configuredLength != parsedLength )
				    {
                        START_VERBOSITY(pv_low)
					    COUT( "Error: length of Resp (or RespSize) does not equal length in message (first 4 bytes) " 
						      << configuredLength << " != " << parsedLength << " for use case " 
						      << std::endl << m_useCases[j] << std::endl );
                        END_VERBOSITY
					    throw SimulatorParserException();
				    }
			    }

		    }
	    }

        START_VERBOSITY(pv_low)
	    COUT( m_useCases.size() << " Use Cases" << std::endl );
        END_VERBOSITY

    }


    SimulatorUseCaseList& SimulatorParser::getUseCases()
    {
        return m_useCases;
    }


    SimulatorParser::NVpair SimulatorParser::parseLine(const std::string& text, bool removeValueWS)
    {


        NVpair p = std::make_pair(
            removeWhitespace(
			    text.substr(0,text.find("="))), 
		    (removeValueWS ? 
			    removeWhitespace(convertQuoted(text.substr(text.find("=")+1))) 
			    : text.substr(text.find("=")+1) )
            );
	    std::transform(p.first.begin(), p.first.end(), p.first.begin(), Uppercase());
        START_VERBOSITY(pv_medium)
        COUT( "Name: [" << p.first << "] Value: [" << p.second << "]" << std::endl );
        END_VERBOSITY
        return p;
    }


    std::string SimulatorParser::getValue(const std::string &text)
    {
        return text.substr(text.find("=")+1) ;
    }


    std::string SimulatorParser::removeWhitespace(std::string text) 
    {
        text.erase( std::remove(text.begin(), text.end(), ' '), text.end() );
        text.erase( std::remove(text.begin(), text.end(), '\t'), text.end() );
        return text;
    }

    std::string SimulatorParser::convertQuoted(std::string text)
    {
	    // Replace with ascii text equivalent, so it can be fed into subsequent parsing stages
	    // format "text" <eol>  # no padding no null on end of string
	    // format "text"/size <eol> # padded with \0's
	    // whitespace is significant inside the string
	    // no support for escape chars.

	    std::string::size_type startOffset = text.find('"', 0);	
	    std::string::size_type endOffset = text.find('"', startOffset+1);
	    std::string::size_type startSizeSpec = text.find('/', endOffset+1);
	    std::string::size_type endSizeSpec = text.find(' ', startSizeSpec+1);
	    if (endSizeSpec == std::string::npos)
	    {
		    endSizeSpec = text.find('\t', startSizeSpec+1);
		    // if still npos, then we must have run off the end of the line
	    }
	    if (endSizeSpec == std::string::npos)
	    {
		    endSizeSpec = text.size();
	    }
	    unsigned int padSize = strtol(text.substr(startSizeSpec+1).c_str(),0,0);
	    
	    if (startOffset == std::string::npos)
	    {
		    return text;
	    }

	    std::string replacement;
	    for (unsigned int i = startOffset+1; i < endOffset; ++i)
	    {
		    replacement += "0123456789ABCDEF"[(text[i] & 0xf0) >> 4];
		    replacement += "0123456789ABCDEF"[(text[i] & 0x0f)];
	    }

	    if (startSizeSpec != std::string::npos)
	    { 
		    // we have a size spec so process it
		    if (padSize == 0 || padSize < replacement.size()/2) 
		    {
                START_VERBOSITY(pv_low)
			    COUT( "Error: either invalid size or size specified is too small" << std::endl );
                END_VERBOSITY
			    throw SimulatorParserException();
		    }
		    // string not got push_back() ;(		std::fill_n(std::back_inserter(replacement), padSize*2 - replacement.size(), '0'); // two chars per byte
		    replacement.append(padSize*2 - replacement.size(), '0');
	    }

        START_VERBOSITY(pv_medium)
	    COUT( ">>>replacing [" << text << "]" << std::endl
		      << ">>>with [" << replacement << "]" << std::endl );
        END_VERBOSITY

    //	text.replace(startOffset, endOffset, replacement);
	    if (startSizeSpec != std::string::npos && endSizeSpec != std::string::npos)
	    {
		    text.erase(startSizeSpec,endSizeSpec);
	    }
	    text.erase(startOffset, endOffset - startOffset +1 );
	    text.insert(startOffset, replacement); 
        START_VERBOSITY(pv_medium)
	    COUT( ">>>result is [" << text << "]" << std::endl );
        END_VERBOSITY
	    return text;
    }

    std::vector<unsigned char> SimulatorParser::generateMask(const std::string& theText) const
    {
        std::string text = theText;
        std::replace(text.begin(), text.end(), '*', 'f'); // wild
	    return chr2hex(text);
    }

    std::vector<unsigned char> SimulatorParser::generatePattern(const std::string& theText) const
    {
        std::string text = theText;
	    std::replace_if(text.begin(), text.end(), std::bind2nd(std::not_equal_to<unsigned char>(),(char)'*'),(char)'f');
        std::replace(text.begin(), text.end(), '*', '0'); // wild
	    return chr2hex(text);
    }

    std::vector<unsigned char> SimulatorParser::chr2hex(const std::string& theText) const
    {
        std::string text = theText;
    //    std::replace(text.begin(), text.end(), '*', 'f'); // wild

        //START_VERBOSITY(pv_)
        //COUT( "hex: " << std::flush );
        //END_VERBOSITY
        std::vector<unsigned char> hex;
        for (std::string::const_iterator i = text.begin(); i != text.end(); i+=2) 
        {
            // There are better ways to do this...
            unsigned char hexval = 0;
            char * buf = new char[2];
            buf[1] = '\0';
            long val;
        
            buf[0] = *i;
            val = strtol(buf,0,16);
            //START_VERBOSITY(pv_)
            //COUT( "[hi:" << val << "]" );
            //END_VERBOSITY
            hexval = (unsigned char)(val * 0x10); // high nibble
            buf[0] = *(i+1);
            val = strtol(buf,0,16);
            //START_VERBOSITY(pv_)
            //COUT( "[lo:" << val << "]" );
            //END_VERBOSITY
            hexval += (unsigned char)val ; // low nibble
            delete [] buf;

            //START_VERBOSITY(pv_)
            //COUT( (int)hexval << "," << std::flush );
            //END_VERBOSITY
            hex.push_back( hexval ); /* *i * 0x10 & *(i+1) */ 
        }
        //START_VERBOSITY(pv_)
        //COUT( std::endl );
        //END_VERBOSITY
        return hex;
    }

}
