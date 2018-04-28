#include "stdafx.h"

#include "GateBase.h"
#include <regex>

AllowedConnectionMapType GateBase::m_insideInsideMap;
AllowedConnectionMapType GateBase::m_insideParentMap;
AllowedConnectionMapType GateBase::m_parentInsideMap;

GateBase::GateBase() : m_parent(NULL)
{
}

GateBase::GateBase(const char* name) : m_name(name), m_parent(NULL)
{
	ValidateGateName(name);
}

GateBase::~GateBase()
{
}

void GateBase::SetName(const char *name)
{
	ValidateGateName(name);
	this->m_name = name;
}

std::string GateBase::GetFullName()
{
	std::ostringstream os;
	if (m_parent)
	{
		os << m_parent->GetFullName() << ".";
	}
	os << m_name;

	return os.str();
}

IOPin* GateBase::AddInput(const char* name, int8_t width)
{
	ValidatePinName(name);
	ValidatePinWidth(width);
	
	IOPin* pin = new IOPin(this, name, IOPin::IO_DIRECTION::INPUT);

	m_inputPins[name] = pin;
	return pin;
}

IOPin* GateBase::AddOutput(const char* name, int8_t width, IOPin::IO_DIRECTION dir)
{
	ValidatePinName(name);
	ValidatePinWidth(width);

	IOPin* pin = nullptr;
	switch (dir)
	{
	case IOPin::IO_DIRECTION::OUTPUT:
	case IOPin::IO_DIRECTION::OUTPUT_HI_Z:
		pin = new IOPin(this, name, dir);
		break;
	default:
		throw std::invalid_argument("bad output direction");
	}

	m_outputPins[name] = pin;
	return pin;
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

PinConnectionsType& GateBase::GetConnectedPins(const char * source)
{
	return GetConnectedPins(GetPin(source));
}

PinConnectionsType& GateBase::GetConnectedPins(IOPin * source)
{
	if (source == nullptr)
	{
		throw std::invalid_argument("source pin not set");
	}

	return m_connectedPins[source];
}

void GateBase::SetParent(GateBase * parent)
{
	if (m_parent != NULL)
	{
		throw std::invalid_argument("gate already has a parent");
	}

	m_parent = parent;
}

void GateBase::ConnectPins(IOPin * source, IOPin * target)
{
	if (source == NULL && target == NULL)
	{
		throw std::invalid_argument("source or target is null");
	}

	//TODO validate source

	if (target == NULL)
	{
		throw std::invalid_argument("Pin is NULL");
	}

	if (target->GetParent() == this)
	{
		throw std::invalid_argument("Cannot connect to self");
	}

	bool insideInside = (GetParent() == target->GetParent()->GetParent());
	bool insideToParent = (GetParent() == target->GetParent());
	bool parentToInside = (this == target->GetParent()->GetParent());

	InitAllowedConnectionMaps();
	if ((insideInside && !m_insideInsideMap[source->GetDirection()][target->GetDirection()]) ||
		(insideToParent && !m_insideParentMap[source->GetDirection()][target->GetDirection()]) ||
		(parentToInside && !m_parentInsideMap[source->GetDirection()][target->GetDirection()]) ||
		(!insideInside && !insideToParent && !parentToInside))
	{
		throw std::invalid_argument("Not allowed by connection rules");
	}

	// TODO:Hi-Z
	IOConnection connection(source, target);
	auto& connectedPins = m_connectedPins[source];
	auto found = connectedPins.find(connection);
	if (found != connectedPins.end())
	{
		throw std::invalid_argument("Connection already exists");
	}

	m_connectedPins[source].insert(connection);
}

void GateBase::InitAllowedConnectionMaps()
{
	if (m_insideInsideMap.empty())
	{
		m_insideInsideMap[IOPin::INPUT]			[IOPin::INPUT]			= false;
		m_insideInsideMap[IOPin::INPUT]			[IOPin::OUTPUT]			= false;
		m_insideInsideMap[IOPin::INPUT]			[IOPin::OUTPUT_HI_Z]	= false;

		m_insideInsideMap[IOPin::OUTPUT]		[IOPin::INPUT]			= true;
		m_insideInsideMap[IOPin::OUTPUT]		[IOPin::OUTPUT]			= false;
		m_insideInsideMap[IOPin::OUTPUT]		[IOPin::OUTPUT_HI_Z]	= false;

		m_insideInsideMap[IOPin::OUTPUT_HI_Z]	[IOPin::INPUT]			= true;
		m_insideInsideMap[IOPin::OUTPUT_HI_Z]	[IOPin::OUTPUT]			= false;
		m_insideInsideMap[IOPin::OUTPUT_HI_Z]	[IOPin::OUTPUT_HI_Z]	= false;
	}

	if (m_parentInsideMap.empty())
	{
		m_parentInsideMap[IOPin::INPUT]			[IOPin::INPUT]			= true;
		m_parentInsideMap[IOPin::INPUT]			[IOPin::OUTPUT]			= false;
		m_parentInsideMap[IOPin::INPUT]			[IOPin::OUTPUT_HI_Z]	= false;

		m_parentInsideMap[IOPin::OUTPUT]		[IOPin::INPUT]			= false;
		m_parentInsideMap[IOPin::OUTPUT]		[IOPin::OUTPUT]			= false;
		m_parentInsideMap[IOPin::OUTPUT]		[IOPin::OUTPUT_HI_Z]	= false;

		m_parentInsideMap[IOPin::OUTPUT_HI_Z]	[IOPin::INPUT]			= false;
		m_parentInsideMap[IOPin::OUTPUT_HI_Z]	[IOPin::OUTPUT]			= false;
		m_parentInsideMap[IOPin::OUTPUT_HI_Z]	[IOPin::OUTPUT_HI_Z]	= false;
	}

	if (m_insideParentMap.empty())
	{
		m_insideParentMap[IOPin::INPUT]			[IOPin::INPUT]			= false;
		m_insideParentMap[IOPin::INPUT]			[IOPin::OUTPUT]			= false;
		m_insideParentMap[IOPin::INPUT]			[IOPin::OUTPUT_HI_Z]	= false;

		m_insideParentMap[IOPin::OUTPUT]		[IOPin::INPUT]			= false;
		m_insideParentMap[IOPin::OUTPUT]		[IOPin::OUTPUT]			= true;
		m_insideParentMap[IOPin::OUTPUT]		[IOPin::OUTPUT_HI_Z]	= true;

		m_insideParentMap[IOPin::OUTPUT_HI_Z]	[IOPin::INPUT]			= false;
		m_insideParentMap[IOPin::OUTPUT_HI_Z]	[IOPin::OUTPUT]			= true;
		m_insideParentMap[IOPin::OUTPUT_HI_Z]	[IOPin::OUTPUT_HI_Z]	= true;
	}

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

bool GateBase::IsValidGateName(const char* name)
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

void GateBase::ValidateGateName(const char * name)
{
	if (!IsValidPinName(name))
	{
		throw std::invalid_argument("invalid gate name");
	}
}
