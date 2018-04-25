#pragma once

#include <vector>
#include <initializer_list>
#include "GateBase.h"
#include "IOPin.h"

class LogicTools
{
public:
	LogicTools();
	~LogicTools();

	typedef std::vector<IOPin::IO_STATE> IOStateList;

	static IOStateList GetTruthTable(GateBase* gate);

	static void PrintTruthTable(std::initializer_list<IOPin*> const& inputs, std::initializer_list<IOPin*> const& outputs);

private:
	static void PrintTruthTable(size_t level, std::vector<IOPin*> const& inputs, std::initializer_list<IOPin*> const& outputs);
	static void GetTruthTable(size_t level, std::vector<IOPin*> const& inputs, IOPinMapType& outputs, IOStateList& result);
};

