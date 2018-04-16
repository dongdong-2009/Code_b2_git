/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/bus/radio/RadioBus/src/RadioTypes.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include <iostream>
#include <map>
#include <list>
#include <utility>
#include <vector>
#include <functional>


#ifndef RADIO_TYPES_H_INCLUDED
#define RADIO_TYPES_H_INCLUDED

#include "boost/algorithm/string/case_conv.hpp"
#include "core/utilities/src/DebugUtil.h"

#include "bus/radio/radio_bus/src/RadioResourceMFTServerAttributes.h"


/**
 * The number used by the CIE to identify the resource
 * Process: allocated by Motorola Console API
 */
typedef unsigned int LogicalChannel;

/**
 * The number used to uniquely identify a resource in the radio system. This is not the SSI,
 * but it can be used to determine the SSI;
 */
typedef unsigned long ResourceIdentifier;

/**
 * A transActive database key used to identify an operator's saved selection.
 */
typedef unsigned long TransactiveResourceIdentifier;


/*************************************************** 
 * Enumerations
 ***************************************************/
enum RadioResourceType
{
	RR_NIL,

	RR_RADIO = 'A',

	RR_TALKGROUP = 'B',

	RR_ANNGROUP = 'C',

	RR_PATCHGROUP = 'D',

	RR_DYNGROUP = 'E',

	RR_MSELECT = 'F',

	RR_SITE = 'G',

	RR_MSITE = 'H',

	RR_TRAIN = 'I', 	/** Used in case a trainborne radio needs to be referenced by its train
					 * identification information.
					 */
	RR_PRIVATECALL = 'P',
	RR_BIM = 'M',
	RR_SPI = 'S'
};


enum ApplicationState
{
	RA_UNKNOWN,

	RA_INITIALISATION_MODE,
	
	RA_OPERATION_MODE,

	RA_CONFIGURATION_MODE,

	RA_CONNECTION_FAILURE_MODE,

	RA_MCC_CONNECTION_ESTABLISH
};

enum ApiState
{
	RA_ASSIGN_STATE,

	RA_DEASSIGN_STATE,

	RA_SELECT_STATE,

	RA_DESELECT_STATE
};



/*************************************************** 
 * RadioResource structure
 ***************************************************/
struct RadioResource
{
	public:

		RadioResource(): type(RR_NIL),id(0),ssi(0),alias(""),hasError(false) {};

		~RadioResource() {};

        void operator=(const RadioResource& p)
        {
            this->type = p.type;
            this->id = p.id;
            this->ssi = p.ssi;
            this->alias = p.alias;
            this->hasError = p.hasError;
        }

		void initialise()
		{
			type  = RR_NIL;
			id    = 0;
			ssi   = 0;
			alias = "";
            hasError = false;
		};


	public:

		RadioResourceType type;
		/**
		 * Either a Radio Resource ID, or transactive ID (msel, msel-site, pg, dg)
		 */
		ResourceIdentifier id;

		unsigned long ssi;

		std::string alias;

        bool hasError; // used when the resource is part of a group
			           // to indicate error state

};

struct SortAliasAsc
{
	bool operator()(const RadioResource& r1, const RadioResource& r2)
	{
		std::string s1(r1.alias.c_str()), s2(r2.alias.c_str());
		boost::to_upper(s1); boost::to_upper(s2);
		return s1.compare(s2) < 0;
	}
};

struct SortAliasDesc
{
	bool operator()(const RadioResource& r1, const RadioResource& r2)
	{
		std::string s1(r1.alias.c_str()), s2(r2.alias.c_str());
		boost::to_upper(s1); boost::to_upper(s2);
		return s1.compare(s2) > 0;
	}
};

struct SortIdAsc
{
	bool operator()(const RadioResource& r1, const RadioResource& r2)
	{
		std::stringstream o1, o2;
		o1 << r1.id; o2 << r2.id;
		std::string s1 = o1.str(), s2 = o2.str();
		return s1.compare(s2) < 0;
	}
};

struct SortIdDesc
{
	bool operator()(const RadioResource& r1, const RadioResource& r2)
	{
		std::stringstream o1, o2;
		o1 << r1.id; o2 << r2.id;
		std::string s1 = o1.str(), s2 = o2.str();
		return s1.compare(s2) > 0;
	}
};

struct ResourceButton
{
    unsigned long key;

    bool isProfile;
    unsigned int buttonRef;

    RadioResourceType type;
    ResourceIdentifier id;
};

struct PredefinedMessage
{
    unsigned long key;
    std::string shortName;
    std::string messageText;
	bool type; //true: radiostatus message; otherwise, SDS message;; TD14312
};

struct MessageInbox
{
    unsigned long key;
    time_t date;
    bool isRead;
    std::string messageText;
	std::string messageSource;
};

typedef std::vector<RadioResource> ResourceCollection;

typedef std::vector<ResourceButton> ButtonCollection;

typedef std::pair<std::string, std::string> MonitorStatus;

struct CallForwardingStatus
{
	short condition;
	unsigned long forwardingISSI;
	unsigned long forwardedISSI;
};

/*************************************************** 
 * Channel Collection
 ***************************************************/
typedef std::map<ResourceIdentifier,LogicalChannel> ChannelCollection;


/*************************************************** 
 * ResourceGroup Collection
 * 1. map<ID, group ID>
 * 2. map<ID, members>
 ***************************************************/
typedef std::map<TransactiveResourceIdentifier,unsigned int> ResourceGroupCollection;
typedef std::map<TransactiveResourceIdentifier,ResourceCollection> ResourceGroupMembersMap;



/*************************************************** 
 * Used to store all Radio resource types, 
 * including those for the server agent
 ***************************************************/
struct rescmp
{
  bool operator()(const RadioResource& a, const RadioResource& b) const
  {
	  if (a.type < b.type)
	  {
		  return true;
	  }
	  else if (a.type == b.type)
	  {
		  return a.id < b.id;
	  }
	  return false;
  }
};
typedef std::map<RadioResource, RadioResourceMFTServerAttributes,rescmp> MFTResourceCollection;


/*************************************************** 
 * ?? Button Collection
 ***************************************************/


/******************************************************** 
 * Functions: Using the following STL prototype to 
 * understand codes
 ********************************************************/
class compareRadioResource : public std::binary_function<RadioResource, RadioResource, bool>
{
	public:
		compareRadioResource() {};

		virtual ~compareRadioResource() {};

		const result_type operator()(const first_argument_type& a, const second_argument_type& b)
		{
			return (a.type == b.type && a.id == b.id);
		};

};


class isNilRadioResource : public std::unary_function<RadioResource, bool>
{

	public:

		isNilRadioResource() {};

		virtual ~isNilRadioResource() {};


		const result_type operator() (const argument_type & a ) 
		{
			RadioResource b;

			//return compareRadioResource()(a, b);
			return ( a.type == b.type && a.id == b.id );
		};

};

typedef std::pair<RadioResource,RadioResourceMFTServerAttributes> RadioMFTPair;

class CompareResource 
{
	public:

		RadioResource b;

		CompareResource(RadioResource resource): b(resource)  { };
		~CompareResource() {};

		bool operator()(RadioMFTPair pair) const 
		{
			RadioResource a = pair.first;
			return (a.type == b.type && a.id == b.id);
		}
};

#endif
