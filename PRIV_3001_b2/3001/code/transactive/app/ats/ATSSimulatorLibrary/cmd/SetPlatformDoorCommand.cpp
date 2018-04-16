/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/SetPlatformDoorCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Definition/Implementation file for ATSSimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/SetPlatformDoorCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimFlexiMask.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

SetPlatformDoorCommand::SetPlatformDoorCommand(CommandOwner * owner)
: LeafCommand(owner, "door")
{
}

bool SetPlatformDoorCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
    if (args.size() < 2)
    {
        err << "ERROR: insufficient parameters. Expected at least 2, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    TableIdentifier tableId = AtsPlatform;
    location = mergeStations(tableId, location);
	
    //
    // table address
	unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
	if (tAddr == TableAddressRegister::INVALID_ADDRESS)
	{
        err << "ERROR: could not determine table address for table \"AtsPlatform\" (ouch)" << std::endl;
        return false;
	}
    query->setTableAddress(tAddr);

    //
	// the record number
    std::string recordstr = args.front(); args.pop_front();
    bool ok = false;
    unsigned long record = query->str2ul(recordstr.c_str(), err, ok);
    if ( ! ok )
    {
        //TODO log a warning
        err << "WARNING: could not convert value to record number: " << recordstr << std::endl;
        return false;
    }
    int numrecords = query->setRecord(record);

    std::string s;
    TokenList fvpairs;
    TokenList::const_iterator argiter = args.begin();

    FlexiMask<Word> doormask;
    // everything that isn't explicitly set or toggled should be left alone
    doormask.setLeaveMask(0xFFFF);

    // no, not a bitmap of doors.  This assists in mapping from doornumber to
    // bit index:
    // Door   X  X  1  2  3  4  5  6  X  X  7  8  9 10 11 12
    // Bit   15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
    int doorBitIndexMap[12]={13,12,11,10,9,8,5,4,3,2,1,0};

    while ( argiter != args.end() )
    {
        bool ok = false;
        s = *argiter;
        fvpairs.clear();
        ATSSimUtility::tokenise(s.c_str(), fvpairs, "=");

        if (fvpairs.size() == 2)
        {
            //
            // Could be either doornum=<U|I|T>  or  close=<start|end>

            std::string fieldstr = fvpairs.front();
            fvpairs.pop_front();

            std::string valuestr = fvpairs.front();
            fvpairs.pop_front();

            if (fieldstr == "close")
            {
                if (valuestr=="start")
                {
                    query->pushArg(54,0);
                }
                else if (valuestr=="end")
                {
                    query->pushArg(54,1);
                }
                else
                {
                    err << "ERROR: unrecognised platform door close pre-warning argument: " << valuestr << std::endl;
                    return false;
                }
            }
            else if (fieldstr == "psd")
            {
                if (!doormask.setFromTL01String(valuestr))
                {
                    err << "ERROR: mask incorrect, must be 16 chars and consist of 0|1|T|L :" << valuestr << std::endl;
                }
            }
            else // doornum=<U|I|T>
            {
                unsigned long doornum = query->str2ul( fieldstr.c_str(), err, ok );
                if ( ok )
                {
                    if ( valuestr=="U" )
                    {
                        // Not to inhibit state is 1
                        doormask.setValueBit(doorBitIndexMap[doornum-1]);
                    }
                    else if ( valuestr=="I" )
                    {
                        // To inhibit state is 0
                        doormask.clearValueBit(doorBitIndexMap[doornum-1]);
                    }
                    else if ( valuestr=="T" )
                    {
                        doormask.setToggleBit(doorBitIndexMap[doornum-1]);
                    }
                }
                else
                {
                    err << "ERROR: unrecognised platform door argument: " << fieldstr << std::endl;
                    return false;
                }
            }
        }
        else
        {
            //TODO log a warning ?
            err << "WARNING: Could not parse. Expected \"field=value\" pair, got: " << s << std::endl;
            return false;
        }

        argiter++;
    }

    if (doormask.isUsed())
    {
        if (doormask.isValueUsed())
        {
            query->pushArg(MaskFieldIndicator,doormask.getValueMask());
        }
        if (doormask.isToggleUsed())
        {
            query->pushArg(ToggleMaskFieldIndicator,doormask.getToggleMask());
        }
        query->pushArg(LeaveMaskFieldIndicator,doormask.getLeaveMask());
        query->pushArg(MaskEndFieldIndicator,0);
        query->pushArg(56,0); // the value is unimportant
    }

    return true;
}


void SetPlatformDoorCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " <record number> [<number>=[U|I|T]] [close=[start|end]] [psd=mask]" << std::endl;
}

