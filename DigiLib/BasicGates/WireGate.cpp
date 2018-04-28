#include "stdafx.h"
#include "WireGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		WireGate::WireGate() noexcept : GateBase("wire")
		{
			AddInput("in");
			AddOutput("out");
		}

		GateBase * WireGate::Clone(const char * name)
		{
			return new WireGate();
		}

		void WireGate::ComputeState()
		{
			GetPin("out")->Set(GetPin("in")->Get());
		}
	}
}