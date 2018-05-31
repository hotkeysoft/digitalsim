#pragma once
#include <set>
#include "Common.h"
#include "IOPin.h"
#include "IOState.h"

namespace DigiLib
{
	namespace Core
	{
		class GateBase;

		class IOPinSubset : public IOPin
		{
		public:
			enum IO_DIRECTION { INPUT, OUTPUT, OUTPUT_HI_Z };

			IOPinSubset(IOPinRef parentPin, size_t offset);
			IOPinSubset(IOPinRef parentPin, size_t low, size_t hi);

			IOPinPtr Clone(GateRef cloneParent) override;

			std::string GetName() override;
			IOPinRef GetParent() noexcept { return m_parentPin; }
			IOState GetMask() noexcept override;
			bool Overlaps(IOState) override;
			
			IOState Get() noexcept override;
			void Set(IOState state) override;

		protected:
			size_t GetOffset() override { return m_low; }

			size_t m_low;
			size_t m_hi;
			IOPinRef m_parentPin;
		};
	}
}