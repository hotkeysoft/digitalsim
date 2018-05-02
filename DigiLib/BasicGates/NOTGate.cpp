#include "stdafx.h"
#include "NOTGate.h"

namespace DigiLib {
	namespace BasicGates {

		using namespace DigiLib::Core;

		NOTGate::NOTGate() noexcept : GateBase("not")
		{
			m_in = AddInput("in");
			m_out = AddOutput("out");
		}

		GateBase * NOTGate::Clone(const char * name)
		{
			return new NOTGate();
		}

		void NOTGate::ComputeState()
		{
			if (m_in->Get() == IOState::HI)
			{
				m_out->Set(IOState::LOW);
			}
			else
			{
				m_out->Set(IOState::HI);
			}
		}
	}
}