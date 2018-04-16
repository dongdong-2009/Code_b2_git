#pragma once;

/*
    vecNameValues.push_back(itmNameValue);
    vecNameValues.back().DisplayName = ParamArgs;
    vecNameValues.back().Name = ParamArgs;
    vecNameValues.back().Item = m_refParameter.args;
    vecNameValues.back().Type = eString;
*/

#define EDITOR_PUSH_PARAMETER(VECTOR, ITEM, PARAM, NAME, DISPLAY_NAME, TYPE, VALID) \
	VECTOR.push_back(ITEM); VECTOR.back().Name = NAME; VECTOR.back().DisplayName = DISPLAY_NAME;\
	convertToDisplayValue(PARAM, VECTOR.back().Item); VECTOR.back().Type = TYPE; VECTOR.back().Valid = VALID
