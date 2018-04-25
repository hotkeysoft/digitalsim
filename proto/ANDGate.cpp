#include "stdafx.h"
#include "ANDGate.h"
#include <sstream>

ANDGate::ANDGate(int inputs/* = 2 */)
{
	assert(inputs > 1);
	for (int i = 1; i <= inputs; ++i)
	{
		std::ostringstream ss;
		ss << "in" << i;
		AddInput(ss.str().c_str());
	}

	AddOutput("out");
}


ANDGate::~ANDGate()
{
}

void ANDGate::ComputeState()
{
	for (auto pin : m_inputPins)
	{
		if (pin.second->Get() == IOPin::LOW)
		{
			GetPin("out")->Set(IOPin::LOW);
			return;
		}
	}

	GetPin("out")->Set(IOPin::HI);
}
