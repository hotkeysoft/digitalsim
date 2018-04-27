#pragma once

#include <vector>
#include "IOPin.h"
#include "GateBase.h"

class LogicTools
{
public:
	LogicTools();
	~LogicTools();

	typedef std::vector<IOPin::IO_STATE> IOStateList;

	static IOStateList GetTruthTable(GateBase* gate);

	static void PrintTruthTable(GateBase* gate);
	static void PrintTruthTable(std::vector<IOPin*> const& inputs, std::vector<IOPin*> const& outputs);

	static void PrintInternalConnections(GateBase* gate);

private:
	static void PrintTruthTable(size_t level, std::vector<IOPin*> const& inputs, std::vector<IOPin*> const& outputs);
	static void GetTruthTable(size_t level, std::vector<IOPin*> const& inputs, IOPinMapType& outputs, IOStateList& result);
};

