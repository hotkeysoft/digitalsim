#pragma once

#include "GateBase.h"

class WireGate : public GateBase
{
public:
	WireGate();
	virtual ~WireGate();

	virtual void ComputeState();

};

