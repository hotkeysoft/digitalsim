#include "stdafx.h"
#include "BufferGate.h"
#include <sstream>

namespace DigiLib {
	namespace BasicGates {
		using namespace DigiLib::Core;

		BufferGate::BufferGate() noexcept : GateBase("buffer")
		{
			AddInput("in");
			AddInput("en");
			AddOutput("out", 1, IOPin::OUTPUT_HI_Z);
		}

		GateBase * BufferGate::Clone(const char * name)
		{
			return new BufferGate();
		}

		void BufferGate::ComputeState()
		{
			if (GetPin("en")->Get() == IOPin::LOW)
			{
				GetPin("out")->Set(IOPin::HI_Z);
			}
			else
			{
				GetPin("out")->Set(GetPin("in")->Get());
			}
		}
	}
}