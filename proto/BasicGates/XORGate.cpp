#include "stdafx.h"
#include "XORGate.h"

XORGate::XORGate()
{
	AddInput("in1");
	AddInput("in2");
	AddOutput("out");
}

GateBase * XORGate::Clone(const char * name)
{
	return new XORGate();
}

void XORGate::ComputeState()
{
	if ((GetPin("in1")->Get() == IOPin::LOW && GetPin("in2")->Get() == IOPin::HI) || 
		(GetPin("in1")->Get() == IOPin::HI && GetPin("in2")->Get() == IOPin::LOW))
	{
		GetPin("out")->Set(IOPin::HI);
	}
	else
	{
		GetPin("out")->Set(IOPin::LOW);
	}

}
