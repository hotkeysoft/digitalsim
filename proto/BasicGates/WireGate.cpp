#include "stdafx.h"
#include "WireGate.h"

WireGate::WireGate()
{
	AddInput("in");
	AddOutput("out");
}

void WireGate::ComputeState()
{
	GetPin("out")->Set(GetPin("in")->Get());
}
