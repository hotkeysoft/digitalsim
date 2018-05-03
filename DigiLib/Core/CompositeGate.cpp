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
		
		CompositeGatePtr CompositeGate::Create(const char* name)
		{
			auto ptr = std::make_shared<shared_enabler>(name);
			CompositeGatePtr gate = std::static_pointer_cast<CompositeGate>(ptr);
			gate->Init();
			return gate;
		}

		GatePtr CompositeGate::Clone(const char * name)
		{
			ValidateGateName(name, false);
			auto ptr = std::make_shared<shared_enabler>(name);
			CompositeGatePtr clone = std::static_pointer_cast<CompositeGate>(ptr);
			clone->Init();
			CompositeGatePtr source = thisCompositeGate();
			InternalCloneInputs(source, clone);
			InternalCloneOutputs(source, clone);
			InternalCloneGates(source, clone);
			InternalCloneLinks(source, clone);

			return clone;
		}

		void CompositeGate::AddGate(const char* name, GatePtr gate)
		{
			ValidateGateName(name);
			if (gate == NULL)
			{
				throw std::invalid_argument("Cannot add null gate");
			}
			else if (gate.get() == this)
			{
				throw std::invalid_argument("Cannot add self");
			}
			
			gate->SetParent(shared_from_this());
			gate->SetName(name);

			m_internalGates[name] = gate;
		}

		GatePtr CompositeGate::GetGate(const char* name)
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

		void CompositeGate::InternalCloneInputs(CompositeGatePtr source, CompositeGatePtr clone)
		{
			// Clone input pins
			for (auto & inputs : source->m_inputPinsNames)
			{
				IOPinPtr pin = source->m_ioPins[inputs.second];

				clone->AddInput(pin->GetRawName().c_str(), pin->GetWidth());
			}
		}

		void CompositeGate::InternalCloneOutputs(CompositeGatePtr source, CompositeGatePtr clone)
		{
			for (auto & outputs: source->m_outputPinsNames)
			{
				IOPinPtr pin = source->m_ioPins[outputs.second];
				clone->AddOutput(pin->GetRawName().c_str(), pin->GetWidth(), pin->GetDirection());
			}
		}

		void CompositeGate::InternalCloneGates(CompositeGatePtr source, CompositeGatePtr clone)
		{
			for (auto & gate : source->m_internalGates)
			{
				const char* gateName = gate.first.c_str();
				CompositeGatePtr innerSource = std::dynamic_pointer_cast<CompositeGate>(gate.second);
				if (innerSource)
				{
					CompositeGatePtr innerClone = CompositeGate::Create(gateName);
					InternalCloneGates(innerSource, innerClone);
					InternalCloneInputs(innerSource, innerClone);
					InternalCloneOutputs(innerSource, innerClone);
					clone->AddGate(gateName, innerClone);
				}
				else
				{
					GatePtr innerClone = gate.second->Clone(gateName);
					clone->AddGate(gateName, innerClone);
				}
			}
		}

		void CompositeGate::InternalCloneLinks(GatePtr source, GatePtr clone)
		{
			CompositeGatePtr compositeSource = std::dynamic_pointer_cast<CompositeGate>(source);
			CompositeGatePtr compositeClone = std::dynamic_pointer_cast<CompositeGate>(clone);
			if (compositeSource)
			{
				for (auto & gate : compositeSource->m_internalGates)
				{
					InternalCloneLinks(gate.second, compositeClone->GetGate(gate.first.c_str()));
				}
			}

			InternalCloneInnerLinks(source, clone);
		}

		void CompositeGate::InternalCloneInnerLinks(GatePtr source, GatePtr clone)
		{
			CompositeGatePtr compositeClone = std::dynamic_pointer_cast<CompositeGate>(clone);

			for (auto connections : source->GetConnectedToPins())
			{
				for (auto links : connections)
				{
					IOPinPtr clonedSource = nullptr;
					IOPinPtr clonedTarget = nullptr;
					
					IOPinPtr sourcePin = links.GetSource();
					IOPinPtr targetPin = links.GetTarget();

					clonedSource = sourcePin->Clone(clone);

					if (targetPin->GetParent() == source->GetParent())
					{
						// Target on same level as source pin
						clonedTarget = targetPin->Clone(clone->GetParent());
					}
					else if (targetPin->GetParent()->GetParent() == source->GetParent())
					{
						// Target is outside component
						std::string gateName = targetPin->GetParent()->GetName();
						CompositeGatePtr comp = std::dynamic_pointer_cast<CompositeGate>(clone->GetParent());
						clonedTarget = targetPin->Clone(comp->GetGate(gateName.c_str()));
					}
					else
					{
						// Target is an internal gate below
						std::string gateName = targetPin->GetParent()->GetName();
						clonedTarget = targetPin->Clone(compositeClone->GetGate(gateName.c_str()));
					}

					clone->ConnectPins(clonedSource, clonedTarget);
				}
			}
		}

		struct CompositeGate::shared_enabler : public CompositeGate
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: CompositeGate(std::forward<Args>(args)...)
			{
			}
		};
	}
}