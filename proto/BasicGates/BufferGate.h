#pragma once

#include "GateBase.h"

class BufferGate : public GateBase
{
public:
	BufferGate();
	virtual ~BufferGate() {}

	virtual GateBase* Clone(const char* name);

	virtual void ComputeState();

};

