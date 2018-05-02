#include "stdafx.h"
#include "IOConnection.h"
#include "IOPin.h"

namespace DigiLib
{
	namespace Core
	{
		IOConnection::IOConnection(IOPinPtr source, IOPinPtr target) : m_source(source), m_target(target)
		{
		}

		bool IOConnection::operator<(const IOConnection & rhs) const noexcept
		{
			// TODO : Sort by fully qualified name?
			if (m_source == rhs.m_source)
			{
				return m_target < rhs.m_target;
			}

			return m_source < rhs.m_source;
		}
	}
}