#pragma once

#include "GateBase.h"

class NOTGate : public GateBase
{
public:
	NOTGate();
	virtual ~NOTGate() {}

	virtual GateBase* Clone(const char* name);

	virtual void ComputeState();
};


