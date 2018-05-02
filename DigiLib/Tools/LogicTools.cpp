#include "stdafx.h"
#include "LogicTools.h"
#include "Core\IOPin.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"

namespace DigiLib {
	namespace Tools {
		using namespace DigiLib::Core;

		void LogicTools::PrintTruthTable(std::ostream& os, size_t level, std::vector<IOPin*> const& inputs, std::vector<IOPin*> const& outputs)
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

		std::string LogicTools::PrintTruthTable(std::vector<IOPin*> const & inputs, std::vector<IOPin*> const & outputs)
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

		std::string LogicTools::PrintInternalConnections(GateBase * gate)
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

			CompositeGate* composite = dynamic_cast<CompositeGate*>(gate);
			if (composite)
			{
				for (auto subGate : composite->GetInternalGates())
				{
					os << PrintInternalConnections(subGate.second);
				}
			}

			return os.str();
		}

		void LogicTools::PrintPinInfo(std::ostream& os, Core::GateBase * gate, const Core::IOPinNameToIDMapType& pins)
		{
			const char* prefix = "";
			for (auto & pinId : pins)
			{
				IOPin * pin = gate->GetPin(pinId.second);
				os << prefix << " - " << pin->GetName();
				if (pin->GetWidth() > 1)
				{
					os << "[" << pin->GetWidth() << "]";
				}
				prefix = "\n";
			}
			os << std::endl;
		}

		std::string LogicTools::PrintPinInfo(Core::GateBase * gate)
		{
			std::ostringstream os;
			os << "Input pins:" << std::endl;
			PrintPinInfo(os, gate, gate->GetInputPins());

			os << "Output pins:" << std::endl;
			PrintPinInfo(os, gate, gate->GetOutputPins());

			return os.str();
		}

		std::string LogicTools::PrintTruthTable(GateBase* gate)
		{
			std::ostringstream os;
			std::vector<IOPin*> inputsVect;
			std::vector<IOPin*> outputsVect;

			for (auto & pinId : gate->GetInputPins())
			{
				IOPin* pin = gate->GetPin(pinId.second);
				inputsVect.push_back(pin);
				os << pin->GetName() << "    ";
			}

			for (auto & pinId : gate->GetOutputPins())
			{
				IOPin* pin = gate->GetPin(pinId.second);
				outputsVect.push_back(pin);
				os << pin->GetName() << "    ";
			}
			os << std::endl;

			PrintTruthTable(os, 0, inputsVect, outputsVect);
			os << std::endl;

			return os.str();
		}

		void LogicTools::GetTruthTable(size_t level, std::vector<IOPin*> const& inputs, GateBase * gate, const IOPinNameToIDMapType& outputs, LogicTools::ResultListType& result)
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
					IOPin * pin = gate->GetPin(pinID.second);
					result.push_back(pin->Get().Get());
				}
			}
		}

		LogicTools::ResultListType LogicTools::GetTruthTable(GateBase * gate)
		{
			const IOPinNameToIDMapType & inputs = gate->GetInputPins();
			const IOPinNameToIDMapType & outputs = gate->GetOutputPins();

			LogicTools::ResultListType results;
			std::vector<IOPin*> inputsVect;

			for (auto& pinId : inputs)
			{
				IOPin * pin = gate->GetPin(pinId.second);
				inputsVect.push_back(pin);
			}

			GetTruthTable(0, inputsVect, gate, outputs, results);

			return results;
		}
	}
}