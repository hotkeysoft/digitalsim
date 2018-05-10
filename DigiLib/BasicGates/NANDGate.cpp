#include "stdafx.h"
#include "Core\Simulator.h"
#include "NANDGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		NANDGate::NANDGate(size_t inputs, size_t delay) noexcept : GateBase("nand", delay), m_inputs(inputs)
		{
			assert(inputs > 1);
		}
		void NANDGate::Init()
		{
			GateBase::Init();
			for (size_t i = 1; i <= m_inputs; ++i)
			{
				std::ostringstream ss;
				ss << "in" << i;
				m_inputPins.push_back(AddInput(ss.str().c_str()));
			}

			m_out = AddOutput("out");
		}

		Core::GatePtr NANDGate::Create(size_t inputs, size_t delay)
		{
			auto ptr = std::make_shared<shared_enabler>(inputs, delay);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr NANDGate::Clone(const char * name, bool deep)
		{
			auto ptr = std::make_shared<shared_enabler>(this->m_inputs, this->m_delay);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void NANDGate::ComputeState()
		{
			IOState newState = IOState::LOW;
			for (auto & pin : m_inputPins)
			{
				if(pin->Get() == IOState::UNDEF)
				{
					newState = IOState::UNDEF;
					break;
				}
				if (pin->Get() == IOState::LOW)
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
				GetSimulator()->PostEventRelative({ newState, m_out }, m_delay);
			}
		}

		struct NANDGate::shared_enabler : public NANDGate
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: NANDGate(std::forward<Args>(args)...)
			{
			}
		};
	}
}