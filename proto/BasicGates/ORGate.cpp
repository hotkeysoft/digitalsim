#include "stdafx.h"
#include "ORGate.h"

ORGate::ORGate(int inputs/* = 2 */)
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

void ORGate::ComputeState()
{
	for (auto pin : m_inputPins)
	{
		if (pin.second->Get() == IOPin::HI)
		{
			GetPin("out")->Set(IOPin::HI);
			return;
		}
	}

	GetPin("out")->Set(IOPin::LOW);
}
