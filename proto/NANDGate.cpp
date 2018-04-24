#include "stdafx.h"
#include "NANDGate.h"


NANDGate::NANDGate()
{
	AddInput("in1");
	AddInput("in2");
	AddOutput("out");
}


NANDGate::~NANDGate()
{
}

void NANDGate::ComputeState()
{
	if (GetPin("in1")->Get() == IOPin::HI &&
		GetPin("in2")->Get() == IOPin::HI)
	{
		GetPin("out")->Set(IOPin::LOW);
	}
	else
	{
		GetPin("out")->Set(IOPin::HI);
	}

}