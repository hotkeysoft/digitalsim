#pragma once
#include "Core/Common.h"
#include <list>
#include <vector>
#include <sstream>

namespace DigiLib {
	namespace Parser {
		class DllExport TextParser
		{
		public:
			struct SectionDefType { std::string name; bool mandatory; };
			struct Section { std::string name;  std::string data; };
			using Sections = std::vector<Section>;
			using SectionElement = std::list<std::string>;
			using PinDefType = std::tuple<std::string, size_t>;
			using PartDefType = std::tuple<std::string, std::string>;
			using SectionDef = std::list<SectionDefType>;

			void Attach(Core::CompositeGatePtr gate, PartsBinPtr parts = nullptr)
			{
				m_gate = gate;
				m_parts = parts;
			}

			Core::CompositeGatePtr Get() { return m_gate; }

			void LoadFromFile(const char*);

			void ParseGate(const char*);
			void ParseSection(const DigiLib::Parser::TextParser::Section & section);
			void ParseConnection(const char *);
			void ParsePartsSection(const char *);
			void ParseWireSection(const char *);
			void ParseInputsSection(const char *);
			void ParseOutputsSection(const char *);

			Sections GetSections(const char*);

			static const char * SkipSpaces(const char * in);
			static const char * ReadGateName(const char * in);
			static const char * ReadPinName(const char * in);
			static const char * ReadPinNumber(const char * in, size_t & out);
			static const char * ExtractGateName(const char * in, char * out, size_t outSize);
			static const char * ExtractPinName(const char * in, char * out, size_t outSize);
			static const char * ExtractPinRange(const char * in, size_t & pinLow, size_t & pinHi);
			static const char * ExtractPinSize(const char * in, size_t & size);

		protected:
			static SectionElement ParseSection(const char *in);
			static PinDefType ExtractPinDef(const std::string & in);
			static PartDefType ExtractPartDef(const std::string & in);
			static Section ExtractSection(const std::string & in);
			static std::string RemoveComments(const std::string & in);

		private:
			Core::CompositeGatePtr m_gate;
			PartsBinPtr m_parts;

			static SectionDef m_sectionNames;
		};
	}
}