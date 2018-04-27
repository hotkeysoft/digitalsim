#pragma once

#include "GateBase.h"

class WireGate : public GateBase
{
public:
	WireGate();
	virtual ~WireGate() {}

	virtual GateBase* Clone(const char* name);

	virtual void ComputeState();
};

