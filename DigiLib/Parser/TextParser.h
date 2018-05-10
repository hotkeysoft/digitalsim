#pragma once
#include "Core\Common.h"
#include <list>
#include <vector>
#include <sstream>

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib {
	namespace Parser {
		class DllExport TextParser
		{
		public:
			struct Section { std::string Name;  std::string Data; };
			using Sections = std::vector<Section>;
			using SectionElement = std::list<std::string>;
			using PinDefType = std::tuple<std::string, size_t>;
			using PartDefType = std::tuple<std::string, std::string>;
			using SectionDefType = std::tuple<std::string, bool>;
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