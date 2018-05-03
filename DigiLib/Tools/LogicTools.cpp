#include "stdafx.h"
#include "LogicTools.h"
#include "Core\IOPin.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"

namespace DigiLib {
	namespace Tools {
		using namespace DigiLib::Core;

		void LogicTools::PrintTruthTable(std::ostream& os, size_t level, std::vector<IOPinPtr> const& inputs, std::vector<IOPinPtr> const& outputs)
		{
			if (level <= inputs.size() - 1)
			{
				inputs[level]->Set(IOState::LOW);
				PrintTruthTable(os, level + 1, inputs, outputs);

				inputs[level]->Set(IOState::HI);
				PrintTruthTable(os, level + 1, inputs, outputs);
			}
			else
			{
				for (auto pin : inputs)
				{
					os << pin->Get().ToString() << "    ";
				}
				for (auto pin : outputs)
				{
					os << pin->Get().ToString() << "    ";
				}
				os << std::endl;
			}
		}

		std::string LogicTools::PrintTruthTable(std::vector<IOPinPtr> const & inputs, std::vector<IOPinPtr> const & outputs)
		{
			std::ostringstream os;
			if (inputs.size() == 0 || outputs.size() == 0)
			{
				return "";
			}

			for (auto pin : inputs)
			{
				os << pin->GetName() << "    ";
			}

			for (auto pin : outputs)
			{
				os << pin->GetName() << "    ";
			}
			os << std::endl;

			PrintTruthTable(os, 0, inputs, outputs);
			os << std::endl;

			return os.str();
		}

		std::string LogicTools::PrintInternalConnections(GatePtr gate)
		{
			std::ostringstream os;
			for (auto & input : gate->GetInputPins())
			{
				for (auto connection : gate->GetConnectedToPins(input.second))
				{
					os << connection.GetSource()->GetFullName() << " -> " << connection.GetTarget()->GetFullName() << std::endl;
				}
			}

			for (auto & output : gate->GetOutputPins())
			{
				for (auto connection : gate->GetConnectedToPins(output.second))
				{
					os << connection.GetSource()->GetFullName() << " -> " << connection.GetTarget()->GetFullName() << std::endl;
				}
			}

			CompositeGatePtr composite = std::dynamic_pointer_cast<CompositeGate>(gate);
			if (composite)
			{
				for (auto & subGate : composite->GetInternalGates())
				{
					os << PrintInternalConnections(subGate.second);
				}
			}

			return os.str();
		}

		void LogicTools::PrintPinInfo(std::ostream& os, GatePtr gate, const IOPinNameToIDMapType& pins)
		{
			const char* prefix = "";
			for (auto & pinId : pins)
			{
				IOPinPtr pin = gate->GetPin(pinId.second);
				os << prefix << " - " << pin->GetName();
				if (pin->GetWidth() > 1)
				{
					os << "[" << pin->GetWidth() << "]";
				}
				prefix = "\n";
			}
			os << std::endl;
		}

		std::string LogicTools::PrintPinInfo(GatePtr gate)
		{
			std::ostringstream os;
			os << "Input pins:" << std::endl;
			PrintPinInfo(os, gate, gate->GetInputPins());

			os << "Output pins:" << std::endl;
			PrintPinInfo(os, gate, gate->GetOutputPins());

			return os.str();
		}

		std::string LogicTools::PrintTruthTable(GatePtr gate)
		{
			std::ostringstream os;
			std::vector<IOPinPtr> inputsVect;
			std::vector<IOPinPtr> outputsVect;

			for (auto & pinId : gate->GetInputPins())
			{
				IOPinPtr pin = gate->GetPin(pinId.second);
				inputsVect.push_back(pin);
				os << pin->GetName() << "    ";
			}

			for (auto & pinId : gate->GetOutputPins())
			{
				IOPinPtr pin = gate->GetPin(pinId.second);
				outputsVect.push_back(pin);
				os << pin->GetName() << "    ";
			}
			os << std::endl;

			PrintTruthTable(os, 0, inputsVect, outputsVect);
			os << std::endl;

			return os.str();
		}

		void LogicTools::GetTruthTable(size_t level, std::vector<IOPinPtr> const& inputs, GatePtr gate, const IOPinNameToIDMapType& outputs, LogicTools::ResultListType& result)
		{
			if (level <= inputs.size() - 1)
			{
				inputs[level]->Set(IOState::LOW);
				GetTruthTable(level + 1, inputs, gate, outputs, result);

				inputs[level]->Set(IOState::HI);
				GetTruthTable(level + 1, inputs, gate, outputs, result);
			}
			else
			{
				for (auto & pinID : outputs)
				{
					IOPinPtr pin = gate->GetPin(pinID.second);
					result.push_back(pin->Get().Get());
				}
			}
		}

		LogicTools::ResultListType LogicTools::GetTruthTable(GatePtr gate)
		{
			const IOPinNameToIDMapType & inputs = gate->GetInputPins();
			const IOPinNameToIDMapType & outputs = gate->GetOutputPins();

			LogicTools::ResultListType results;
			std::vector<IOPinPtr> inputsVect;

			for (auto& pinId : inputs)
			{
				IOPinPtr pin = gate->GetPin(pinId.second);
				inputsVect.push_back(pin);
			}

			GetTruthTable(0, inputsVect, gate, outputs, results);

			return results;
		}
	}
}