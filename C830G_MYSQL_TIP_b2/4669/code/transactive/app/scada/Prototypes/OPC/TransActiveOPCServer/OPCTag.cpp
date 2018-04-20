/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/OPCTag.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#include "stdafx.h"

#include <comdef.h>

#include "CorbaUtil.h"
#include "CommonDmIf.h"
#include "DataPointDmIf.h"
#include "OPCTag.h"

OPCTag::OPCTag ( const std::string & attributeDataType,
				 const std::string & dataPointName,
				 const std::string & dataPointCorbaName,
				 unsigned long entityKey )
:
m_dataPointName ( dataPointName ),
m_dataPointCorbaName ( dataPointCorbaName ),
m_entityKey ( entityKey )
{
	// hard coded stuffs
	m_quality = OPC_QUALITY_GOOD;
	m_timestamp = 0;
	VariantInit ( & m_value );
	if ( attributeDataType == "BYTE" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_BYTE;
		m_value.vt = VT_UI1;
		m_value.bVal = 10;
	}
	else if ( attributeDataType == "WORD" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_SHORT;
		m_value.vt = VT_I2;
		m_value.iVal = 10;
	}
	else if ( attributeDataType == "INT" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_SHORT;
		m_value.vt = VT_I2;
		m_value.iVal = 10;
	}
	else if ( attributeDataType == "UINT" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_UNSIGNED_SHORT;
		m_value.vt = VT_I2;
		m_value.iVal = 10;
	}
	else if ( attributeDataType == "DINT" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_LONG;
		m_value.vt = VT_I4;
		m_value.lVal = 10;
	}
	else if ( attributeDataType == "UDINT" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_UNSIGNED_LONG;
		m_value.vt = VT_I4;
		m_value.lVal = 10;
	}
	else if ( attributeDataType == "IEEE_32" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_FLOAT;
		m_value.vt = VT_R4;
		m_value.fltVal = 10;
	}
	else if ( attributeDataType == "IEEE_64" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_DOUBLE;
		m_value.vt = VT_R8;
		m_value.dblVal = 10;
	}
	else if ( attributeDataType == "BOOL" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_BOOLEAN;
		m_value.vt = VT_BOOL;
		m_value.boolVal = TRUE;
	}
	else if ( attributeDataType == "TEXT" )
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_TEXT;
		m_value.vt = VT_BSTR;
		_bstr_t bstr_string ( "BLAH" );
		m_value.bstrVal = bstr_string;
	}
	else
	{
		m_dataType = ATTRIBUTE_DATA_TYPE_DOUBLE;
		m_value.vt = VT_R8;
		m_value.dblVal = 10;
	}



