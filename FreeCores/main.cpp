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

#include <Windows.h>

#include <Psapi.h>

#include "Configuration.h"
#include "ConsoleManager.h"
#include "Logger.h"

void PrintUsage();
bool SetFreeCores(const Configuration& config, const Logger& logger);

/// Program entry point
int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance, _In_ PWSTR cmdLine, _In_ int cmdShow)
{
	Configuration config;
	if (!ParseConfiguration(&config) || config.FreeCount < 0)
	{
		PrintUsage();
		return __argc == 1 ? 0 : 1;
	}

	Logger logger(config);

	bool success = SetFreeCores(config, logger);

	// A console may have been created if any output was necessary.
	ConsoleManager::GetInstance().ReleaseConsole();

	return success ? 0 : 1;
}

/// Prints program usage instructions
void PrintUsage()
{
	ConsoleManager::GetInstance().AttachOrCreateConsole();

	wprintf(
		L"FreeCores: Sets CPU affinity for running processes to exclude a number of\n"
		L"           physical cores starting from core 0.\n"
		L"\n"
		L"Usage: FreeCores [-q -v] [count] [processes]\n"
		L"\n"
		L"  count      The number of physical cores to remove from the affinity of the\n"
		L"             processes. This will be translated to logical cores automatically.\n"
		L"             Must be a positive integer less than physical core count minus one.\n"
		L"\n"
		L"  processes  List of process names or pids to update, space-delimited. Use\n"
		L"             quotes around any names which contain a space. If a pid is passed,\n"
		L"             only that specific process will be updated. If a name is passed,\n"
		L"             all matching processes (case-insensitive) will be updated.\n"
		L"\n"
		L"Options\n"
		L"\n"
		L"  -q    Quiet mode. Do not print any output.\n"
		L"\n"
		L"  -v    Verbose mode. Prints additional information. Overrides -q.\n"
		L"\n"
		L"  -e    Silent errors. Do not print error messages.\n"
	);
}

/// Main program logic. Updates process affinities based on program parameters.
bool SetFreeCores(const Configuration& config, const Logger& logger)
{
	// Build an affinity mask including all logical CPUs except those associated with the first N physical cores
	DWORD affinityMask = 0;
	{
		DWORD bufferSize = 0;

		// The first call fails but sets the buffer size
		GetLogicalProcessorInformation(NULL, &bufferSize);

		int infoCount = bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		auto* infoBuffer = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION[infoCount];

		if (GetLogicalProcessorInformation(infoBuffer, &bufferSize) == FALSE)
		{
			DWORD errorCode = GetLastError();
			logger.LogError(errorCode, L"Failed to get CPU information.");

			delete[] infoBuffer;
			return false;
		}

		int pCore = 0;
		for (int i = 0; i < infoCount; ++i)
		{
			if (infoBuffer[i].Relationship == RelationProcessorCore)
			{
				++pCore;
				if (pCore <= config.FreeCount)
				{
					continue;
				}

				affinityMask |= infoBuffer[i].ProcessorMask;
			}
		}

		if (config.IsVerbose)
		{
			logger.LogMessage(L"Detected %d physical cores.", pCore);
		}

		delete[] infoBuffer;
	}

	if (affinityMask == 0)
	{
		logger.LogError(L"Cannot set process affinity to 0 cores. Decrease free core count.");
		return false;
	}

	// If the passed in arg is a pid, set affinity for that pid an exit
	if (config.PID != 0)
	{
		HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, config.PID);
		if (hProcess != NULL)
		{
			if (SetProcessAffinityMask(hProcess, affinityMask) == FALSE)
			{
				DWORD errorCode = GetLastError();

				logger.LogError(errorCode, L"Error setting affinity for process %d.", config.PID);

				CloseHandle(hProcess);
				return false;
			}
			else
			{
				logger.LogMessage(L"Set affinity for process with pid %d", config.PID);
			}

			CloseHandle(hProcess);
			return true;
		}
	}

	// If the passed in arg is not a pid, treat it as a process name, find matching processes and set affinity on all of them
	// Process name matching is case-insensitive.
	DWORD processes[1024], bytesOut, processCount;
	if (!EnumProcesses(processes, sizeof(processes), &bytesOut))
	{
		DWORD errorCode = GetLastError();
		logger.LogError(errorCode, L"Error enumerating processes");
		return false;
	}
	processCount = bytesOut / sizeof(DWORD);

	int processFoundCount = 0;
	for (DWORD i = 0; i < processCount; ++i)
	{
		if (processes[i] != 0u)
		{
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_SET_INFORMATION, FALSE, processes[i]);
			if (hProcess == NULL) continue;

			HMODULE hMod;
			DWORD bytesOut;
			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &bytesOut))
			{
				WCHAR processName[MAX_PATH] = {};
				GetModuleBaseNameW(hProcess, hMod, processName, sizeof(processName) / sizeof(WCHAR));

				if (_wcsicmp(processName, config.ProcessName) == 0)
				{
					++processFoundCount;
					if (SetProcessAffinityMask(hProcess, affinityMask) == FALSE)
					{
						DWORD errorCode = GetLastError();
						logger.LogError(errorCode, L"Error setting affinity for process '%s' with pid %d", processName, processes[i]);

						CloseHandle(hProcess);
						return false;
					}
					else
					{
						logger.LogMessage(L"Set affinity for process '%s' with pid %d", processName, processes[i]);
					}
				}
			}

			CloseHandle(hProcess);
		}
	}

	if (processFoundCount == 0)
	{
		logger.LogError(L"Failed to find any process matching the pid or name %s\n", config.ProcessName);

		return false;
	}

	return true;
}
