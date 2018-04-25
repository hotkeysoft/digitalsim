#pragma once

#include "GateBase.h"

class ORGate : public GateBase
{
public:
	ORGate(int inputs = 2);
	virtual ~ORGate();

	virtual void ComputeState();

};

