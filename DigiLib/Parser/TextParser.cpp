#include "stdafx.h"
#include "TextParser.h"
#include "Core\IOPin.h"
#include "Core\GateBase.h"
#include "Core\CompositeGate.h"
#include <cctype>
#include <regex>
#include "PartsBin.h"

#define PIN_DEF_REGEX "^(\\/?[A-Za-z](?:\\w){0,31})(?:\\[(\\d+)\\])?$"
#define PART_DEF_REGEX "^([A-Za-z](?:\\w){0,31})\\s+([A-Za-z](?:\\w){0,31})$"

namespace DigiLib {
	namespace Parser {
		using namespace DigiLib::Core;

		inline std::string trim(const std::string &s)
		{
			auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) {return std::isspace(c); });
			auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c) {return std::isspace(c); }).base();
			return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
		}

		void TextParser::ParseGate(const char * in)
		{
			auto sections = GetSections(in);
			for (const auto & section : sections)
			{
				if (section.Name == "Parts")
					ParsePartsSection(section.Data.c_str());
				else if (section.Name == "Inputs")
					ParseInputsSection(section.Data.c_str());
				else if (section.Name == "Outputs")
					ParseOutputsSection(section.Data.c_str());
				else if (section.Name == "Wires")
					ParseWireSection(section.Data.c_str());
				else throw std::invalid_argument("unknown section");
			}
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

			std::string inStr = trim(in);
			if (inStr.empty())
				return;

			size_t firstSeparator = inStr.find(separator);

			if (firstSeparator == -1)
			{
				throw std::invalid_argument("connection separator not found");
			}
			size_t nextSeparator = inStr.find(separator, firstSeparator + 1);
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

		void TextParser::ParsePartsSection(const char * in)
		{
			if (m_parts == nullptr)
			{
				throw std::invalid_argument("no parts bin assigned");
			}

			TextParser::SectionElement partsSection = ParseSection(in);
			for (const auto & part : partsSection)
			{
				if (!part.empty())
				{
					auto out = ExtractPartDef(part);

					std::string type = std::get<0>(out);
					std::string name = std::get<1>(out);

					// Check if name matches a part type
					if (m_parts->Find(name.c_str()))
					{
						throw std::invalid_argument("there is a part with this name");
					}

					GatePtr part = m_parts->Find(type.c_str());
					if (part == nullptr)
					{
						throw std::invalid_argument("part not found");
					}

					m_gate->AddGate(name.c_str(), part->Clone(name.c_str()));
				}
			}
		}

		void TextParser::ParseWireSection(const char * in)
		{
			TextParser::SectionElement wireSection = ParseSection(in);
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
			if (!std::regex_match(in, base_match, pinRegex))
			{
				throw std::invalid_argument("invalid pin definition");
			}
			// Pin name
			assert(base_match[1].matched);
			std::string pinName = base_match[1];

			// Pin width
			if (base_match[2].matched)
			{
				size = atoi(base_match[2].str().c_str());
			}
			else
			{
				size = 1;
			}

			return std::make_tuple(pinName, size);
		}

		TextParser::PartDefType TextParser::ExtractPartDef(const std::string & in)
		{
			static std::regex pinRegex(PART_DEF_REGEX);

			std::smatch base_match;
			if (!std::regex_match(in, base_match, pinRegex))
			{
				throw std::invalid_argument("invalid part definition");
			}

			assert(base_match[1].matched);
			assert(base_match[2].matched);

			return std::make_tuple(base_match[1], base_match[2]);
		}

		void TextParser::ParseInputsSection(const char * in)
		{
			TextParser::SectionElement inputsSection = ParseSection(in);
			for (const auto & section : inputsSection)
			{
				if (!section.empty())
				{
					auto out = ExtractPinDef(section);

					std::string name = std::get<0>(out);
					size_t size = std::get<1>(out);

					m_gate->AddInput(name.c_str(), size);
				}
			}
		}

		void TextParser::ParseOutputsSection(const char * in)
		{
			TextParser::SectionElement outputsSection = ParseSection(in);
			for (const auto & in : outputsSection)
			{
				if (!in.empty())
				{
					auto out = ExtractPinDef(in);

					std::string name = std::get<0>(out);
					size_t size = std::get<1>(out);

					//TODO:HI_Z
					m_gate->AddOutput(name.c_str(), size);
				}
			}
		}

		TextParser::Sections TextParser::GetSections(const char * in)
		{
			if (in == nullptr)
			{
				throw std::invalid_argument("null parameter");
			}

			TextParser::Sections sections;

			std::stringstream ss(in);
			while (ss.good())
			{
				std::string substr;
				std::getline(ss, substr, ';');
				substr = trim(substr);
				sections.push_back(ExtractSection(substr));
			}
			// Since we need a trailing ;, we expect an empty element at the end
			if (sections.size() > 0)
			{
				if (!sections.rbegin()->Name.empty())
				{
					throw std::invalid_argument("unterminated section (need trailing ;)");
				}
				sections.pop_back();
			}

			return sections;
		}

		TextParser::Section TextParser::ExtractSection(const std::string & in)
		{
			if (in.size() == 0)
			{
				return TextParser::Section();
			}
			size_t index = in.find_first_of(':');
			if (index == -1)
			{
				throw std::invalid_argument("section label not found");
			}
			if (in.find_first_of(':', index + 1) != -1)
			{
				throw std::invalid_argument("Unexpected ':' character");
			}
			Section section;
			section.Name = trim(in.substr(0, index));
			section.Data = trim(in.substr(index + 1));
			if (section.Name.empty())
			{
				throw std::invalid_argument("missing section label");
			}
			return section;
		}

		TextParser::SectionElement TextParser::ParseSection(const char * in)
		{
			if (in == nullptr)
			{
				throw std::invalid_argument("null parameter");
			}

			TextParser::SectionElement section;

			bool hasEmptyElement = false;
			std::stringstream ss(in);
			while (ss.good())
			{
				std::string substr;
				std::getline(ss, substr, ',');
				substr = trim(substr);
				if (substr.empty())
				{
					hasEmptyElement = true;
				}
				section.push_back(substr);
			}

			if (section.size() > 1 && hasEmptyElement)
			{
				throw std::invalid_argument("empty element in section");
			}

			return section;
		}
	}
}