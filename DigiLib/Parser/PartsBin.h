#pragma once
#include "Core/Common.h"
#include <map>

namespace DigiLib {
	namespace Parser {
		class DllExport PartsBin : public std::enable_shared_from_this<PartsBin>
		{
		public:
			using GateList = std::map<std::string, Core::GatePtr>;
			using PartFinderFunc = Core::GatePtr(*)(PartsBinPtr parts, const char* partName);

			static PartsBinPtr Create();
			PartsBin(const PartsBin&) = delete;
			PartsBin& operator=(const PartsBin&) = delete;
			PartsBin(PartsBin&&) = delete;
			PartsBin& operator=(PartsBin&&) = delete;
			
			void AddPart(const char * name, Core::GatePtr gate);
			size_t GetPartCount() { return m_gates.size(); }
			void Clear() { m_gates.clear(); }

			void SetPartFinder(PartFinderFunc f) { m_partFinderFunc = f; }

			bool HasGate(const char * name);
			Core::GatePtr Find(const char * name);

		private:
			PartsBin() = default;
			GateList m_gates;
			PartFinderFunc m_partFinderFunc;

			struct shared_enabler;
		};
	}
}