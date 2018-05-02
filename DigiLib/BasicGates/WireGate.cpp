#include "stdafx.h"
#include "WireGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		WireGate::WireGate() noexcept : GateBase("wire")
		{
			m_in = AddInput("in");
			m_out = AddOutput("out");
		}

		GateBase * WireGate::Clone(const char * name)
		{
			return new WireGate();
		}

		void WireGate::ComputeState()
		{
			m_out->Set(m_in->Get());
		}
	}
}