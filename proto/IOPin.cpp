#include "stdafx.h"
#include "IOPin.h"
#include "GateBase.h"

IOPin::IOPin(GateBase *parentGate, const char* name, IO_DIRECTION direction) : m_name(name), m_direction(direction), m_state(IOPin::UNDEF), m_parentGate(parentGate)
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

	m_state = state; 
	if (m_direction == IO_DIRECTION::INPUT && m_connectedPins.size() > 0)
	{
		// Connect to internal gates
		for (auto connected : m_connectedPins)
		{
			connected->Set(state);
		}
	}
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
	if (target == NULL)
	{
		throw std::invalid_argument("Pin is NULL");
	}

	if (target->m_parentGate == this->m_parentGate)
	{
		throw std::invalid_argument("Cannot connect to self");
	}

	bool insideInside = (m_parentGate->GetParent() == target->m_parentGate->GetParent());
	bool insideToParent = (m_parentGate->GetParent() == target->m_parentGate);
	bool parentToInside = (m_parentGate == target->m_parentGate->GetParent());

	if (insideInside)
	{
		if (m_direction != IOPin::OUTPUT)
		{
			throw std::invalid_argument("Cannot connect from INPUT pin");
		}

		if (target->m_direction != IOPin::INPUT)
		{
			throw std::invalid_argument("Cannot connect to OUTPUT pin");
		}
	}
	else if (insideToParent)
	{
		if (m_direction != IOPin::OUTPUT)
		{
			throw std::invalid_argument("Cannot connect from INPUT pin");
		}

		if (target->m_direction != IOPin::OUTPUT)
		{
			throw std::invalid_argument("Must connect to parent OUTPUT pin");
		}
	} 
	else if (parentToInside)
	{
		if (m_direction != IOPin::INPUT)
		{
			throw std::invalid_argument("Must connect from parent from INPUT pin");
		}

		if (target->m_direction != IOPin::INPUT)
		{
			throw std::invalid_argument("Must connect to parent INPUT pin");
		}
	}
	else
	{
		throw std::invalid_argument("Unsupported connection type");
	}

	// TODO:Hi-Z
	if (m_connectedPins.find(target) != m_connectedPins.end())
	{
		throw std::invalid_argument("Already connected to target pin");
	}

	m_connectedPins.insert(target);
}
