#include "stdafx.h"

#include "GateBase.h"

GateBase::GateBase()
{
}

GateBase::~GateBase()
{
}

void GateBase::AddInput(std::string name, int8_t width)
{
	assert(width == 1);
	IOPin* pin = new IOPin(this, name, IOPin::IO_DIRECTION::INPUT);

	m_inputPins[name] = pin;
}

void GateBase::AddOutput(std::string name, int8_t width)
{
	assert(width == 1);
	IOPin* pin = new IOPin(this, name, IOPin::IO_DIRECTION::OUTPUT);
	
	m_outputPins[name] = pin;
}

IOPin* GateBase::GetPin(std::string name)
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
