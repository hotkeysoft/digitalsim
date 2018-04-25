#pragma once

#include "GateBase.h"

class XORGate : public GateBase
{
public:
	XORGate();
	virtual ~XORGate();

	virtual void ComputeState();

};
