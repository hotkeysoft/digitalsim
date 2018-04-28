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
			if (GetPin("in")->Get() == IOPin::HI)
			{
				GetPin("out")->Set(IOPin::LOW);
			}
			else
			{
				GetPin("out")->Set(IOPin::HI);
			}

		}
	}
}