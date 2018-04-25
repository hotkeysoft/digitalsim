#include "stdafx.h"
#include "NANDGate.h"


NANDGate::NANDGate(int inputs/* = 2 */)
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


NANDGate::~NANDGate()
{
}

void NANDGate::ComputeState()
{
	for (auto pin : m_inputPins)
	{
		if (pin.second->Get() == IOPin::LOW)
		{
			GetPin("out")->Set(IOPin::HI);
			return;
		}
	}

	GetPin("out")->Set(IOPin::LOW);
}