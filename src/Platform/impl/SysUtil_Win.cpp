#include "Platform.h"
#ifdef RAINBOW_WIN

#include <windows.h>

#include "Platform/SysUtil.h"

namespace Rainbow
{
	namespace SysUtil
	{
		bool HasGetUserDefaultLocaleName()
		{
			return !!GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetUserDefaultLocaleName");
		}

		bool has_accelerometer()
		{
			return false;
		}

		bool has_touchscreen()
		{
			return false;
		}

		void locales(Vector<char*> &locales)
		{
			char *locale_id;
			if (!HasGetUserDefaultLocaleName())  // Windows 98, XP
			{
				LCID locale = GetUserDefaultLCID();
				char lpLCData[19];
				int length = GetLocaleInfo(locale, LOCALE_SISO639LANGNAME, lpLCData, 9);
				lpLCData[length++] = '-';
				length += GetLocaleInfo(locale, LOCALE_SISO3166CTRYNAME, lpLCData + length, 9);
				locale_id = new char[length + 1];
				memcpy(locale_id, lpLCData, length + 1);
			}
			else  // Windows Vista and later
			{
				wchar_t lpLocaleName[LOCALE_NAME_MAX_LENGTH];
				int size = GetUserDefaultLocaleName(lpLocaleName, LOCALE_NAME_MAX_LENGTH);
				char locale[LOCALE_NAME_MAX_LENGTH << 1];
				wcstombs(locale, lpLocaleName, sizeof(locale));
				size_t length = strlen(locale);
				locale_id = new char[length + 1];
				memcpy(locale_id, locale, length + 1);
			}
			locales.push_back(locale_id);
		}

		size_t memory()
		{
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);
			return status.ullTotalPhys / (1024 * 1024);
		}
	}
}

#endif
