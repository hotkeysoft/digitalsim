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
				for (auto connection : gate->GetConnectedToPins(input.second.get()))
				{
					os << connection.GetSource()->GetFullName() << " -> " << connection.GetTarget()->GetFullName() << std::endl;
				}
			}

			for (auto & output : gate->GetOutputPins())
			{
				for (auto connection : gate->GetConnectedToPins(output.second.get()))
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

		void LogicTools::PrintPinInfo(std::ostream& os, const Core::IOPinMapType& pins)
		{
			const char* prefix = "";
			for (auto & pin : pins)
			{
				os << prefix << " - " << pin.second->GetName();
				if (pin.second->GetWidth() > 1)
				{
					os << "[" << pin.second->GetWidth() << "]";
				}
				prefix = "\n";
			}
			os << std::endl;
		}

		std::string LogicTools::PrintPinInfo(Core::GateBase * gate)
		{
			std::ostringstream os;
			os << "Input pins:" << std::endl;
			PrintPinInfo(os, gate->GetInputPins());

			os << "Output pins:" << std::endl;
			PrintPinInfo(os, gate->GetOutputPins());

			return os.str();
		}

		std::string LogicTools::PrintTruthTable(GateBase* gate)
		{
			std::ostringstream os;
			std::vector<IOPin*> inputsVect;
			std::vector<IOPin*> outputsVect;

			for (auto & pin : gate->GetInputPins())
			{
				inputsVect.push_back(pin.second.get());
				os << pin.first << "    ";
			}

			for (auto & pin : gate->GetOutputPins())
			{
				outputsVect.push_back(pin.second.get());
				os << pin.first << "    ";
			}
			os << std::endl;

			PrintTruthTable(os, 0, inputsVect, outputsVect);
			os << std::endl;

			return os.str();
		}

		void LogicTools::GetTruthTable(size_t level, std::vector<IOPin*> const& inputs, const IOPinMapType& outputs, LogicTools::ResultListType& result)
		{
			if (level <= inputs.size() - 1)
			{
				inputs[level]->Set(IOState::LOW);
				GetTruthTable(level + 1, inputs, outputs, result);

				inputs[level]->Set(IOState::HI);
				GetTruthTable(level + 1, inputs, outputs, result);
			}
			else
			{
				for (auto & pin : outputs)
				{
					// TODO: Won't work for bus
					result.push_back(pin.second.get()->Get().Get());
				}
			}
		}

		LogicTools::ResultListType LogicTools::GetTruthTable(GateBase * gate)
		{
			const IOPinMapType & inputs = gate->GetInputPins();
			const IOPinMapType & outputs = gate->GetOutputPins();

			LogicTools::ResultListType results;
			std::vector<IOPin*> inputsVect;

			for (auto& pin : inputs)
			{
				inputsVect.push_back(pin.second.get());
			}

			GetTruthTable(0, inputsVect, outputs, results);

			return results;
		}
	}
}