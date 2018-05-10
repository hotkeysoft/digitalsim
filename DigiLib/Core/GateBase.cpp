#include "stdafx.h"

#include "GateBase.h"
#include "IOPin.h"
#include "IOPinSubset.h"
#include <regex>
#include <memory>

#define PIN_NAME_REGEX "(\\/?[A-Za-z](?:\\w){0,31})"
#define PIN_RANGE_REGEX "(?:\\[(\\d+)(?:-(\\d+))?\\])?"
#define GATE_NAME_REGEX "([A-Za-z](?:\\w){0,31})"

namespace DigiLib
{
	namespace Core
	{
		AllowedConnectionMapType GateBase::m_insideInsideMap;
		AllowedConnectionMapType GateBase::m_insideParentMap;
		AllowedConnectionMapType GateBase::m_parentInsideMap;

		GateBase::GateBase(const char* name, size_t delay) : m_name(name), m_parent(NULL), m_ioPinCount(0), m_mode(ASYNC), m_simulator(nullptr), m_delay(delay)
		{
			m_ioPins.reserve(MAX_PINS * 2);
			m_connectedFromPins.reserve(MAX_PINS);
			m_connectedToPins.reserve(MAX_PINS);
			ValidateGateName(name);
		}
		
		void GateBase::SetName(const char *name)
		{
			ValidateGateName(name);
			this->m_name = name;
		}

		void GateBase::SetMode(Mode mode, SimulatorRef simulator)
		{
			if (mode == SYNC)
			{
				if (simulator == nullptr)
					throw std::invalid_argument("simulator not set");
				if (this->m_simulator != nullptr)
					throw std::invalid_argument("simulator already set");
				if (this->m_parent == nullptr || this->m_parent->GetMode() != SYNC)
					throw std::invalid_argument("gate should be child of simulator");
				this->m_mode = mode;
				this->m_simulator = simulator;
			}
			else
			{
				if (simulator != nullptr)
					throw std::invalid_argument("simulator shouldn't be set");
				if (this->m_parent != nullptr && this->m_parent->GetMode() == SYNC)
					throw std::invalid_argument("parent and child should have same mode");
				this->m_mode = mode;
				this->m_simulator = nullptr;
			}
		}

		std::string GateBase::GetFullName()
		{
			std::ostringstream os;
			if (m_parent)
			{
				os << m_parent->GetFullName() << ".";
			}
			os << m_name;

			return os.str();
		}

		void GateBase::InitVccGndPins()
		{
			m_vccPin = std::make_shared<IOPin>(this, MAX_PINS, "vcc", 1, IOPin::IO_DIRECTION::POWER);
			m_connectedToVcc.clear();
			
			m_gndPin = std::make_shared<IOPin>(this, MAX_PINS, "gnd", 1, IOPin::IO_DIRECTION::POWER);
			m_connectedToGnd.clear();
		}

		IOPinPtr GateBase::AddInput(const char* name, size_t width)
		{
			ValidatePinName(name);
			ValidatePinWidth(width);

			GatePtr thisGate = this->shared_from_this();
			size_t newPinID = m_ioPinCount;
			m_ioPins.push_back(std::make_shared<IOPin>(this, newPinID, name, width, IOPin::IO_DIRECTION::INPUT));
			m_inputPinsNames[name] = newPinID;
			m_ioPinCount++;
			m_connectedFromPins.resize(m_ioPinCount);
			m_connectedToPins.resize(m_ioPinCount);

			return m_ioPins[newPinID];
		}

		IOPinPtr GateBase::AddOutput(const char* name, size_t width, IOPin::IO_DIRECTION dir)
		{
			ValidatePinName(name);
			ValidatePinWidth(width);

			if (dir != IOPin::IO_DIRECTION::OUTPUT &&
				dir != IOPin::IO_DIRECTION::OUTPUT_HI_Z)
			{
				throw std::invalid_argument("bad output direction");
			}

			GatePtr thisGate = this->shared_from_this();
			size_t newPinID = m_ioPinCount;
			m_ioPins.push_back(std::make_shared<IOPin>(this, newPinID, name, width, dir));
			m_outputPinsNames[name] = newPinID;
			m_ioPinCount++;
			m_connectedFromPins.resize(m_ioPinCount);
			m_connectedToPins.resize(m_ioPinCount);

			return m_ioPins[newPinID];
		}

		IOPinPtr GateBase::GetPin(const char* name)
		{
			if (name == nullptr)
			{
				throw std::invalid_argument("name is null");
			}

			if (m_vccPin->GetName() == name)
			{
				return m_vccPin;
			}
			if (m_gndPin->GetName() == name)
			{
				return m_gndPin;
			}

			auto it = m_inputPinsNames.find(name);
			if (it != m_inputPinsNames.end())
			{
				return m_ioPins[it->second];
			}

			it = m_outputPinsNames.find(name);
			if (it != m_outputPinsNames.end())
			{
				return m_ioPins[it->second];
			}

			return nullptr;
		}

