#include "stdafx.h"
#include "LogicTools.h"
#include "Core\IOPin.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"

namespace DigiLib {
	namespace Tools {
		using namespace DigiLib::Core;

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
					std::cout << pin->Get() << "    ";
				}
				for (auto pin : outputs)
				{
					std::cout << pin->Get() << "    ";
				}
				std::cout << std::endl;
			}
		}

		void LogicTools::PrintTruthTable(std::vector<IOPin*> const & inputs, std::vector<IOPin*> const & outputs)
		{
			if (inputs.size() == 0 || outputs.size() == 0)
			{
				return;
			}

			for (auto pin : inputs)
			{
				std::cout << pin->GetName() << "    ";
			}

			for (auto pin : outputs)
			{
				std::cout << pin->GetName() << "    ";
			}
			std::cout << std::endl;

			PrintTruthTable(0, inputs, outputs);
			std::cout << std::endl;
		}

		void LogicTools::PrintInternalConnections(GateBase * gate)
		{
			for (auto & input : gate->GetInputPins())
			{
				for (auto connection : gate->GetConnectedToPins(input.second.get()))
				{
					std::cout << connection.GetSource()->GetFullName() << " -> " << connection.GetTarget()->GetFullName() << std::endl;
				}
			}

			for (auto & output : gate->GetOutputPins())
			{
				for (auto connection : gate->GetConnectedToPins(output.second.get()))
				{
					std::cout << connection.GetSource()->GetFullName() << " -> " << connection.GetTarget()->GetFullName() << std::endl;
				}
			}

			CompositeGate* composite = dynamic_cast<CompositeGate*>(gate);
			if (composite)
			{
				for (auto subGate : composite->GetInternalGates())
				{
					PrintInternalConnections(subGate.second);
				}
			}
		}

		void LogicTools::PrintTruthTable(GateBase* gate)
		{
			std::vector<IOPin*> inputsVect;
			std::vector<IOPin*> outputsVect;

			for (auto & pin : gate->GetInputPins())
			{
				inputsVect.push_back(pin.second.get());
				std::cout << pin.first << "    ";
			}

			for (auto & pin : gate->GetOutputPins())
			{
				outputsVect.push_back(pin.second.get());
				std::cout << pin.first << "    ";
			}
			std::cout << std::endl;

			PrintTruthTable(0, inputsVect, outputsVect);
			std::cout << std::endl;
		}

		void LogicTools::GetTruthTable(size_t level, std::vector<IOPin*> const& inputs, const IOPinMapType& outputs, IOStateList& result)
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
				for (auto & pin : outputs)
				{
					result.push_back(pin.second.get()->Get());
				}
			}
		}

		LogicTools::IOStateList LogicTools::GetTruthTable(GateBase * gate)
		{
			const IOPinMapType & inputs = gate->GetInputPins();
			const IOPinMapType & outputs = gate->GetOutputPins();
			IOStateList outputList;

			std::vector<IOPin*> inputsVect;

			for (auto& pin : inputs)
			{
				inputsVect.push_back(pin.second.get());
			}

			GetTruthTable(0, inputsVect, outputs, outputList);

			return outputList;
		}
	}
}