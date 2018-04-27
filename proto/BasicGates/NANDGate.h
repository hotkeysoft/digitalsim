#pragma once

#include "GateBase.h"

class NANDGate : public GateBase
{
public:
	NANDGate(size_t inputs = 2);
	virtual ~NANDGate() {}

	virtual GateBase* Clone(const char* name);

	virtual void ComputeState();

};


