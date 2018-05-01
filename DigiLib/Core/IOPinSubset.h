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

			IOPinSubset(IOPin* parentPin, size_t offset);
			IOPinSubset(IOPin* parentPin, size_t low, size_t hi);

			IOPin* Clone(GateBase *cloneParent) override;

			std::string GetName() override;

			IOPin* GetParent() noexcept { return m_parentPin; }

			virtual size_t GetWidth() { return m_width; }
			IOState Get() noexcept override;
			void Set(IOState state) override;

		protected:
			size_t GetOffset() override { return m_low; }

			size_t m_low;
			size_t m_hi;
			size_t m_width;
			IOPin * m_parentPin;
		};
	}
}