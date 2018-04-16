/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/generic_simulator/src/GenericSimulator.cpp $
  * @author:  KL Team
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * This is the main controller for the simulation.  It basically controls
  * a bunch of ISimulationModules.  A CommandMonitor controls an instance of this.
  *
  */

// GenericSimulator.cpp
//
// implementation

#include "GenericSimulator.h"
#include <iostream>

using namespace TA_Base_Bus;

GenericSimulator::GenericSimulator() : m_isRunning(false)
{
}


GenericSimulator::~GenericSimulator()
{
	std::vector<ISimulationModule *>::iterator it = m_modules.begin();
	while (it != m_modules.end())
	{
		(*it)->stopSim();
		it++;
	}
	it = m_modules.begin();
	while (it != m_modules.end())
	{
		(*it)->destroySim();
		it++;
	}
}


void 
GenericSimulator::addModule(ISimulationModule *module)
{
	m_modules.push_back(module);
}


void 
GenericSimulator::removeModule(ISimulationModule *module)
{
	std::vector<ISimulationModule *>::iterator it = m_modules.begin();
	while (it != m_modules.end())
	{
		if (*it == module)
		{
			m_modules.erase(it);
			break;
		}
		it++;
	}
}


ISimulationModule*
GenericSimulator::getModuleByName(const std::string& name)
{
	std::vector<ISimulationModule *>::iterator it = m_modules.begin();
	while (it != m_modules.end())
	{
		if ((*it)->getName() == name)
		{
			return *it;
		}
		it++;
	}

	return 0;
}


void 
GenericSimulator::startSimulation()
{
	if (!m_isRunning)
	{
		std::vector<ISimulationModule *>::iterator it = m_modules.begin();
		while (it != m_modules.end())
		{
			(*it)->startSim();
			it++;
		}
		m_isRunning = true;
	}
}


void 
GenericSimulator::restartSimulation()
{
	if (!m_isRunning)
	{
		std::vector<ISimulationModule *>::iterator it = m_modules.begin();
		while (it != m_modules.end())
		{
			(*it)->restartSim();
			it++;
		}
		m_isRunning = true;
	}
}


void 
GenericSimulator::stopSimulation()
{
	if (m_isRunning)
	{
		std::vector<ISimulationModule *>::iterator it = m_modules.begin();
		while (it != m_modules.end())
		{
			(*it)->stopSim();
			it++;
		}
		m_isRunning = false;
	}
}
