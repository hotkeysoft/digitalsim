#include "stdafx.h"
#include "ANDGate.h"
#include <sstream>

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		ANDGate::ANDGate(size_t inputs/* = 2 */) noexcept : GateBase("and")
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

		GateBase * ANDGate::Clone(const char * name)
		{
			return new ANDGate(this->GetInputCount());
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
	}
}