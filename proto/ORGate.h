#pragma once

#include "GateBase.h"

class ORGate : public GateBase
{
public:
	ORGate();
	virtual ~ORGate();

	virtual void ComputeState();

};

