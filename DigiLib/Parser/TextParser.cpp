#include "stdafx.h"
#include "TextParser.h"
#include "Core\IOPin.h"
#include "Core\GateBase.h"
#include <cctype>

namespace DigiLib {
	namespace Parser {
		using namespace DigiLib::Core;

		inline std::string trim(const std::string &s)
		{
			auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) {return std::isspace(c); });
			auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c) {return std::isspace(c); }).base();
			return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
		}

		void TextParser::ParseConnection(const char * in)
		{
			static const std::string separator = "->";

			if (m_gate == nullptr)
			{
				return; // nothing to do
			}

			if (in == nullptr)
			{
				throw std::invalid_argument("input string is null");
			}

			std::string inStr(in);
			size_t firstSeparator = inStr.find(separator);

			if (firstSeparator == -1)
			{
				throw std::invalid_argument("connection separator not found");
			}
			size_t nextSeparator = inStr.find(separator, firstSeparator+1);
			if (nextSeparator != -1)
			{
				throw std::invalid_argument("more than one connection separator");
			}
			
			std::string pinName1 = trim(inStr.substr(0, firstSeparator));
			std::string pinName2 = trim(inStr.substr(firstSeparator + separator.size()));
			if (pinName1.empty())
			{
				throw std::invalid_argument("source pin is empty");
			}
			if (pinName2.empty())
			{
				throw std::invalid_argument("target pin is empty");
			}

			IOPinPtr pin1 = m_gate->FindPin(pinName1.c_str());
			IOPinPtr pin2 = m_gate->FindPin(pinName2.c_str());
			if (pin1 == nullptr || pin2 == nullptr)
			{
				throw std::invalid_argument("pin not found");
			}

			//TODO how to indicate 'negative' connections?
			std::cout << "connect " << pin1->GetFullName() << " -> " << pin2->GetFullName() << std::endl;
			pin1->ConnectTo(pin2);
		}
	}
}