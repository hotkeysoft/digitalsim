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

void IOPin::ConnectTo(IOPin* input)
{
	assert(input != NULL);
	assert(input->m_direction == IOPin::INPUT);
	assert(input->m_parentGate != this->m_parentGate); // not sure
	assert(m_connectedPins.find(input) == m_connectedPins.end());

	m_connectedPins.insert(input);
}
