#include "stdafx.h"
#include "BufferGate.h"
#include <sstream>

namespace DigiLib {
	namespace BasicGates {
		using namespace DigiLib::Core;

		BufferGate::BufferGate(size_t width) : GateBase("buffer"), m_width(width)
		{
			m_in = AddInput("in", width);
			m_en = AddInput("en");
			m_out = AddOutput("out", width, IOPin::OUTPUT_HI_Z);
		}

		GateBase * BufferGate::Clone(const char * name)
		{
			return new BufferGate(this->m_width);
		}

		void BufferGate::ComputeState()
		{
			if (m_en->Get() == IOState::LOW)
			{
				IOState hiz(IOState::HI_Z, m_width);
				m_out->Set(hiz);
			}
			else
			{
				m_out->Set(m_in->Get());
			}
		}
	}
}