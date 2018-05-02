#include "stdafx.h"
#include "XORGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		XORGate::XORGate() noexcept : GateBase("xor")
		{
			m_in1 = AddInput("in1");
			m_in2 = AddInput("in2");
			m_out = AddOutput("out");
		}

		GateBase * XORGate::Clone(const char * name)
		{
			return new XORGate();
		}

		void XORGate::ComputeState()
		{
			if ((m_in1->Get() == IOState::LOW && m_in2->Get() == IOState::HI) ||
				(m_in1->Get() == IOState::HI && m_in2->Get() == IOState::LOW))
			{
				m_out->Set(IOState::HI);
			}
			else
			{
				m_out->Set(IOState::LOW);
			}

		}
	}
}