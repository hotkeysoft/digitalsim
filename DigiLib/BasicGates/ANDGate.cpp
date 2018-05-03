#include "stdafx.h"
#include "ANDGate.h"
#include <sstream>

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;
		
		ANDGate::ANDGate(size_t inputs) noexcept : GateBase("and"), m_inputs(inputs)
		{
			assert(inputs > 1);
		}

		void ANDGate::Init()
		{
			for (size_t i = 1; i <= m_inputs; ++i)
			{
				std::ostringstream ss;
				ss << "in" << i;
				m_inputPins.push_back(AddInput(ss.str().c_str()));
			}

			m_out = AddOutput("out");
		}

		Core::GatePtr ANDGate::Create(size_t inputs)
		{
			auto ptr = std::make_shared<shared_enabler>(inputs);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr ANDGate::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>(this->m_inputs);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		void ANDGate::ComputeState()
		{
			for (auto & pin : m_inputPins)
			{
				if (pin->Get() == IOState::LOW)
				{
					m_out->Set(IOState::LOW);
					return;
				}
			}

			m_out->Set(IOState::HI);
		}

		struct ANDGate::shared_enabler : public ANDGate
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: ANDGate(std::forward<Args>(args)...)
			{
			}
		};
	}
}