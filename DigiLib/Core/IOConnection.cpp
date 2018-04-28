#include "stdafx.h"
#include "IOConnection.h"
#include "IOPin.h"

namespace DigiLib
{
	namespace Core
	{
		IOConnection::IOConnection(IOPin * source, IOPin * target) : m_source(source), m_target(target)
		{
			if (source == nullptr || target == nullptr)
			{
				throw std::invalid_argument("source or target is null");
			}
		}

		bool IOConnection::operator==(const IOConnection & rhs) const
		{
			return this->m_source == rhs.m_source &&
				this->m_target == rhs.m_target;
		}

		bool IOConnection::operator<(const IOConnection & rhs) const
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