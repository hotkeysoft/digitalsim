#include "stdafx.h"

#include "GateBase.h"
#include "IOPinSubset.h"
#include <regex>

namespace DigiLib
{
	namespace Core
	{
		AllowedConnectionMapType GateBase::m_insideInsideMap;
		AllowedConnectionMapType GateBase::m_insideParentMap;
		AllowedConnectionMapType GateBase::m_parentInsideMap;

		GateBase::GateBase(const char* name) : m_name(name), m_parent(NULL)
		{
			ValidateGateName(name);
		}


		void GateBase::SetName(const char *name)
		{
			ValidateGateName(name);
			this->m_name = name;
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

		IOPin* GateBase::AddInput(const char* name, size_t width)
		{
			ValidatePinName(name);
			ValidatePinWidth(width);

			if (width == 1)
			{
				m_inputPins[name] = std::make_unique<IOPin>(this, name, IOPin::IO_DIRECTION::INPUT);
			}
			else
			{
				m_inputPins[name] = std::make_unique<IOPin>(this, name, width, IOPin::IO_DIRECTION::INPUT);
			}

			return m_inputPins[name].get();
		}

		IOPin* GateBase::AddOutput(const char* name, size_t width, IOPin::IO_DIRECTION dir)
		{
			ValidatePinName(name);
			ValidatePinWidth(width);

			if (dir != IOPin::IO_DIRECTION::OUTPUT &&
				dir != IOPin::IO_DIRECTION::OUTPUT_HI_Z)
			{
				throw std::invalid_argument("bad output direction");
			}

			if (width == 1)
			{
				m_outputPins[name] = std::make_unique<IOPin>(this, name, dir);
			}
			else
			{
				m_outputPins[name] = std::make_unique<IOPin>(this, name, width, dir);
			}

			return m_outputPins[name].get();
		}

		IOPin* GateBase::GetPin(const char* name)
		{
			if (name == nullptr)
			{
				throw std::invalid_argument("name is null");
			}

			IOPinMapType::const_iterator it = m_inputPins.find(name);
			if (it != m_inputPins.end())
			{
				return it->second.get();
			}

			it = m_outputPins.find(name);
			if (it != m_outputPins.end())
			{
				return it->second.get();
			}

			return nullptr;
		}

		IOPin * GateBase::GetPin(const char * name, size_t offset)
		{
			IOPin* ioPin = GetPin(name);
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

				IOPinSubset* subset = new IOPinSubset(ioPin, offset);
				
				return subset;
			}

			return nullptr;
		}

		IOPin * GateBase::GetPin(const char * name, size_t low, size_t hi)
		{
			IOPin* ioPin = GetPin(name);
			if (ioPin != nullptr)
			{
				if (low < 0 || low >= ioPin->GetWidth() ||
					hi < 0 || hi >= ioPin->GetWidth() ||
					low > hi)
				{
					throw std::out_of_range("invalid pin index");
				}

				IOPinSubset* subset = new IOPinSubset(ioPin, low, hi);

				return subset;
			}

			return nullptr;
		}

		PinConnectionsType& GateBase::GetConnectedToPins(const char * pinName)
		{
			if (pinName == nullptr)
			{
				throw std::invalid_argument("source pin is null");
			}

			IOPin* pin = GetPin(pinName);
			if (pin == nullptr)
			{
				throw std::invalid_argument("source pin not found");
			}

			return GetConnectedToPins(pin);
		}

		PinConnectionsType& GateBase::GetConnectedToPins(IOPin * pin)
		{
			if (pin == nullptr)
			{
				throw std::invalid_argument("source pin is null");
			}

			if (pin->GetParent() != this)
			{
				throw std::invalid_argument("pin belongs to another gate");
			}

			return m_connectedToPins[pin->GetRawName()];
		}

		PinConnectionsType& GateBase::GetConnectedFromPins(const char * pinName)
		{
			if (pinName == nullptr)
			{
				throw std::invalid_argument("source pin is null");
			}

			IOPin* pin = GetPin(pinName);
			if (pin == nullptr)
			{
				throw std::invalid_argument("source pin not found");
			}

			return GetConnectedFromPins(pin);
		}

		PinConnectionsType& GateBase::GetConnectedFromPins(IOPin * pin)
		{
			if (pin == nullptr)
			{
				throw std::invalid_argument("source pin is null");
			}

			if (pin->GetParent() != this)
			{
				throw std::invalid_argument("pin belongs to another gate");
			}

			return m_connectedFromPins[pin->GetRawName()];
		}

