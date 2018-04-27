#pragma once

#include "GateBase.h"

class ANDGate : public GateBase
{
public:
	ANDGate(size_t inputs = 2);
	virtual ~ANDGate() {}

	virtual GateBase* Clone(const char* name);

	virtual void ComputeState();

};

