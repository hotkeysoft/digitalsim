#include "stdafx.h"
#include "TextParser.h"
#include "Core\IOPin.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"
#include <cctype>
#include <regex>

#define PIN_DEF_REGEX "^(\\/?[A-Za-z](?:\\w){0,31})(?:\\[(\\d+)\\])?$"

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
				throw std::invalid_argument("parser not connected to gate");
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
			pin1->ConnectTo(pin2);
		}

		void TextParser::ParseWireSection(const char * in)
		{
			TextParser::SectionType wireSection = ParseSection(in);
			for (const auto & wire : wireSection)
			{
				ParseConnection(wire.c_str());
			}
		}

		TextParser::PinDefType TextParser::ExtractPinDef(const std::string & in)
		{
			static std::regex pinRegex(PIN_DEF_REGEX);

			std::string name;
			size_t size;

			std::smatch base_match;
			if (std::regex_match(in, base_match, pinRegex))
			{
				// Pin name
				std::ssub_match base_sub_match = base_match[1];
				if (!base_sub_match.matched)
				{
					throw std::invalid_argument("pin name");
				}
				std::string pinName = base_sub_match;

				base_sub_match = base_match[2];
				if (base_sub_match.matched)
				{
					size = atoi(base_sub_match.str().c_str());
				}
				else
				{
					size = 1;
				}

				return std::make_tuple(pinName, size);
			}
			else
			{
				throw std::invalid_argument("invalid pin definition");
			}
		}

		void TextParser::ParseInputsSection(const char * in)
		{
			TextParser::SectionType inputsSection = ParseSection(in);
			for (const auto & in : inputsSection)
			{
				auto out = ExtractPinDef(in);

				std::string name = std::get<0>(out);
				size_t size = std::get<1>(out);

				m_gate->AddInput(name.c_str(), size);
			}
		}

		void TextParser::ParseOutputsSection(const char * in)
		{
			TextParser::SectionType outputsSection = ParseSection(in);
			for (const auto & in : outputsSection)
			{
				auto out = ExtractPinDef(in);

				std::string name = std::get<0>(out);
				size_t size = std::get<1>(out);

				//TODO:HI_Z
				m_gate->AddOutput(name.c_str(), size);
			}
		}

		TextParser::SectionType TextParser::ParseSection(const char * in)
		{
			if (in == nullptr)
			{
				throw std::invalid_argument("null parameter");
			}

			TextParser::SectionType section;

			std::stringstream ss(in);
			while (ss.good())
			{
				std::string substr;
				std::getline(ss, substr, ',');
				substr = trim(substr);
				if (substr.empty())
				{
					throw std::invalid_argument("empty element in statement");
				}
				section.push_back(substr);
			}	

			return section;
		}
	}
}