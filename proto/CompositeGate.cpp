#include "stdafx.h"
#include "CompositeGate.h"
#include <regex>

CompositeGate::CompositeGate() : GateBase()
{
}

CompositeGate::CompositeGate(const char* name) : GateBase(name)
{
	ValidateGateName(name, false);
}

CompositeGate::~CompositeGate()
{
}

void CompositeGate::SetName(const char *name)
{
	ValidateGateName(name, false);
	this->m_name = name;
}

GateBase * CompositeGate::Clone(const char* name)
{
	ValidateGateName(name, false);
	CompositeGate* clone = new CompositeGate(name);

	InternalCloneInputs(this, clone);
	InternalCloneOutputs(this, clone);
	InternalCloneGates(this, clone);
	InternalCloneLinks(this, clone);

	return clone;
}

void CompositeGate::AddGate(const char* name, GateBase* gate)
{
	ValidateGateName(name);
	if (gate == NULL)
	{
		throw std::invalid_argument("Cannot add null gate");
	}
	else if (gate == this)
	{
		throw std::invalid_argument("Cannot add self");
	}

	gate->SetParent(this);
	gate->SetName(name);
	m_internalGates[name] = gate;
}

GateBase * CompositeGate::GetGate(const char* name)
{
	GateMapType::iterator it = m_internalGates.find(name);
	if (it != m_internalGates.end())
	{
		return it->second;
	}

	throw std::exception("Gate not found");
}

void CompositeGate::ConnectPins(IOPin * source, IOPin * target)
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

	if (insideInside)
	{
		if (source->GetDirection() != IOPin::OUTPUT)
		{
			throw std::invalid_argument("Cannot connect from INPUT pin");
		}

		if (target->GetDirection() != IOPin::INPUT)
		{
			throw std::invalid_argument("Cannot connect to OUTPUT pin");
		}
	}
	else if (insideToParent)
	{
		if (source->GetDirection() != IOPin::OUTPUT)
		{
			throw std::invalid_argument("Cannot connect from INPUT pin");
		}

		if (target->GetDirection() != IOPin::OUTPUT)
		{
			throw std::invalid_argument("Must connect to parent OUTPUT pin");
		}
	}
	else if (parentToInside)
	{
		if (source->GetDirection() != IOPin::INPUT)
		{
			throw std::invalid_argument("Must connect from parent from INPUT pin");
		}

		if (target->GetDirection() != IOPin::INPUT)
		{
			throw std::invalid_argument("Must connect to parent INPUT pin");
		}
	}
	else
	{
		throw std::invalid_argument("Unsupported connection type");
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

void CompositeGate::ValidateGateName(const char * name, bool checkDuplicate)
{
	if (!IsValidPinName(name))
	{
		throw std::invalid_argument("invalid gate name");
	}

	if (checkDuplicate && m_internalGates.find(name) != m_internalGates.end())
	{
		throw std::invalid_argument("duplicate gate name");
	}
}

void CompositeGate::InternalCloneInputs(CompositeGate* source, CompositeGate * clone)
{
	// Clone input pins
	for (auto pin : source->m_inputPins)
	{
		clone->AddInput(pin.second->GetName().c_str(), 1);
	}
}

void CompositeGate::InternalCloneOutputs(CompositeGate* source, CompositeGate * clone)
{
	for (auto pin : source->m_outputPins)
	{
		clone->AddOutput(pin.second->GetName().c_str(), 1);
	}
}

void CompositeGate::InternalCloneGates(CompositeGate* source, CompositeGate* clone)
{
	for (auto gate : source->m_internalGates)
	{
		const char* gateName = gate.first.c_str();
		CompositeGate* innerSource = dynamic_cast<CompositeGate*>(gate.second);
		if (innerSource)
		{
			CompositeGate* innerClone = new CompositeGate(gateName);
			InternalCloneGates(innerSource, innerClone);
			InternalCloneInputs(innerSource, innerClone);
			InternalCloneOutputs(innerSource, innerClone);
			clone->AddGate(gateName, innerClone);
		}
		else
		{
			GateBase* innerClone = gate.second->Clone(gateName);
			clone->AddGate(gateName, innerClone);
		}
	}
}

void CompositeGate::InternalCloneLinks(GateBase* source, GateBase* clone)
{
	CompositeGate* compositeSource = dynamic_cast<CompositeGate*>(source);
	CompositeGate* compositeClone = dynamic_cast<CompositeGate*>(clone);
	if (compositeSource)
	{
		for (auto gate : compositeSource->m_internalGates)
		{
			InternalCloneLinks(gate.second, compositeClone->GetGate(gate.first.c_str()));
		}
	}

	InternalCloneInnerLinks(source, clone);
}

void CompositeGate::InternalCloneInnerLinks(GateBase* source, GateBase* clone)
{
	CompositeGate* compositeClone = dynamic_cast<CompositeGate*>(clone);

	for (auto connections : source->GetConnectedPins())
	{
		IOPin* currentPin = connections.first;

		for (auto links : connections.second)
		{
			IOPin* clonedSource = nullptr;
			IOPin* clonedTarget = nullptr;

			IOPin* sourcePin = links.GetSource();
			IOPin* targetPin = links.GetTarget();

			if (sourcePin->GetParent() == source)
			{
				clonedSource = clone->GetPin(sourcePin->GetName().c_str());
			}
			else
			{
				std::string gateName = sourcePin->GetParent()->GetName();
				clonedSource = compositeClone->GetGate(gateName.c_str())->GetPin(sourcePin->GetName().c_str());
			}

			if (targetPin->GetParent() == source->GetParent())
			{
				clonedTarget = clone->GetParent()->GetPin(targetPin->GetName().c_str());
			}
			else if (targetPin->GetParent()->GetParent() == source->GetParent())
			{
				std::string gateName = targetPin->GetParent()->GetName();
				CompositeGate* comp = dynamic_cast<CompositeGate*>(clone->GetParent());
				clonedTarget = comp->GetGate(gateName.c_str())->GetPin(targetPin->GetName().c_str());
			}
			else
			{
				std::string gateName = targetPin->GetParent()->GetName();
				clonedTarget = compositeClone->GetGate(gateName.c_str())->GetPin(targetPin->GetName().c_str());
			}

			clone->ConnectPins(clonedSource, clonedTarget);
		}
	}
}
