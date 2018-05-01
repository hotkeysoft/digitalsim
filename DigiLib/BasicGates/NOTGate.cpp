#include "stdafx.h"
#include "NOTGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		NOTGate::NOTGate() noexcept : GateBase("not")
		{
			AddInput("in");
			AddOutput("out");
		}

		GateBase * NOTGate::Clone(const char * name)
		{
			return new NOTGate();
		}

		void NOTGate::ComputeState()
		{
			if (GetPin("in")->Get() == IOState::HI)
			{
				GetPin("out")->Set(IOState::LOW);
			}
			else
			{
				GetPin("out")->Set(IOState::HI);
			}

		}
	}
}