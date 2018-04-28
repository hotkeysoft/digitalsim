#include "stdafx.h"
#include "IOPin.h"
#include "GateBase.h"

IOPin::IOPin(GateBase *parentGate, const char* name, IO_DIRECTION direction) : 
	m_name(name), m_direction(direction), m_state(IOPin::UNDEF), m_parentGate(parentGate)
{
	assert(parentGate != NULL);
	assert(name != NULL);
}

IOPin::~IOPin()
{
}

std::string IOPin::GetFullName()
{
	std::ostringstream os;
	os << m_parentGate->GetFullName() << "." << m_name;
	return os.str();
}

void IOPin::Set(IO_STATE state) 
{
	//if (m_direction == IO_DIRECTION::OUTPUT)
	//{
	//	throw std::exception("Cannot set input pin");
	//}

	auto& connectedPins = m_parentGate->GetConnectedPins(this);

	m_state = state; 
	if (m_direction == IO_DIRECTION::INPUT && connectedPins.size() > 0)
	{
		// Connect to internal gates
		for (auto connected : connectedPins)
		{
			connected.GetTarget()->Set(state);
		}
	}
	if (m_parentGate != NULL && m_direction == IO_DIRECTION::INPUT)
	{
		m_parentGate->ComputeState();
	}

	if (m_direction == IO_DIRECTION::OUTPUT)
	{
		for (auto connected : connectedPins)
		{
			connected.GetTarget()->Set(state);
		}		
	}
}

void IOPin::ConnectTo(IOPin* target)
{
	m_parentGate->ConnectPins(this, target);
}
