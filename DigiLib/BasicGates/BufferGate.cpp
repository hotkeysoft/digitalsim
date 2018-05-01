#include "stdafx.h"
#include "BufferGate.h"
#include <sstream>

namespace DigiLib {
	namespace BasicGates {
		using namespace DigiLib::Core;

		BufferGate::BufferGate(size_t width) : GateBase("buffer"), m_width(width)
		{
			AddInput("in", width);
			AddInput("en");
			AddOutput("out", width, IOPin::OUTPUT_HI_Z);
		}

		GateBase * BufferGate::Clone(const char * name)
		{
			return new BufferGate(this->m_width);
		}

		void BufferGate::ComputeState()
		{
			if (GetPin("en")->Get() == IOState::LOW)
			{
				IOState hiz(IOState::HI_Z, m_width);
				GetPin("out")->Set(hiz);
			}
			else
			{
				GetPin("out")->Set(GetPin("in")->Get());
			}
		}
	}
}