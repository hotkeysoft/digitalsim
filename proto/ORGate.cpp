#include "stdafx.h"
#include "ORGate.h"


ORGate::ORGate()
{
	AddInput("in1");
	AddInput("in2");
	AddOutput("out");
}


ORGate::~ORGate()
{
}

void ORGate::ComputeState()
{
	if (GetPin("in1")->Get() == IOPin::HI ||
		GetPin("in2")->Get() == IOPin::HI)
	{
		GetPin("out")->Set(IOPin::HI);
	}
	else
	{
		GetPin("out")->Set(IOPin::LOW);
	}

}
