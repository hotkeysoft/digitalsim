#include "stdafx.h"
#include "CompositeGate.h"
#include <regex>

namespace DigiLib
{
	namespace Core
	{
		CompositeGate::CompositeGate(const char* name) : GateBase(name)
		{
			ValidateGateName(name, false);
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
			if (!name)
			{
				throw std::invalid_argument("name is null");
			}

			GateMapType::iterator it = m_internalGates.find(name);
			if (it != m_internalGates.end())
			{
				return it->second;
			}

			return nullptr;
		}

		void CompositeGate::ValidateGateName(const char * name, bool checkDuplicate)
		{
			if (!IsValidGateName(name))
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
				const IOPin* currentPin = connections.first;

				for (auto links : connections.second)
				{
					IOPin* clonedSource = nullptr;
					IOPin* clonedTarget = nullptr;

					IOPin* sourcePin = links.GetSource();
					IOPin* targetPin = links.GetTarget();

					clonedSource = clone->GetPin(sourcePin->GetName().c_str());

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
	}
}