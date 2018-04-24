#include "stdafx.h"
#include "WireGate.h"


WireGate::WireGate()
{
	AddInput("in");
	AddOutput("out");
}


WireGate::~WireGate()
{
}

void WireGate::ComputeState()
{
	GetPin("out")->Set(GetPin("in")->Get());
}
