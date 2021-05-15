#pragma once

#include "Base/Windows/Common.h"

#include <boost/filesystem.hpp>

namespace base
{
	namespace win
	{
		namespace fs = boost::filesystem;

		class Module
		{
		public:
			static Module Get(const char* moduleName);

			Module();
			explicit Module(HMODULE module);
			~Module();

			fs::path getPath() const;
			fs::path getDir() const;

			FARPROC getProcAddress(const char* procName) const;

			template <typename T>
			T getProcAddress(const char* procName) const
			{
				return reinterpret_cast<T>(getProcAddress(procName));
			}

			bool operator ==(nullptr_t) const;
			bool operator !=(nullptr_t) const;
			bool operator !() const;
			operator bool() const;
			operator HMODULE() const;

		protected:
			static constexpr DWORD BUFFER_SIZE = 2 * 1024;

			HMODULE m_module;
		};
	}
}
