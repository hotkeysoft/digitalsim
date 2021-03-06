#include "stdafx.h"
#include <iomanip>
#include "LogicTools.h"
#include "Core/IOPin.h"
#include "Core/GateBase.h"
#include "Core/CompositeGate.h"
#include "Core/Simulator.h"

namespace DigiLib {
	namespace Tools {
		using namespace DigiLib::Core;

		void LogicTools::PrintTruthTable(std::ostream& os, size_t level, const IOPinListType & inputs, const IOPinListType outputs)
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

		std::string LogicTools::PrintTruthTable(const IOPinListType & inputs, const IOPinListType & outputs)
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
				IOPinPtr pin = gate->GetPin(input.second);
				for (auto connection : gate->GetConnectedToPin(pin))
				{
					os << connection.GetSource()->GetFullName() << " -> " << connection.GetTarget()->GetFullName() << std::endl;
				}
			}

			for (auto & output : gate->GetOutputPins())
			{
				IOPinPtr pin = gate->GetPin(output.second);
				for (auto connection : gate->GetConnectedToPin(pin))
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

		std::string LogicTools::PrintInternalStates(Core::GatePtr gate, size_t maxLevel)
		{
			std::ostringstream os;

			for (auto & i : gate->GetInputPins())
			{
				IOPinPtr pin = gate->GetPin(i.second);
				os << pin->GetFullName() << ": " << pin->Get() << std::endl;
			}
			for (auto & o : gate->GetOutputPins())
			{
				IOPinPtr pin = gate->GetPin(o.second);
				os << pin->GetFullName() << ": " << pin->Get() << std::endl;
			}

			os << std::endl;
			CompositeGatePtr c = std::dynamic_pointer_cast<CompositeGate>(gate);
			if (c && maxLevel > 1)
			{
				for (auto & g : c->GetInternalGates())
				{
					os << PrintInternalStates(g.second, maxLevel-1) << std::endl;
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
					if (pin->GetName() == "vcc" || pin->GetName() == "gnd")
						continue;
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

		void LogicTools::OutputStates(const IOPinListType & inputs, std::vector<std::ostringstream> &outputs, bool busAsHex)
		{
			size_t output = 0;
			for (size_t i = 0; i < inputs.size(); ++i)
			{
				if (inputs[i] == nullptr)
				{
					output++;
					continue;
				}
				IOState state = inputs[i]->Get();
				if (state.GetWidth() == 1)
				{
					outputs[output++] << state;
				}
				else if (busAsHex)
				{
					if (state.IsUndef())
					{
						outputs[output++] << "x";
					}
					else
					{
						outputs[output++] << std::uppercase << std::hex << state.ToInt16();
					}
				}
				else
				{
					for (size_t j = 0; j < state.GetWidth(); ++j)
					{
						outputs[output++] << IOState::ToString(state[j]);
					}
				}
			}
		}

		void LogicTools::FindMaxSize(const DigiLib::Core::IOPinListType & inputs, size_t &outCount, size_t &maxlen, bool busAsHex)
		{
			for (auto & i : inputs)
			{
				if (i == nullptr)
				{
					outCount++;
					continue;
				}
				size_t size = i->GetFullName().size();
				if (i->GetWidth() > 1)
				{
					if (busAsHex)
					{
						if (i->GetWidth() > 4)
						{
							throw std::invalid_argument("busAsHex flag not currently supported for bus > 4 bits");
						}
						outCount++;
					}
					else
					{
						size += 4;
						outCount += (i->GetWidth());
					}
				}
				else
				{
					outCount++;
				}

				if (size > maxlen)
				{
					maxlen = size;
				}
			}
		}

		void LogicTools::PrintHeader(const DigiLib::Core::IOPinListType & inputs, std::vector<std::ostringstream> &outputs, const size_t maxlen, bool busAsHex)
		{
			int output = 0;
			for (size_t i = 0; i < inputs.size(); ++i)
			{
				if (inputs[i] == nullptr)
				{
					output++;
				}
				else if (inputs[i]->GetWidth() == 1 || busAsHex)
				{
					outputs[output++] << std::setw(maxlen) << inputs[i]->GetFullName() << '|';
				}
				else
				{
					for (size_t j = 0; j < inputs[i]->GetWidth(); ++j)
					{
						outputs[output++] << std::setw(maxlen - 3) << inputs[i]->GetFullName() << "[" << j << "]" << '|';
					}
				}
			}
		}

		void LogicTools::PrintOutputs(std::vector<std::ostringstream> &outputs, std::ostringstream &os)
		{
			for (auto & out : outputs)
			{
				os << out.str() << std::endl;
				out.str("");
				out.clear();
			}
		}

		std::string LogicTools::LogicAnalyser(SimulatorPtr sim, const IOPinListType & inputs, size_t maxTicks, size_t modulo, bool busAsHex)
		{
			size_t maxlen = 0;
			size_t outCount = 0;

			std::ostringstream os;

			FindMaxSize(inputs, outCount, maxlen, busAsHex);

			std::vector<std::ostringstream> outputs(outCount);

			PrintHeader(inputs, outputs, maxlen, busAsHex);

			size_t hPos = 1;
			size_t tick = 0;
			size_t block = 2;
			while (sim->GetEventQueue().size())
			{
				if (tick % modulo == 0)
				{
					OutputStates(inputs, outputs, busAsHex);

					if (hPos % (77-maxlen) == 0)
					{
						PrintOutputs(outputs, os);
						os << std::setw(3) << block++ << "/ " << std::string(72, '-') << std::endl;
						PrintHeader(inputs, outputs, maxlen, busAsHex);
					}
					hPos++;
				}

				tick = sim->Tick();

				if (tick > maxTicks)
				{
					break;
				}
			}

			// Output last state
			OutputStates(inputs, outputs, busAsHex);
			PrintOutputs(outputs, os);

			return os.str();
		}

	}
}