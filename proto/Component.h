#pragma once

#include "GateBase.h"

class Component : public GateBase
{
public:
	Component();
	~Component();

	// Inherited via GateBase
	virtual void ComputeState() override;

};

