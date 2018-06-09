#include "stdafx.h"
#include "CompositeGate.h"

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

		void CompositeGate::SetMode(Mode mode, SimulatorRef simulator)
		{
			GateBase::SetMode(mode, simulator);
			for (auto & gate : m_internalGates)
			{
				gate.second->SetMode(mode, simulator);
			}
		}
		
		CompositeGatePtr CompositeGate::Create(const char* name)
		{
			auto ptr = std::make_shared<shared_enabler>(name);
			CompositeGatePtr gate = std::static_pointer_cast<CompositeGate>(ptr);
			gate->Init();
			return gate;
		}

		GatePtr CompositeGate::Clone(const char * name, bool deep)
		{
			ValidateGateName(name, false);

			auto ptr = std::make_shared<shared_enabler>(name);
			CompositeGatePtr clone = std::static_pointer_cast<CompositeGate>(ptr);
			clone->Init();
			CompositeGatePtr source = thisCompositeGate();
			InternalCloneInputs(source, clone);
			InternalCloneOutputs(source, clone);
			InternalCloneGates(source, clone);
			if (deep)
			{
				InternalCloneLinks(source, clone);
			}

			return clone;
		}


		size_t CompositeGate::GetGateCount(bool recursive) noexcept
		{
			size_t count = m_internalGates.size();
			if (recursive)
			{
				// Should we include CompositeGate as one gate or 
				// count only Basic Gates...
				// Links between compositeGate and inner gate have 
				// a non-zero delay...

				for (auto & gate : m_internalGates)
				{
					count += gate.second->GetGateCount(true);
				}
			}

			return count;
		}

		void CompositeGate::Reset()
		{
			// TODO: Can this leak?
			m_internalGates.clear();
			GateBase::Reset();
		}

		void CompositeGate::ResetPins()
		{
			GateBase::ResetPins();
			for (auto & child : m_internalGates)
			{
				child.second->ResetPins();
			}
		}

		void CompositeGate::InitializeState()
		{
			GateBase::InitializeState();
			for (auto gate : m_internalGates)
			{
				gate.second->InitializeState();
			}
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
			
			gate->SetParent(this);
			gate->SetName(name);

			m_internalGates[name] = gate;
		}

		bool CompositeGate::ExtractGatePinName(const std::string in, std::string& gate, std::string& pin)
		{
			size_t dot = in.find_first_of('.');
			if (dot == 0)
			{
				return false;
			}
			if (dot == std::string::npos)
			{
				pin = in;
			}
			else
			{
				gate = in.substr(0, dot);
				pin = in.substr(dot + 1);

				if (pin.empty() || gate.empty())
					return false;
			}
			return true;
		}

		IOPinPtr CompositeGate::FindPin(const char * name)
		{
			if (name == nullptr)
			{
				throw std::invalid_argument("name cannot be null");
			}

			std::string gateName;
			std::string pinName;
			if (!ExtractGatePinName(name, gateName, pinName))
			{
				return nullptr;
			}

			if (gateName.empty())
			{
				return GateBase::FindPin(name);
			}

			GatePtr gate = GetGate(gateName.c_str());
			if (gate != nullptr)
			{
				return gate->FindPin(pinName.c_str());
			}

			return nullptr;
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

				IOPinPtr clonePin = clone->AddInput(pin->GetRawName().c_str(), pin->GetWidth());
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
				GatePtr innerClone = gate.second->Clone(gateName, false);
				clone->AddGate(gateName, innerClone);
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
			for (auto & connections : source->GetConnectedToPins())
			{
				InternalClonePinLinks(connections, source, clone);
			}
			InternalClonePinLinks(source->GetConnectedToPin("vcc"), source, clone);
			InternalClonePinLinks(source->GetConnectedToPin("gnd"), source, clone);
		}

		void CompositeGate::InternalClonePinLinks(PinConnectionsType connections, GatePtr source, GatePtr clone)
		{
			CompositeGatePtr compositeClone = std::dynamic_pointer_cast<CompositeGate>(clone);
			for (auto & link : connections)
			{
				IOPinPtr clonedSource = nullptr;
				IOPinPtr clonedTarget = nullptr;

				IOPinPtr sourcePin = link.GetSource();
				IOPinPtr targetPin = link.GetTarget();

				clonedSource = sourcePin->Clone(clone.get());

				if (targetPin->GetParent() == sourcePin->GetParent()) 
				{
					// Source and target are gate pins
					clonedTarget = targetPin->Clone(clone.get());
				}
				else if (targetPin->GetParent() == source->GetParent())
				{
					// Target on same level as source pin
					clonedTarget = targetPin->Clone(clone->GetParent());
				}
				else if (targetPin->GetParent()->GetParent() == source->GetParent())
				{
					// Target is outside component
					std::string gateName = targetPin->GetParent()->GetName();
					CompositeGateRef comp = dynamic_cast<CompositeGateRef>(clone->GetParent());
					clonedTarget = targetPin->Clone(comp->GetGate(gateName.c_str()).get());
				}
				else
				{
					// Target is an internal gate below
					std::string gateName = targetPin->GetParent()->GetName();
					clonedTarget = targetPin->Clone(compositeClone->GetGate(gateName.c_str()).get());
				}

				clone->ConnectPins(clonedSource, clonedTarget);
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