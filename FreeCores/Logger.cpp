// Copyright 2023 Crystal Ferrai
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Logger.h"

#include <Windows.h>

#include <cstdarg>
#include <cstdio>
#include <malloc.h>

#include "Configuration.h"
#include "ConsoleManager.h"

Logger::Logger(const Configuration& config)
	: mConfig(config)
{
}

void Logger::LogMessage(const wchar_t* fmt, ...) const
{
	if (mConfig.IsQuiet && !mConfig.IsVerbose) return;

	ConsoleManager::GetInstance().AttachOrCreateConsole();

	va_list args;
	va_start(args, fmt);
	vfwprintf(stdout, fmt, args);
	va_end(args);

	fwprintf(stdout, L"\n");
}

void Logger::LogError(const wchar_t* fmt, ...) const
{
	if (mConfig.IsQuiet && !mConfig.IsVerbose || mConfig.SilentErrors) return;

	ConsoleManager::GetInstance().AttachOrCreateConsole();

	va_list args;
	va_start(args, fmt);
	vfwprintf(stderr, fmt, args);
	va_end(args);

	fwprintf(stdout, L"\n");
}

void Logger::LogError(int code, const wchar_t* fmt, ...) const
{
	if (mConfig.IsQuiet && !mConfig.IsVerbose || mConfig.SilentErrors) return;

	ConsoleManager::GetInstance().AttachOrCreateConsole();

	va_list args;
	va_start(args, fmt);
	vfwprintf(stderr, fmt, args);
	va_end(args);

	fwprintf(stdout, L"\n");

	LPVOID buffer;

	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&buffer,
		0,
		NULL);

	fwprintf(stderr, (LPWSTR)buffer);

	LocalFree(buffer);
}
