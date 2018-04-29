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
				AddInput(ss.str().c_str());
			}

			AddOutput("out");
		}

		GateBase * ANDGate::Clone(const char * name)
		{
			return new ANDGate(this->GetInputCount());
		}

		void ANDGate::ComputeState()
		{
			for (auto & pin : m_inputPins)
			{
				if (pin.second->Get() == IOPin::LOW)
				{
					GetPin("out")->Set(IOPin::LOW);
					return;
				}
			}

			GetPin("out")->Set(IOPin::HI);
		}
	}
}