		IOPinPtr GateBase::GetPin(const char * name, size_t offset)
		{
			IOPinPtr ioPin = GetPin(name);
			if (ioPin != nullptr)
			{
				if (ioPin->GetWidth() < 2)
				{
					throw std::invalid_argument("pin is not a bus");
				}
				else if (offset < 0 || offset >= ioPin->GetWidth())
				{
					throw std::out_of_range("invalid pin index");
				}

				IOPinPtr subset = std::make_shared<IOPinSubset>(ioPin.get(), offset);
				
				return subset;
			}

			return nullptr;
		}

		IOPinPtr GateBase::GetPin(const char * name, size_t low, size_t hi)
		{
			IOPinPtr ioPin = GetPin(name);
			if (ioPin != nullptr)
			{
				if (low < 0 || low >= ioPin->GetWidth() ||
					hi < 0 || hi >= ioPin->GetWidth() ||
					low > hi)
				{
					throw std::out_of_range("invalid pin index");
				}

				IOPinPtr subset = std::make_shared<IOPinSubset>(ioPin.get(), low, hi);

				return subset;
			}

			return nullptr;
		}

		PinConnectionsType& GateBase::GetConnectedToPin(size_t pinID)
		{
			if (pinID < 0 || pinID >= m_ioPinCount)
			{
				throw std::invalid_argument("invalid pin ID");
			}

			return m_connectedToPins[pinID];
		}
		PinConnectionsType& GateBase::GetConnectedToPin(const char * pinName)
		{
			if (pinName == nullptr)
			{
				throw std::invalid_argument("source pin is null");
			}

			IOPinPtr pin = GetPin(pinName);
			if (pin == nullptr)
			{
				throw std::invalid_argument("source pin not found");
			}

			return GetConnectedToPin(pin->GetID());
		}

		PinConnectionsType& GateBase::GetConnectedToPin(IOPinPtr pin)
		{
			if (pin == nullptr)
			{
				throw std::invalid_argument("source pin is null");
			}

			if (pin->GetParent() != this)
			{
				throw std::invalid_argument("pin belongs to another gate");
			}

			if (pin == m_vccPin)
			{
				return m_connectedToVcc;
			}
			else if (pin == m_gndPin)
			{
				return m_connectedToGnd;
			}

			return m_connectedToPins[pin->GetID()];
		}

		PinConnectionsType& GateBase::GetConnectedFromPin(size_t pinID)
		{
			if (pinID < 0 || pinID >= m_ioPinCount)
			{
				throw std::invalid_argument("invalid pin ID");
			}

			return m_connectedFromPins[pinID];
		}
		PinConnectionsType& GateBase::GetConnectedFromPin(const char * pinName)
		{
			if (pinName == nullptr)
			{
				throw std::invalid_argument("source pin is null");
			}

			IOPinPtr pin = GetPin(pinName);
			if (pin == nullptr)
			{
				throw std::invalid_argument("source pin not found");
			}

			return GetConnectedFromPin(pin->GetID());
		}

		PinConnectionsType& GateBase::GetConnectedFromPin(IOPinPtr pin)
		{
			if (pin == nullptr)
			{
				throw std::invalid_argument("source pin is null");
			}

			if (pin->GetParent() != this)
			{
				throw std::invalid_argument("pin belongs to another gate");
			}

			return m_connectedFromPins[pin->GetID()];
		}

		void GateBase::SetParent(GateRef parent)
		{
			if (m_parent != NULL)
			{
				throw std::invalid_argument("gate already has a parent");
			}

			m_parent = parent;
		}

		void GateBase::Reset()
		{
			this->m_connectedFromPins.clear();
			this->m_connectedToPins.clear();
			this->m_delay = 1;
			this->m_inputPinsNames.clear();
			this->m_outputPinsNames.clear();
			this->m_ioPins.clear();
			this->m_ioPinCount = 0;
			this->InitVccGndPins();
		}

		void GateBase::ResetPins()
		{
			for (auto pin : m_ioPins)
			{
				pin->Reset();
			}
			GetPin("vcc")->Reset(IOState::UNDEF);
			GetPin("gnd")->Reset(IOState::UNDEF);
		}

