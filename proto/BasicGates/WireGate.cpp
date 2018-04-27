#include "stdafx.h"
#include "WireGate.h"

WireGate::WireGate()
{
	AddInput("in");
	AddOutput("out");
}

GateBase * WireGate::Clone(const char * name)
{
	return new WireGate();
}

void WireGate::ComputeState()
{
	GetPin("out")->Set(GetPin("in")->Get());
}
