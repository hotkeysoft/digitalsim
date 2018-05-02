#include "stdafx.h"
#include "ORGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		ORGate::ORGate(size_t inputs/* = 2 */) noexcept : GateBase("or")
		{
			assert(inputs > 1);
			for (size_t i = 1; i <= inputs; ++i)
			{
				std::ostringstream ss;
				ss << "in" << i;
				m_inputPins.push_back(AddInput(ss.str().c_str()));
			}

			m_out = AddOutput("out");
		}

		GateBase * ORGate::Clone(const char * name)
		{
			return new ORGate(this->GetInputCount());
		}

		void ORGate::ComputeState()
		{
			for (auto & pin : m_inputPins)
			{
				if (pin->Get() == IOState::HI)
				{
					m_out->Set(IOState::HI);
					return;
				}
			}

			m_out->Set(IOState::LOW);
		}
	}
}