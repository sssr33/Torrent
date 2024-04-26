#include "BuildHelpers.h"

namespace BuildHelpers {
	std::wstring ToDefaultString(BuildArch arch) {
		switch (arch) {
		case BuildArch::Win32:
			return L"Win32";
		case BuildArch::x64:
			return L"x64";
		}

		return {};
	}

	std::wstring ToDefaultString(BuildConfig config) {
		switch (config) {
		case BuildConfig::Debug:
			return L"Debug";
		case BuildConfig::Release:
			return L"Release";
		}

		return {};
	}
}
