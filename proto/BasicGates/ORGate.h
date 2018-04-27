#pragma once

#include "GateBase.h"

class ORGate : public GateBase
{
public:
	ORGate(size_t inputs = 2);
	virtual ~ORGate() {}

	virtual GateBase* Clone(const char* name);

	virtual void ComputeState();
};

