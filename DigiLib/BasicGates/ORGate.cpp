#include "stdafx.h"
#include "Core\Simulator.h"
#include "ORGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		ORGate::ORGate(size_t inputs) noexcept : GateBase("or"), m_inputs(inputs)
		{
			assert(inputs > 1);
		}

		void ORGate::Init()
		{
			for (size_t i = 1; i <= m_inputs; ++i)
			{
				std::ostringstream ss;
				ss << "in" << i;
				m_inputPins.push_back(AddInput(ss.str().c_str()));
			}

			m_out = AddOutput("out");
		}

		Core::GatePtr ORGate::Create(size_t inputs)
		{
			auto ptr = std::make_shared<shared_enabler>(inputs);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr ORGate::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>(this->m_inputs);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}
		
		void ORGate::ComputeState()
		{
			IOState newState = IOState::LOW;
			for (auto & pin : m_inputPins)
			{
				if (pin->Get() == IOState::UNDEF)
				{
					newState = IOState::UNDEF;
					break;
				}
				if (pin->Get() == IOState::HI)
				{
					newState = IOState::HI;
					break;
				}
			}

			if (GetMode() == ASYNC)
			{
				m_out->Set(newState);
			}
			else
			{
				GetSimulator()->PostEvent({ newState, m_out });
			}
		}

		struct ORGate::shared_enabler : public ORGate
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: ORGate(std::forward<Args>(args)...)
			{
			}
		};
	}
}