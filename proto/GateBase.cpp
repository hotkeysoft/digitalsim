#include "stdafx.h"

#include "GateBase.h"
#include <regex>

GateBase::GateBase() : m_parent(NULL)
{
}

GateBase::~GateBase()
{
}

void GateBase::AddInput(const char* name, int8_t width)
{
	ValidatePinName(name);
	ValidatePinWidth(width);
	
	IOPin* pin = new IOPin(this, name, IOPin::IO_DIRECTION::INPUT);

	m_inputPins[name] = pin;
}

void GateBase::AddOutput(const char* name, int8_t width)
{
	ValidatePinName(name);
	ValidatePinWidth(width);

	IOPin* pin = new IOPin(this, name, IOPin::IO_DIRECTION::OUTPUT);
	
	m_outputPins[name] = pin;
}

void GateBase::AddGate(const char* name, GateBase * gate)
{
	assert(m_internalGates.find(name) == m_internalGates.end());

	gate->SetParent(this);
	m_internalGates[name] = gate;
}


GateBase * GateBase::GetGate(const char* name)
{
	GateMapType::iterator it = m_internalGates.find(name);
	if (it != m_internalGates.end())
	{
		return it->second;
	}

	throw std::exception("Gate not found");
}

IOPin* GateBase::GetPin(const char* name)
{
	IOPinMapType::iterator it = m_inputPins.find(name);
	if (it != m_inputPins.end())
	{
		return it->second;
	}

	it = m_outputPins.find(name);
	if (it != m_outputPins.end())
	{
		return it->second;
	}

	throw std::exception("Pin not found");
}

void GateBase::SetParent(GateBase * parent)
{
	assert(m_parent == NULL);
	m_parent = parent;
}

bool GateBase::IsValidPinName(const char* name)
{
	if (name == NULL)
	{
		return false;
	}

	if (!std::regex_match(name, std::regex("^[A-Za-z](\\w){0,31}$")))
	{
		return false;
	}

	size_t len = strlen(name);
	if (len < 1 || len > 32)
	{ 
		return false;
	}

	return true;
}

void GateBase::ValidatePinName(const char * name)
{
	if (!IsValidPinName(name))
	{
		throw std::invalid_argument("invalid pin name");
	}

	if (m_inputPins.find(name) != m_inputPins.end() ||
		m_outputPins.find(name) != m_outputPins.end())
	{
		throw std::invalid_argument("duplicate pin name");
	}
}

void GateBase::ValidatePinWidth(int8_t width)
{
	if (width < 1)
	{
		throw std::out_of_range("width must be >=1");
	}
	else if (width > 1)
	{
		throw std::out_of_range("width >1 not supported yet");
	}
}
