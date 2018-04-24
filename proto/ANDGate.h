#pragma once

#include "GateBase.h"

class ANDGate : public GateBase
{
public:
	ANDGate(int inputs = 2);
	virtual ~ANDGate();

	virtual void ComputeState();

};