		IOPinPtr GateBase::FindPin(const char * name)
		{
			static std::regex pinRegex(PIN_NAME_REGEX PIN_RANGE_REGEX);		

			if (name == nullptr)
			{
				throw std::invalid_argument("name cannot be null");
			}

			std::cmatch base_match;
			if (std::regex_match(name, base_match, pinRegex))
			{
				// Pin name
				std::csub_match base_sub_match = base_match[1];
				if (!base_sub_match.matched)
				{
					return nullptr;
				}
				std::string pinName = base_sub_match;

				base_sub_match = base_match[2];
				if (!base_sub_match.matched)
				{
					return GetPin(pinName.c_str());
				}

				size_t pinLow = atoi(base_sub_match.str().c_str());
				size_t pinHi = pinLow;

				base_sub_match = base_match[3];
				if (base_sub_match.matched)
				{
					pinHi = atoi(base_sub_match.str().c_str());
				}

				return GetPin(pinName.c_str(), pinLow, pinHi);
			}

			return IOPinPtr();
		}

		bool GateBase::ConnectPins(IOPinPtr source, IOPinPtr target, bool inverted)
		{
			if (source == NULL || target == NULL)
			{
				throw std::invalid_argument("source or target is null");
			}

			// If source is vcc or gnd, ignore sizes
			if ((source->GetWidth() != target->GetWidth()))
			{
				throw std::invalid_argument("bus width mismatch");
			}

			// Reset all pins
			source->Reset();
			target->Reset();
			ResetPins();
			target->GetParent()->ResetPins();

			const bool insideInside = (GetParent() == target->GetParent()->GetParent());
			const bool insideToParent = (GetParent() == target->GetParent());
			const bool parentToInside = (this == target->GetParent()->GetParent());

			InitAllowedConnectionMaps();
			if ((insideInside && !m_insideInsideMap[source->GetDirection()][target->GetDirection()]) ||
				(insideToParent && !m_insideParentMap[source->GetDirection()][target->GetDirection()]) ||
				(parentToInside && !m_parentInsideMap[source->GetDirection()][target->GetDirection()]) ||
				(!insideInside && !insideToParent && !parentToInside))
			{
				throw std::invalid_argument("Not allowed by connection rules");
			}

			const IOConnection connection(source, target, inverted);

			if (CanConnectToTarget(connection))
			{
				if (source == m_vccPin)
				{
					m_connectedToVcc.insert(connection);
				}
				else if (source == m_gndPin)
				{
					m_connectedToGnd.insert(connection);
				}
				else 
				{
					m_connectedToPins[source->GetID()].insert(connection);
				}
				target->GetParent()->m_connectedFromPins[target->GetID()].insert(connection);
				return true;
			}

			return false;
		}

		bool GateBase::CanConnectToTarget(const IOConnection & link)
		{
			if (link.GetSource()->GetDirection() == IOPin::OUTPUT_HI_Z)
			{
				// TODO: May need to be more subtle...
				return true;
			}

			// Build target connections mask
			IOPinPtr target = link.GetTarget();
			GateRef targetGate = target->GetParent();

			IOState mask = IOState(IOState::UNDEF, target->GetMask().GetWidth());
			for (const auto & t : targetGate->GetConnectedFromPin(target))
			{
				mask = mask | t.GetTarget()->GetMask();
			}
			
			if (target->Overlaps(mask))
			{
				return false;
			}

			return true;
		}

		void GateBase::InitAllowedConnectionMaps()
		{
			if (m_insideInsideMap.empty())
			{
				m_insideInsideMap[IOPin::INPUT][IOPin::INPUT] = false;
				m_insideInsideMap[IOPin::INPUT][IOPin::OUTPUT] = false;
				m_insideInsideMap[IOPin::INPUT][IOPin::OUTPUT_HI_Z] = false;
				m_insideInsideMap[IOPin::INPUT][IOPin::POWER] = false;

				m_insideInsideMap[IOPin::OUTPUT][IOPin::INPUT] = true;
				m_insideInsideMap[IOPin::OUTPUT][IOPin::OUTPUT] = false;
				m_insideInsideMap[IOPin::OUTPUT][IOPin::OUTPUT_HI_Z] = false;
				m_insideInsideMap[IOPin::OUTPUT][IOPin::POWER] = false;

				m_insideInsideMap[IOPin::OUTPUT_HI_Z][IOPin::INPUT] = true;
				m_insideInsideMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT] = false;
				m_insideInsideMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT_HI_Z] = false;
				m_insideInsideMap[IOPin::OUTPUT_HI_Z][IOPin::POWER] = false;

				m_insideInsideMap[IOPin::POWER][IOPin::INPUT] = true;
				m_insideInsideMap[IOPin::POWER][IOPin::OUTPUT] = false;
				m_insideInsideMap[IOPin::POWER][IOPin::OUTPUT_HI_Z] = false;
				m_insideInsideMap[IOPin::POWER][IOPin::POWER] = false;
			}

