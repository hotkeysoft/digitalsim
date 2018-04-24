#pragma once

#include "GateBase.h"

class NOTGate : public GateBase
{
public:
	NOTGate();
	virtual ~NOTGate();

	virtual void ComputeState();

};


