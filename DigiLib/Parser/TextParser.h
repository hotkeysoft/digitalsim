#pragma once
#include "Core\Common.h"
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
			using SectionType = std::vector<std::string>;
			using PinDefType = std::tuple<std::string, size_t>;

			void Attach(Core::CompositeGatePtr gate)
			{
				m_gate = gate;
			}

			Core::CompositeGatePtr Get() { return m_gate;  }

			void ParseConnection(const char *);
			void ParseWireSection(const char *);
			void ParseInputsSection(const char *);
			void ParseOutputsSection(const char *);
			
		protected:
			static SectionType ParseSection(const char *in);
			static PinDefType ExtractPinDef(const std::string & in);

		private:
			Core::CompositeGatePtr m_gate;
		};
	}
}