			if (m_parentInsideMap.empty())
			{
				m_parentInsideMap[IOPin::INPUT][IOPin::INPUT] = true;
				m_parentInsideMap[IOPin::INPUT][IOPin::OUTPUT] = false;
				m_parentInsideMap[IOPin::INPUT][IOPin::OUTPUT_HI_Z] = false;
				m_parentInsideMap[IOPin::INPUT][IOPin::POWER] = false;

				m_parentInsideMap[IOPin::OUTPUT][IOPin::INPUT] = false;
				m_parentInsideMap[IOPin::OUTPUT][IOPin::OUTPUT] = false;
				m_parentInsideMap[IOPin::OUTPUT][IOPin::OUTPUT_HI_Z] = false;
				m_parentInsideMap[IOPin::OUTPUT][IOPin::POWER] = false;

				m_parentInsideMap[IOPin::OUTPUT_HI_Z][IOPin::INPUT] = false;
				m_parentInsideMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT] = false;
				m_parentInsideMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT_HI_Z] = false;
				m_parentInsideMap[IOPin::OUTPUT_HI_Z][IOPin::POWER] = false;

				m_parentInsideMap[IOPin::POWER][IOPin::INPUT] = true;
				m_parentInsideMap[IOPin::POWER][IOPin::OUTPUT] = false;
				m_parentInsideMap[IOPin::POWER][IOPin::OUTPUT_HI_Z] = false;
				m_parentInsideMap[IOPin::POWER][IOPin::POWER] = false;

			}

			if (m_insideParentMap.empty())
			{
				m_insideParentMap[IOPin::INPUT][IOPin::INPUT] = false;
				m_insideParentMap[IOPin::INPUT][IOPin::OUTPUT] = false;
				m_insideParentMap[IOPin::INPUT][IOPin::OUTPUT_HI_Z] = false;
				m_insideParentMap[IOPin::INPUT][IOPin::POWER] = false;

				m_insideParentMap[IOPin::OUTPUT][IOPin::INPUT] = false;
				m_insideParentMap[IOPin::OUTPUT][IOPin::OUTPUT] = true;
				m_insideParentMap[IOPin::OUTPUT][IOPin::OUTPUT_HI_Z] = true;
				m_insideParentMap[IOPin::OUTPUT][IOPin::POWER] = false;

				m_insideParentMap[IOPin::OUTPUT_HI_Z][IOPin::INPUT] = false;
				m_insideParentMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT] = true;
				m_insideParentMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT_HI_Z] = true;
				m_insideParentMap[IOPin::OUTPUT_HI_Z][IOPin::POWER] = false;

				m_insideParentMap[IOPin::POWER][IOPin::INPUT] = false;
				m_insideParentMap[IOPin::POWER][IOPin::OUTPUT] = true;
				m_insideParentMap[IOPin::POWER][IOPin::OUTPUT_HI_Z] = true;
				m_insideParentMap[IOPin::POWER][IOPin::POWER] = false;

			}

		}

		bool GateBase::IsValidPinName(const char* name)
		{
			static std::regex pinNameRegex(PIN_NAME_REGEX);

			if (name == NULL)
			{
				return false;
			}

			if (!std::regex_match(name, pinNameRegex))
			{
				return false;
			}

			return true;
		}

		void GateBase::ValidatePinName(const char * name)
		{
			if (!IsValidPinName(name))
			{
				throw std::invalid_argument("invalid pin name");
			}
			if (m_vccPin->GetName() == name ||
				m_gndPin->GetName() == name)
			{
				throw std::invalid_argument("reserved pin name: " + std::string(name));
			}

			if (m_inputPinsNames.find(name) != m_inputPinsNames.end() ||
				m_outputPinsNames.find(name) != m_outputPinsNames.end())
			{
				throw std::invalid_argument("duplicate pin name");
			}
		}

		void GateBase::ValidatePinWidth(size_t width)
		{
			if (width < 1)
			{
				throw std::out_of_range("width must be >=1");
			}
			else if (width > MAX_PINS)
			{
				throw std::out_of_range("width>MAX_WIDTH");
			}
		}

		bool GateBase::IsValidGateName(const char* name)
		{
			static std::regex gateNameRegex(GATE_NAME_REGEX);

			if (name == NULL)
			{
				return false;
			}

			if (!std::regex_match(name, gateNameRegex))
			{
				return false;
			}

			return true;
		}

		void GateBase::ValidateGateName(const char * name)
		{
			if (!IsValidGateName(name))
			{
				throw std::invalid_argument("invalid gate name");
			}
		}
		void GateBase::Init()
		{
			InitVccGndPins();
		}
		void GateBase::InitializeState()
		{
			m_vccPin->Set(IOState::HI);
			m_gndPin->Set(IOState::LOW);
		}
	}
}