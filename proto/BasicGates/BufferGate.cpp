#include "stdafx.h"
#include "BufferGate.h"
#include <sstream>

BufferGate::BufferGate()
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
	if (GetPin("en")->Get() == IOPin::HI)
	{
		GetPin("out")->Set(IOPin::HI_Z);
	}
	else
	{
		GetPin("out")->Set(GetPin("in")->Get());
	}
}
