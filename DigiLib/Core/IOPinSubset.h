#pragma once
#include <set>
#include "IOPin.h"
#include "IOState.h"

#ifdef  DIGILIB_EXPORTS 
/*Enabled as "export" while compiling the dll project*/
#define DllExport __declspec(dllexport)  
#else
/*Enabled as "import" in the Client side for using already created dll file*/
#define DllExport __declspec(dllimport)  
#endif

namespace DigiLib
{
	namespace Core
	{
		class GateBase;

		class IOPinSubset : public IOPin
		{
		public:
			enum IO_DIRECTION { INPUT, OUTPUT, OUTPUT_HI_Z };

			IOPinSubset(IOPinPtr parentPin, size_t offset);
			IOPinSubset(IOPinPtr parentPin, size_t low, size_t hi);

			IOPinPtr Clone(GateBase *cloneParent) override;

			std::string GetName() override;

			IOPinPtr GetParent() noexcept { return m_parentPin; }

			IOState Get() noexcept override;
			void Set(IOState state) override;

		protected:
			size_t GetOffset() override { return m_low; }

			size_t m_low;
			size_t m_hi;
			IOPinPtr m_parentPin;
		};
	}
}