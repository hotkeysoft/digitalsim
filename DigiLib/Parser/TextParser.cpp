#include "stdafx.h"
#include "TextParser.h"
#include "Core/IOPin.h"
#include "Core/GateBase.h"
#include "Core/CompositeGate.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include "PartsBin.h"

namespace DigiLib {
	namespace Parser {
		using namespace DigiLib::Core;

		inline bool isWordChar(char ch)
		{
			return ((ch >= 'A' && ch <= 'Z') ||
				(ch >= 'a' && ch <= 'z') ||
				(ch >= '0' && ch <= '9') ||
				ch == '_');
		}

		inline bool isAlpha(char ch)
		{
			return ((ch >= 'A' && ch <= 'Z') ||
				(ch >= 'a' && ch <= 'z'));
		}

		inline std::string trim(const std::string &s)
		{
			auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) {return std::isspace(c); });
			auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c) {return std::isspace(c); }).base();
			return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
		}

		TextParser::SectionDef TextParser::m_sectionNames = {
			{ "Description", false },
			{ "Inputs", true },
			{ "Outputs", true },
			{ "Parts", true },
			{ "Wires", true },
		};

		void TextParser::LoadFromFile(const char * path)
		{
			if (path == nullptr)
			{
				throw std::invalid_argument("path is null");
			}

			std::ifstream t(path);
			if (!t)
			{
				throw std::invalid_argument("Unable to open file: " + std::string(path));
			}

			std::stringstream buffer;
			buffer << t.rdbuf();

			ParseGate(buffer.str().c_str());
			m_gate->InitializeState();
		}

		void TextParser::ParseGate(const char * in)
		{
			const Sections & sections = GetSections(in);
			Sections::const_iterator currSection = sections.begin();

			for (const auto & sectionDef : m_sectionNames)
			{
				// mandatory section
				if (sectionDef.mandatory)
				{
					const std::string &expected = sectionDef.name;
					if (currSection == sections.end())
					{
						throw std::invalid_argument("missing section: " + expected);
					}
					else if (currSection->name != expected)
					{
						throw std::invalid_argument("expected section: " + expected);
					}
					else 
					{ 
						// Check next one
						++currSection; 
					}
				}
				else // optional section
				{
					const std::string &expected = sectionDef.name;
					if (currSection->name == expected)
					{
						// Check next one
						++currSection;
					}
				}
			}
			if (currSection != sections.end())
			{
				throw std::invalid_argument("unexpected section: " + currSection->name);
			}

			for (const auto & section : sections)
			{
				ParseSection(section);
			}
		}

		void TextParser::ParseSection(const DigiLib::Parser::TextParser::Section & section)
		{
			if (section.name == "Description")
			{
				//TODO
			}
			else if (section.name == "Parts")
				ParsePartsSection(section.data.c_str());
			else if (section.name == "Inputs")
				ParseInputsSection(section.data.c_str());
			else if (section.name == "Outputs")
				ParseOutputsSection(section.data.c_str());
			else if (section.name == "Wires")
				ParseWireSection(section.data.c_str());
			else throw std::invalid_argument("unknown section: " + section.name);
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

			if (firstSeparator == std::string::npos)
			{
				throw std::invalid_argument("connection separator not found");
			}
			size_t nextSeparator = inStr.find(separator, firstSeparator + 1);
			if (nextSeparator != std::string::npos)
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
			if (pin1 == nullptr)
			{
				throw std::invalid_argument("pin not found: " + pinName1);
			}
			if (pin2 == nullptr)
			{
				throw std::invalid_argument("pin not found: " + pinName2);
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
					if (m_parts->HasGate(name.c_str()))
					{
						throw std::invalid_argument("there is a part named: " + name);
					}

					GatePtr part = m_parts->Find(type.c_str());
					if (part == nullptr)
					{
						throw std::invalid_argument("part not found: " + type);
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
			char pinName[40];
			const char* endPos = TextParser::ExtractPinName(in.c_str(), pinName, 40);
			if (endPos == nullptr)
				throw std::invalid_argument("invalid pin definition");

			if (*endPos == '\0')
			{
				return std::make_tuple(pinName, 1);
			}

			size_t size;
			endPos = TextParser::ExtractPinSize(endPos, size);

			if (endPos && *endPos == '\0')			
			{
				return std::make_tuple(pinName, size);
			}

			throw std::invalid_argument("invalid pin definition");
		}

		TextParser::PartDefType TextParser::ExtractPartDef(const std::string & in)
		{
			char partType[40];
			const char* endPos = ExtractGateName(in.c_str(), partType, 40);
			if (endPos == nullptr || *endPos == '\0')
				throw std::invalid_argument("invalid part definition");

			// Skip spaces
			endPos = SkipSpaces(endPos);
			if (endPos == nullptr || *endPos == '\0')
			{
				throw std::invalid_argument("invalid part definition");
			}

			char partName[40];
			endPos = TextParser::ExtractGateName(endPos, partName, 40);

			if (endPos && *endPos == '\0')
			{
				return std::make_tuple(partType, partName);
			}

			throw std::invalid_argument("invalid part definition");
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

			std::stringstream ss(RemoveComments(in));
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
				if (!sections.rbegin()->name.empty())
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
			if (index == std::string::npos)
			{
				throw std::invalid_argument("section label not found");
			}
			if (in.find_first_of(':', index + 1) != std::string::npos)
			{
				throw std::invalid_argument("Unexpected ':' character");
			}
			Section section;
			section.name = trim(in.substr(0, index));
			section.data = trim(in.substr(index + 1));
			if (section.name.empty())
			{
				throw std::invalid_argument("missing section label");
			}
			return section;
		}

		std::string TextParser::RemoveComments(const std::string & in)
		{
			std::string out = in;
			size_t pos;
			while((pos = out.find_first_of('#')) != std::string::npos)
			{
				size_t endPos = out.find_first_of("\n", pos);
				if (endPos == std::string::npos)
				{
					out.erase(pos);
					break;
				}
				else
				{
					out.erase(pos, endPos-pos);
				}
			}

			return out;
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

		const char * TextParser::ExtractGateName(const char * in, char * out, size_t outSize)
		{
			const char *endpos = ReadGateName(in);
			if (endpos == nullptr)
				return nullptr;

			strncpy(out, in, endpos - in);
			out[endpos - in] = '\0';

			return endpos;
		}

		const char * TextParser::ExtractPinName(const char * in, char * out, size_t outSize)
		{
			const char *endpos = ReadPinName(in);
			if (endpos == nullptr)
				return nullptr;

			strncpy(out, in, endpos - in);
			out[endpos - in] = '\0';

			return endpos;
		}

		const char * TextParser::ReadPinNumber(const char * in, size_t & out)
		{
			out = 0;
			const char * endpos = in;
			if (endpos == nullptr)
				return nullptr;

			// At most two digits
			if (!isdigit(*endpos))
				return nullptr;

			// First digit
			out = *endpos - '0';
			++endpos;

			// (optional) 2nd digit
			if (!isdigit(*endpos))
				return endpos;

			out *= 10;
			out += *endpos - '0';

			return ++endpos;
		}

		const char * TextParser::ExtractPinRange(const char * in, size_t & pinLow, size_t & pinHi)
		{
			const char *endPos = in;
			if (endPos == nullptr)
				return nullptr;

			if (*endPos != '[')
				return nullptr;

			++endPos;

			endPos = ReadPinNumber(endPos, pinLow);
			if (endPos == nullptr)
				return nullptr;

			if (*endPos == ']')
			{
				pinHi = pinLow;
				return ++endPos;
			}

			if (*endPos != '-')
				return nullptr;

			endPos = ReadPinNumber(++endPos, pinHi);
			if (endPos == nullptr)
				return nullptr;

			if (*endPos == ']')
			{
				return ++endPos;
			}

			return nullptr;
		}

		const char * TextParser::ExtractPinSize(const char * in, size_t & size)
		{
			const char *endPos = in;
			if (endPos == nullptr)
				return nullptr;

			if (*endPos != '[')
				return nullptr;

			++endPos;

			endPos = ReadPinNumber(endPos, size);
			if (endPos == nullptr)
				return nullptr;

			if (*endPos == ']')
			{
				return ++endPos;
			}

			return nullptr;
		}

		const char * TextParser::ReadPinName(const char * in)
		{
			if (in == nullptr)
			{
				return nullptr;
			}

			size_t index = 0;

			if (in[index] == '\0')
			{
				return nullptr;
			}

			// Optional '/' at beginning
			if (in[index] == '/')
			{
				++index;
			}

			// first character = letter
			if (!isalpha(in[index++]))
				return nullptr;

			size_t len;
			for (len = 0; len < 32; ++len, ++index)
			{
				auto ch = in[index];
				if (ch == '\0' || !isWordChar(ch))
					break;
			}
			if (len == 32)
			{
				return nullptr;
			}

			return in + index;
		}

		const char * TextParser::ReadGateName(const char * in)
		{
			if (in == nullptr)
			{
				return nullptr;
			}

			size_t index = 0;

			if (in[index] == '\0')
			{
				return nullptr;
			}

			// first character = letter
			if (!isalpha(in[index++]))
				return nullptr;

			size_t len;
			for (len = 0; len < 32; ++len, ++index)
			{
				auto ch = in[index];
				if (ch == '\0' || !isWordChar(ch))
					break;
			}
			if (len == 32)
			{
				return nullptr;
			}

			return in + index;
		}

		const char * TextParser::SkipSpaces(const char * in)
		{
			const char * pos = in;
			while (*pos && isspace(*pos))
			{
				++pos;
			}
			return pos;
		}
	}
}