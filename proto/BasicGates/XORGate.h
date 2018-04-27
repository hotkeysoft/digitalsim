#pragma once

#include "GateBase.h"

class XORGate : public GateBase
{
public:
	XORGate();
	virtual ~XORGate() {}

	virtual GateBase* Clone(const char* name);

	virtual void ComputeState();
};
