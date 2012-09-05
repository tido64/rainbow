// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include <cstdlib>
#include <cstring>

#include "Config.h"
#include "Common/Data.h"

Config::Config() : accelerometer(true), width(0), height(0)
{
	Data config("config");
	if (!config)
		return;

	unsigned int length = 0;
	const char *line = config;
	for (const char *i = line; *i; ++i)
	{
		if (*i == '\n')
		{
			if (!length)
				continue;

			this->parse_accelerometer(line, length)
			|| this->parse_resolution(line, length);

			line = i + 1;
			length = 0;
		}
		else
			++length;
	}
}

bool Config::keycmp(const char *line, const char *key)
{
	if (strlen(line) < strlen(key))
		return false;
	for (; *key; ++key, ++line)
		if (*line != *key)
			return false;
	return true;
}

bool Config::parse_accelerometer(const char *line, const unsigned int length)
{
	static const char *key = "accelerometer";

	if (length < 16 || !keycmp(line, key))
		return false;

	this->accelerometer = (line[15] != '0');
	return true;
}

bool Config::parse_resolution(const char *line, const unsigned int length)
{
	static const char *key = "resolution";

	if (length < 19 || !keycmp(line, key))
		return false;
	line += 12;

	const char *h = line;
	for (; *h && *h != 'x'; ++h);
	if (*h != '\0' && *(++h) != '\0')
	{
		this->width = atoi(line);
		this->height = atoi(h);
	}
	return true;
}
