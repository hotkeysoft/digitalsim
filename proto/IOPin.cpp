#include "stdafx.h"
#include "IOPin.h"
#include "GateBase.h"

IOPin::IOPin(GateBase *parentGate, std::string name, IO_DIRECTION direction) : m_name(name), m_direction(direction), m_state(IOPin::UNDEF), m_parentGate(parentGate)
{
}

IOPin::~IOPin()
{
}

void IOPin::Set(IO_STATE state) 
{
	//if (m_direction == IO_DIRECTION::OUTPUT)
	//{
	//	throw std::exception("Cannot set input pin");
	//}

	m_state = state; 
	if (m_parentGate != NULL && m_direction == IO_DIRECTION::INPUT)
	{
		m_parentGate->ComputeState();
	}
	if (m_direction == IO_DIRECTION::OUTPUT)
	{
		for (auto connected : m_connectedPins)
		{
			connected->Set(state);
		}		
	}
}

void IOPin::ConnectTo(IOPin* target)
{
	GateBase* sourceParent = m_parentGate->GetParent();
	GateBase* targetParent = target->m_parentGate;

	if (target == NULL)
	{
		throw std::invalid_argument("Pin is NULL");
	}

	if (target->m_direction != IOPin::INPUT)
	{
		throw std::invalid_argument("Cannot connect to OUTPUT pin");
	}

	// TODO:Not sure
	if (target->m_parentGate == this->m_parentGate)
	{
		throw std::invalid_argument("Cannot connect to self");
	}

	// TODO:Hi-Z
	if (m_connectedPins.find(target) != m_connectedPins.end())
	{
		throw std::invalid_argument("Already connected to target pin");
	}

	m_connectedPins.insert(target);
}
