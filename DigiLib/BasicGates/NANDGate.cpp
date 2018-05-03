#include "stdafx.h"
#include "NANDGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		NANDGate::NANDGate(size_t inputs) noexcept : GateBase("nand"), m_inputs(inputs)
		{
			assert(inputs > 1);
		}
		void NANDGate::Init()
		{
			for (size_t i = 1; i <= m_inputs; ++i)
			{
				std::ostringstream ss;
				ss << "in" << i;
				m_inputPins.push_back(AddInput(ss.str().c_str()));
			}

			m_out = AddOutput("out");
		}

		Core::GatePtr NANDGate::Create(size_t inputs)
		{
			auto ptr = std::make_shared<shared_enabler>(inputs);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr NANDGate::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>(this->m_inputs);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void NANDGate::ComputeState()
		{
			for (auto & pin : m_inputPins)
			{
				if (pin->Get() == IOState::LOW)
				{
					m_out->Set(IOState::HI);
					return;
				}
			}

			m_out->Set(IOState::LOW);
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