/*
	// resolve the datapoint object from Corba Service
	try
	{
		// request nexus common interface to resolve the DataPointObject
		CORBA::Object_var theCORBA_obj = 0;
		theCORBA_obj = CorbaUtil::GetInstance().ResolveName ( m_dataPointCorbaName.c_str() );

		// if successfull
		if ( !( CORBA::is_nil ( theCORBA_obj ) ) )
		{
			// try to convert the CORBA object reference to a DataPoint
			nexus::DataPointDmIf_var  datapoint = 0;
			datapoint = nexus::DataPointDmIf::_narrow ( theCORBA_obj );

			// if successfull
			if ( !( CORBA::is_nil ( datapoint ) ) )
			{
				// update the value of the newly created attribute with the value
				// obtained from the object resolved from the CORBA Naming Service

				nexus::DataPointDmIf::DataPointState_var datapointState = 0;
				datapointState = datapoint->GetDataPointState();

				// assign timestamp
				m_timestamp = datapointState->timeStamp;

				// assign value
				VariantInit ( & m_value );
				switch ( datapointState->value._d() )
				{
					case nexus::DataPointDmIf::dpvFloat:
						{
							if ( attributeDataType == "BYTE" )
							{
								m_dataType = ATTRIBUTE_DATA_TYPE_BYTE;
								m_value.vt = VT_UI1;
								m_value.bVal = ( unsigned char ) datapointState->value.floatValue();
							}
							else if ( attributeDataType == "WORD" )
							{
								m_dataType = ATTRIBUTE_DATA_TYPE_SHORT;
								m_value.vt = VT_I2;
								m_value.iVal = ( short ) datapointState->value.floatValue();
							}
							else if ( attributeDataType == "INT" )
							{
								m_dataType = ATTRIBUTE_DATA_TYPE_SHORT;
								m_value.vt = VT_I2;
								m_value.iVal = ( int ) datapointState->value.floatValue();
							}
							else if ( attributeDataType == "UINT" )
							{
								m_dataType = ATTRIBUTE_DATA_TYPE_UNSIGNED_SHORT;
								m_value.vt = VT_I2;
								m_value.iVal = ( unsigned int ) datapointState->value.floatValue();
							}
							else if ( attributeDataType == "DINT" )
							{
								m_dataType = ATTRIBUTE_DATA_TYPE_LONG;
								m_value.vt = VT_I4;
								m_value.lVal = ( long ) datapointState->value.floatValue();
							}
							else if ( attributeDataType == "UDINT" )
							{
								m_dataType = ATTRIBUTE_DATA_TYPE_UNSIGNED_LONG;
								m_value.vt = VT_I4;
								m_value.lVal = ( unsigned long ) datapointState->value.floatValue();
							}
							else if ( attributeDataType == "IEEE_32" )
							{
								m_dataType = ATTRIBUTE_DATA_TYPE_FLOAT;
								m_value.vt = VT_R4;
								m_value.fltVal = datapointState->value.floatValue();
							}
							else if ( attributeDataType == "IEEE_64" )
							{
								m_dataType = ATTRIBUTE_DATA_TYPE_DOUBLE;
								m_value.vt = VT_R8;
								m_value.dblVal = ( double ) datapointState->value.floatValue();
							}
							else
							{
								m_dataType = ATTRIBUTE_DATA_TYPE_DOUBLE;
								m_value.vt = VT_R8;
								m_value.dblVal = ( double ) datapointState->value.floatValue();
							}

							break;
						}
					case nexus::DataPointDmIf::dpvBoolean:
						{
							m_dataType = ATTRIBUTE_DATA_TYPE_BOOLEAN;
							m_value.vt = VT_BOOL;
							m_value.boolVal = datapointState->value.booleanValue();
							break;
						}
					case nexus::DataPointDmIf::dpvText:
						{
							m_dataType = ATTRIBUTE_DATA_TYPE_TEXT;
							m_value.vt = VT_BSTR;
							_bstr_t bstr_string ( datapointState->value.stringValue() );
							m_value.bstrVal = bstr_string;
							break;
						}
					default:
						{
							break;
						}
				}
			}
		}
	}
	catch ( ... )
	{
		// do nothing
	}
*/
}

OPCTag::OPCTag ( const OPCTag & obj )
:
m_dataType ( obj.m_dataType ),
m_dataPointName ( obj.m_dataPointName ),
m_dataPointCorbaName ( obj.m_dataPointCorbaName ),
m_entityKey ( obj.m_entityKey ),
m_quality ( obj.m_quality ),
m_timestamp ( obj.m_timestamp )
{

}

OPCTag & OPCTag::operator= ( const OPCTag & rhs )
{
	m_dataType = rhs.m_dataType;
	m_dataPointName = rhs.m_dataPointName;
	m_dataPointCorbaName = rhs.m_dataPointCorbaName;
	m_entityKey = rhs.m_entityKey;
	m_quality = rhs.m_quality;
	m_timestamp = rhs.m_timestamp;

	return *this;
}

OPCTag::~OPCTag()
{
	if ( m_value.vt == VT_BSTR )
	{
		_bstr_t bstr_string ( "" );
		m_value.bstrVal = bstr_string;
	}

	VariantClear ( & m_value);
}

ATTRIBUTE_DATA_TYPE_ENUM OPCTag::getAttributeDataType ( void ) const
{
	return m_dataType;
}

const std::string & OPCTag::getDataPointName ( void ) const
{
	return m_dataPointName;
}

const std::string & OPCTag::getDataPointCorbaName ( void ) const
{
	return m_dataPointCorbaName;
}

unsigned long OPCTag::getEntityKey ( void ) const
{
	return m_entityKey;
}

long OPCTag::getTimeStamp ( void )
{
	return m_timestamp;
}

WORD OPCTag::getQuality ( void ) const
{
	return m_quality;
}

VARIANT OPCTag::getValue ( void ) const
{
	return m_value;
}

