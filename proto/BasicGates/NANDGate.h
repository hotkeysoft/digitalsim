#pragma once

#include "GateBase.h"

class NANDGate : public GateBase
{
public:
	NANDGate(int inputs = 2);
	virtual ~NANDGate() {}

	virtual void ComputeState();

};


