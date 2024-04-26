#pragma once
#include "BuildArch.h"
#include "BuildConfig.h"

#include <string>
#include <functional>

namespace BuildHelpers {
	std::wstring ToDefaultString(BuildArch arch);
	std::wstring ToDefaultString(BuildConfig config);

	template<class FnT = std::function<void(BuildArch, BuildConfig)>>
	void ForAllArchConfig(FnT fn) {
		for (auto arch = static_cast<std::underlying_type_t<BuildArch>>(BuildArch::Unknown) + 1;
			arch < static_cast<std::underlying_type_t<BuildArch>>(BuildArch::Count);
			++arch)
		{
			for (auto config = static_cast<std::underlying_type_t<BuildConfig>>(BuildConfig::Unknown) + 1;
				config < static_cast<std::underlying_type_t<BuildConfig>>(BuildConfig::Count);
				++config)
			{
				fn(static_cast<BuildArch>(arch), static_cast<BuildConfig>(config));
			}
		}
	}

	template<class FnT = std::function<void(BuildConfig)>>
	void ForAllConfig(FnT fn) {
		for (auto config = static_cast<std::underlying_type_t<BuildConfig>>(BuildConfig::Unknown) + 1;
			config < static_cast<std::underlying_type_t<BuildConfig>>(BuildConfig::Count);
			++config)
		{
			fn(static_cast<BuildConfig>(config));
		}
	}
};
