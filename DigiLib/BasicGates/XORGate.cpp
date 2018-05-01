#include "stdafx.h"
#include "XORGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		XORGate::XORGate() noexcept : GateBase("xor")
		{
			AddInput("in1");
			AddInput("in2");
			AddOutput("out");
		}

		GateBase * XORGate::Clone(const char * name)
		{
			return new XORGate();
		}

		void XORGate::ComputeState()
		{
			if ((GetPin("in1")->Get() == IOState::LOW && GetPin("in2")->Get() == IOState::HI) ||
				(GetPin("in1")->Get() == IOState::HI && GetPin("in2")->Get() == IOState::LOW))
			{
				GetPin("out")->Set(IOState::HI);
			}
			else
			{
				GetPin("out")->Set(IOState::LOW);
			}

		}
	}
}