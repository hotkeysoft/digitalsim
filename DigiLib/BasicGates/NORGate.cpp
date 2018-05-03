#include "stdafx.h"
#include "NORGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		NORGate::NORGate(size_t inputs) noexcept : GateBase("or"), m_inputs(inputs)
		{
			assert(inputs > 1);
		}

		void NORGate::Init()
		{
			for (size_t i = 1; i <= m_inputs; ++i)
			{
				std::ostringstream ss;
				ss << "in" << i;
				m_inputPins.push_back(AddInput(ss.str().c_str()));
			}

			m_out = AddOutput("out");
		}

		Core::GatePtr NORGate::Create(size_t inputs)
		{
			auto ptr = std::make_shared<shared_enabler>(inputs);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}

		Core::GatePtr NORGate::Clone(const char * name)
		{
			auto ptr = std::make_shared<shared_enabler>(this->m_inputs);
			GatePtr gate = std::static_pointer_cast<GateBase>(ptr);
			gate->Init();
			return gate;
		}
		
		void NORGate::ComputeState()
		{
			for (auto & pin : m_inputPins)
			{
				if (pin->Get() == IOState::HI)
				{
					m_out->Set(IOState::LOW);
					return;
				}
			}

			m_out->Set(IOState::HI);
		}

		struct NORGate::shared_enabler : public NORGate
		{
			template <typename... Args>
			shared_enabler(Args &&... args)
				: NORGate(std::forward<Args>(args)...)
			{
			}
		};
	}
}