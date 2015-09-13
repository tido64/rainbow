// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <string>

#include <catch.hpp>

#include "Common/Algorithm.h"
#include "Config.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"

namespace
{
	constexpr const char kConfigTestPath[] =
	    "rainbow-test-random-path-to-avoid-existing-config-files";

	constexpr const char kAlternateConfig[] =
	    "accelerometer = true;\n"
	    "allow_high_dpi = false;\n"
	    "msaa = 6;\n"
	    "resolution = {750,1334};\n"
	    "suspend_on_focus_lost = true;\n";

	constexpr const char kInvalidConfig[] = "@";

	constexpr const char kSparseConfig[] =
	    "allow_high_dpi = true;\n"
	    "msaa = 10;\n"
	    "suspend_on_focus_lost = false;\n";

	constexpr const char kStandardConfig[] =
	    "accelerometer = false;\n"
	    "allow_high_dpi = true;\n"
	    "msaa = 4;\n"
	    "resolution = {1920,1080};\n"
	    "suspend_on_focus_lost = false;\n";

	class ScopedCurrentDirectory
	{
	public:
		ScopedCurrentDirectory(const char *path)
		{
			working_dir_ = Path::current();
			Path::set_current(path);
		}

		~ScopedCurrentDirectory() { Path::set_current(working_dir_.c_str()); }

	private:
		std::string working_dir_;
	};

	class ScopedConfig
	{
	public:
		template<size_t N>
		ScopedConfig(const char (&config)[N]) : ScopedConfig(config, N - 1) {}
		~ScopedConfig() { remove(path_); }

	private:
		Path path_;
		ScopedCurrentDirectory working_directory_;

		ScopedConfig(const char *config, size_t length)
		    : path_(kConfigTestPath), working_directory_(kConfigTestPath)
		{
			path_.create();
			path_ += "config";
			FILE *fd = fopen(path_, "wb");
			REQUIRE(fd != nullptr);
			fwrite(config, sizeof(*config), length, fd);
			fclose(fd);
		}
	};
}

TEST_CASE("Default configuration", "[config]")
{
	std::unique_ptr<rainbow::Config> config;

	SECTION("No configuration")
	{
		ScopedCurrentDirectory test(kConfigTestPath);
		config.reset(new rainbow::Config());
	}

	SECTION("Empty configuration")
	{
		ScopedConfig conf("");
		config.reset(new rainbow::Config());
	}

	SECTION("Invalid configuration")
	{
		INFO("Lua syntax error \"unexpected symbol near '@'\" is expected");
		ScopedConfig conf(kInvalidConfig);
		config.reset(new rainbow::Config());
	}

	REQUIRE(config->width() == 0);
	REQUIRE(config->height() == 0);
	REQUIRE_FALSE(config->high_dpi());
	REQUIRE_FALSE(config->is_portrait());
	REQUIRE(config->msaa() == 0);
	REQUIRE(config->suspend());
}

TEST_CASE("Parse normal configuration", "[config]")
{
	ScopedConfig config(kStandardConfig);
	rainbow::Config c;

	REQUIRE(c.width() == 1920);
	REQUIRE(c.height() == 1080);
	REQUIRE(c.high_dpi());
	REQUIRE_FALSE(c.is_portrait());
	REQUIRE(c.msaa() == 4);
	REQUIRE_FALSE(c.needs_accelerometer());
	REQUIRE_FALSE(c.suspend());
}

TEST_CASE("Parse alternate configuration", "[config]")
{
	ScopedConfig config(kAlternateConfig);
	rainbow::Config c;

	REQUIRE(c.width() == 750);
	REQUIRE(c.height() == 1334);
	REQUIRE_FALSE(c.high_dpi());
	REQUIRE(c.is_portrait());
	REQUIRE(c.msaa() == 4);
	REQUIRE(c.needs_accelerometer());
	REQUIRE(c.suspend());
}

TEST_CASE("Parse sparse configuration", "[config]")
{
	ScopedConfig config(kSparseConfig);
	rainbow::Config c;

	REQUIRE(c.width() == 0);
	REQUIRE(c.height() == 0);
	REQUIRE(c.high_dpi());
	REQUIRE_FALSE(c.is_portrait());
	REQUIRE(c.msaa() == 8);
	REQUIRE_FALSE(c.suspend());
}
