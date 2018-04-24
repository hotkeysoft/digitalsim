#pragma once

#include "GateBase.h"

class NANDGate : public GateBase
{
public:
	NANDGate();
	virtual ~NANDGate();

	virtual void ComputeState();

};


