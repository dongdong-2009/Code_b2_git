#pragma once

#include "IParser.h"

enum ParserType
{
	ECsvType = 0
};

class ParserDataAccessFactory : public IParser
{
	public:
		ParserDataAccessFactory(void);
		~ParserDataAccessFactory(void);
		
		static void createParser (ParserType parserType = ECsvType);
		static IParser * getParser ();
		static void clearParser ();
	
	private:
		static IParser * m_parser;
};
