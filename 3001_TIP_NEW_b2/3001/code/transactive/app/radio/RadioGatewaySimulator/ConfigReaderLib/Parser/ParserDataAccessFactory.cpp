
#include "ParserDataAccessFactory.h"
#include "Csv\CsvParser.h"

IParser * ParserDataAccessFactory::m_parser = NULL;

ParserDataAccessFactory::ParserDataAccessFactory(void)
{
	
}

ParserDataAccessFactory::~ParserDataAccessFactory(void)
{
}


void ParserDataAccessFactory::createParser (ParserType parserType)
{
	switch (parserType)
	{
		case ECsvType:
			m_parser = new CsvParser ();
			break;
	}

}
void ParserDataAccessFactory::clearParser ()
{
	if (m_parser)
	{
		delete m_parser;
		m_parser = NULL;
	}
}


IParser * ParserDataAccessFactory::getParser ()
{
	return m_parser;
}