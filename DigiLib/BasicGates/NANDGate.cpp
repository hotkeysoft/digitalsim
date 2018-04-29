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
				AddInput(ss.str().c_str());
			}

			AddOutput("out");
		}

		GateBase * NANDGate::Clone(const char * name)
		{
			return new NANDGate(this->GetInputCount());
		}

		void NANDGate::ComputeState()
		{
			for (auto & pin : m_inputPins)
			{
				if (pin.second->Get() == IOPin::LOW)
				{
					GetPin("out")->Set(IOPin::HI);
					return;
				}
			}

			GetPin("out")->Set(IOPin::LOW);
		}
	}
}