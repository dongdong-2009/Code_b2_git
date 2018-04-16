/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/OPCTag.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#ifndef OPCTAG_H
#define OPCTAG_H

#include <string>

#include "opcda.h"
#include "AttributeDataTypeEnum.h"

class OPCTag
{
private:

	OPCTag() {};

	ATTRIBUTE_DATA_TYPE_ENUM m_dataType;
	std::string m_dataPointName;
	std::string m_dataPointCorbaName;
	unsigned long m_entityKey;
	VARIANT m_value;
	long m_timestamp;
	WORD m_quality;

public:

	OPCTag ( const std::string & attributeDataType,
			 const std::string & dataPointName,
			 const std::string & dataPointCorbaName,
			 unsigned long entityKey );

	OPCTag ( const OPCTag & obj );

	OPCTag & operator= ( const OPCTag & rhs );

	virtual ~OPCTag();

	ATTRIBUTE_DATA_TYPE_ENUM getAttributeDataType ( void ) const;

	const std::string & getDataPointName ( void ) const;

	const std::string & getDataPointCorbaName ( void ) const;

	unsigned long getEntityKey ( void ) const;

	long getTimeStamp ( void );

	WORD getQuality ( void ) const;

	VARIANT getValue ( void ) const;

};

#endif