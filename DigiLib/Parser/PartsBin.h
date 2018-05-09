#pragma once
#include "Core\Common.h"
#include <map>

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib {
	namespace Parser {
		class DllExport PartsBin
		{
		public:
			using GateList = std::map<std::string, Core::GatePtr>;

			static PartsBinPtr Create();
			PartsBin(const PartsBin&) = delete;
			PartsBin& operator=(const PartsBin&) = delete;
			PartsBin(PartsBin&&) = delete;
			PartsBin& operator=(PartsBin&&) = delete;
			
			void AddPart(const char * name, Core::GatePtr gate);
			size_t GetPartCount() { return m_gates.size(); }
			void Clear() { m_gates.clear(); }

			Core::GatePtr Find(const char * name);

		private:
			PartsBin() = default;
			GateList m_gates;

			struct shared_enabler;
		};
	}
}