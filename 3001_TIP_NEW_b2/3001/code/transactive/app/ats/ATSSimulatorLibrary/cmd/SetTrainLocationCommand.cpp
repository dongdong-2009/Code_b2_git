/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/cmd/SetTrainLocationCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Definition/Implementation file for ATSSimCommand classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/cmd/SetTrainLocationCommand.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_IRS_App::ATS_Sim;

SetTrainLocationCommand::SetTrainLocationCommand(CommandOwner * owner)
: LeafCommand(owner, "location")
{
}

bool SetTrainLocationCommand::makeQuery(TokenList & args, const std::string & argstr, UserQuery * query, LocationType location, std::ostream & err)
{
    //
    // basic argument checking
	//TD17131
    if (args.size() != 4 && args.size() != 5)
    {
        err << "ERROR: incorrect number of parameters. Expected exactly 4, got "
            << args.size() << ": " << argstr << std::endl;
        return false;
    }

    TableIdentifier tableId = AtsTrain;
    //
    // In order to correctly fetch the table address
    // At the stations, the Ats tables are shared, and have location STATION
    // rather than MSS|SS1|SS2.
    if ( (location==MSS) || (location==SS1) || (location==SS2) )
    {
        location = STATION;
    }
	
    //
    // table address
	unsigned short tAddr = TableAddressRegister::getInstance().getAddress(tableId, location);
	if (tAddr == TableAddressRegister::INVALID_ADDRESS)
	{
        err << "ERROR: could not determine table address for table \"AtsTrain\" (ouch)" << std::endl;
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

    //
    // branch id
    std::string branch_id_str = args.front(); args.pop_front();
    std::string branch_id_cnvrtd_str = "";
    Word branch_id = ATSSimUtility::convertBranchId2IntegerString(branch_id_str, branch_id_cnvrtd_str);
    if (branch_id == 0)
    {
        std::cerr << "ERROR: invalid branch id argument:" << branch_id_str << std::endl;
        return false;
    }
    query->pushArg(10, branch_id);

    //
    // fb abscissa
    std::string fb_abscissa_str = args.front(); args.pop_front();
	UserQuery::FieldValueType fb_abscissa = query->str2FieldValueType(fb_abscissa_str.c_str(), err, ok);
    if ( ! ok )
    {
        //TODO log a warning
        err << "WARNING: could not convert value to fb abscissa: " << fb_abscissa_str << std::endl;
        return false;
    }
    query->pushArg(12, fb_abscissa);

    //
    // hc abscissa
    std::string hc_abscissa_str = args.front(); args.pop_front();
    UserQuery::FieldValueType hc_abscissa = query->str2FieldValueType(hc_abscissa_str.c_str(), err, ok);
    if ( ! ok )
    {
        //TODO log a warning
        err << "WARNING: could not convert value to head car abscissa: " << hc_abscissa_str << std::endl;
        return false;
    }
    query->pushArg(14, hc_abscissa);

	//TD17131
	if (args.size()==1)
	{
		//
		// validity flag
		std::string valitity_str = args.front(); args.pop_front();
		UserQuery::FieldValueType valitity = query->str2FieldValueType(valitity_str.c_str(), err, ok);
		if ( ! ok )
		{
			//TODO log a warning
			err << "WARNING: could not convert value to train vality flag: " << valitity_str << std::endl;
			return false;
		}
		query->pushArg(95, valitity);
	}

    return true;
}


void SetTrainLocationCommand::usage(const std::string & name, std::ostream & os) const
{
    os << name << " <record number> <branch id> <fb abscissa> <hc abscissa> <optional:localisation validity flag>" << std::endl;
}

