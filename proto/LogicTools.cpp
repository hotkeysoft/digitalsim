#include "stdafx.h"
#include "LogicTools.h"
#include "IOPin.h"
#include "GateBase.h"

LogicTools::LogicTools()
{
}

LogicTools::~LogicTools()
{
}

void LogicTools::PrintTruthTable(size_t level, std::vector<IOPin*> const& inputs, std::vector<IOPin*> const& outputs)
{
	if (level <= inputs.size() - 1)
	{

		inputs[level]->Set(IOPin::LOW);
		PrintTruthTable(level + 1, inputs, outputs);

		inputs[level]->Set(IOPin::HI);
		PrintTruthTable(level + 1, inputs, outputs);
	}
	else
	{
		for (auto pin : inputs)
		{
			std::cout << pin->Get() << "\t";
		}
		for (auto pin : outputs)
		{
			std::cout << pin->Get() << "\t";
		}
		std::cout << std::endl;
	}
}

void LogicTools::PrintTruthTable(std::vector<IOPin*> const & inputs, std::vector<IOPin*> const & outputs)
{
	for (auto pin : inputs)
	{
		std::cout << pin->GetName() << "\t";
	}

	for (auto pin : outputs)
	{
		std::cout << pin->GetName() << "\t";
	}
	std::cout << std::endl;

	PrintTruthTable(0, inputs, outputs);
	std::cout << std::endl;
}

void LogicTools::PrintTruthTable(GateBase* gate)
{
	std::vector<IOPin*> inputsVect;
	std::vector<IOPin*> outputsVect;
		
	for (auto pin : gate->GetInputPins())
	{
		inputsVect.push_back(pin.second);
		std::cout << pin.first << "\t";
	}

	for (auto pin : gate->GetOutputPins())
	{
		outputsVect.push_back(pin.second);
		std::cout << pin.first << "\t";
	}
	std::cout << std::endl;

	PrintTruthTable(0, inputsVect, outputsVect);
	std::cout << std::endl;
}

void LogicTools::GetTruthTable(size_t level, std::vector<IOPin*> const& inputs, IOPinMapType& outputs, IOStateList& result)
{
	if (level <= inputs.size() - 1)
	{
		inputs[level]->Set(IOPin::LOW);
		GetTruthTable(level + 1, inputs, outputs, result);

		inputs[level]->Set(IOPin::HI);
		GetTruthTable(level + 1, inputs, outputs, result);
	}
	else
	{
		for (auto pin : outputs)
		{
			result.push_back(pin.second->Get());
		}
	}
}

LogicTools::IOStateList LogicTools::GetTruthTable(GateBase * gate)
{
	IOPinMapType inputs = gate->GetInputPins();
	IOPinMapType outputs = gate->GetOutputPins();
	IOStateList outputList;
	
	std::vector<IOPin*> inputsVect;

	for (auto pin : inputs)
	{
		inputsVect.push_back(pin.second);
	}
	
	GetTruthTable(0, inputsVect, outputs, outputList);

	return outputList;
}