		void GateBase::SetParent(GateBase * parent)
		{
			if (m_parent != NULL)
			{
				throw std::invalid_argument("gate already has a parent");
			}

			m_parent = parent;
		}

		void GateBase::ConnectPins(IOPin * source, IOPin * target)
		{
			if (source == NULL || target == NULL)
			{
				throw std::invalid_argument("source or target is null");
			}

			if (target->GetParent() == this)
			{
				throw std::invalid_argument("cannot connect to self");
			}

			if (source->GetWidth() != target->GetWidth())
			{
				throw std::invalid_argument("bus width mismatch");
			}

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

			const IOConnection connection(source, target);
			auto& connectedPins = m_connectedToPins[source->GetRawName()];
			auto found = connectedPins.find(connection);
			if (found != connectedPins.end())
			{
				throw std::invalid_argument("Connection already exists");
			}

			m_connectedToPins[source->GetRawName()].insert(connection);
			target->GetParent()->m_connectedFromPins[target->GetRawName()].insert(connection);
		}

		void GateBase::InitAllowedConnectionMaps()
		{
			if (m_insideInsideMap.empty())
			{
				m_insideInsideMap[IOPin::INPUT][IOPin::INPUT] = false;
				m_insideInsideMap[IOPin::INPUT][IOPin::OUTPUT] = false;
				m_insideInsideMap[IOPin::INPUT][IOPin::OUTPUT_HI_Z] = false;

				m_insideInsideMap[IOPin::OUTPUT][IOPin::INPUT] = true;
				m_insideInsideMap[IOPin::OUTPUT][IOPin::OUTPUT] = false;
				m_insideInsideMap[IOPin::OUTPUT][IOPin::OUTPUT_HI_Z] = false;

				m_insideInsideMap[IOPin::OUTPUT_HI_Z][IOPin::INPUT] = true;
				m_insideInsideMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT] = false;
				m_insideInsideMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT_HI_Z] = false;
			}

			if (m_parentInsideMap.empty())
			{
				m_parentInsideMap[IOPin::INPUT][IOPin::INPUT] = true;
				m_parentInsideMap[IOPin::INPUT][IOPin::OUTPUT] = false;
				m_parentInsideMap[IOPin::INPUT][IOPin::OUTPUT_HI_Z] = false;

				m_parentInsideMap[IOPin::OUTPUT][IOPin::INPUT] = false;
				m_parentInsideMap[IOPin::OUTPUT][IOPin::OUTPUT] = false;
				m_parentInsideMap[IOPin::OUTPUT][IOPin::OUTPUT_HI_Z] = false;

				m_parentInsideMap[IOPin::OUTPUT_HI_Z][IOPin::INPUT] = false;
				m_parentInsideMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT] = false;
				m_parentInsideMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT_HI_Z] = false;
			}

			if (m_insideParentMap.empty())
			{
				m_insideParentMap[IOPin::INPUT][IOPin::INPUT] = false;
				m_insideParentMap[IOPin::INPUT][IOPin::OUTPUT] = false;
				m_insideParentMap[IOPin::INPUT][IOPin::OUTPUT_HI_Z] = false;

				m_insideParentMap[IOPin::OUTPUT][IOPin::INPUT] = false;
				m_insideParentMap[IOPin::OUTPUT][IOPin::OUTPUT] = true;
				m_insideParentMap[IOPin::OUTPUT][IOPin::OUTPUT_HI_Z] = true;

				m_insideParentMap[IOPin::OUTPUT_HI_Z][IOPin::INPUT] = false;
				m_insideParentMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT] = true;
				m_insideParentMap[IOPin::OUTPUT_HI_Z][IOPin::OUTPUT_HI_Z] = true;
			}

		}

		bool GateBase::IsValidPinName(const char* name)
		{
			if (name == NULL)
			{
				return false;
			}

			if (!std::regex_match(name, std::regex("^[A-Za-z](\\w){0,31}$")))
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

			if (m_inputPins.find(name) != m_inputPins.end() ||
				m_outputPins.find(name) != m_outputPins.end())
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
			if (name == NULL)
			{
				return false;
			}

			if (!std::regex_match(name, std::regex("^[A-Za-z](\\w){0,31}$")))
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
	}
}