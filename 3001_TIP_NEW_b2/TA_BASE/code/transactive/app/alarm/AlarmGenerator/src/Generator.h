/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/Generator.h,v $
 * @author:  Cameron Rochester
 * @version: $Revision: 1.1.2.1 $
 *
 * Last modification: $Date: 2003/06/23 05:00:14 $
 * Last modified by:  $Author: andrewdc $
 *
 * A thread to call getAlarms() on the Alarm Agent while other threads are attempting to 
 * submit, receive, close, etc.
 */

#if !defined(GENERATOR_H)
#define GENERATOR_H
#include <iostream>
#include <vector>


class CAlarmGeneratorDlg;

class Generator
{
public:

    Generator(CAlarmGeneratorDlg *dialog);
	virtual ~Generator() {}
	int start();
	
private:

	int setUpEntityType();
	std::vector<int> setUpSubmitterEntities(const int count,const int entityTypeKey);
	int setUpTestConsole();
	int setUpTestOperator();
	int setUpTestSession(int operatorKey, int consoleKey);
	void outputToDialog(std::string outputString);
	void usage();
	
	CAlarmGeneratorDlg* m_dialog;

};

#endif // !defined(GETALARMTHREAD_H)