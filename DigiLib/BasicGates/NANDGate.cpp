#include "stdafx.h"
#include "NANDGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		NANDGate::NANDGate(size_t inputs/* = 2 */) noexcept : GateBase("nand")
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

		GateBase * NANDGate::Clone(const char * name)
		{
			return new NANDGate(this->GetInputCount());
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
	}
}