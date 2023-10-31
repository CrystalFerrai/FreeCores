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

#include "Configuration.h"

#include <Windows.h>

Configuration::Configuration()
	: FreeCount(0)
	, PID(0)
	, ProcessName(nullptr)
	, IsQuiet(false)
	, IsVerbose(false)
	, SilentErrors(false)
{
}

bool ParseConfiguration(Configuration* outConfig)
{
	bool success = true;

	int posArg = 0;
	for (int i = 1; i < __argc; ++i)
	{
		if (__wargv[i][0] == L'-' || __wargv[i][0] == L'/')
		{
			switch (__wargv[i][1])
			{
			case L'Q':
			case L'q':
				if (!outConfig->IsVerbose) outConfig->IsQuiet = true;
				break;
			case L'V':
			case L'v':
				outConfig->IsVerbose = true;
				outConfig->IsQuiet = false;
				break;
			case L'E':
			case L'e':
				outConfig->SilentErrors = true;
				break;
			default:
				success = false;
				break;
			}
		}
		else
		{
			switch (posArg)
			{
			case 0:
				wchar_t* end;
				outConfig->FreeCount = wcstol(__wargv[i], &end, 10);
				if (*end) success = false;
				break;
			case 1:
				outConfig->PID = _wtoi(__wargv[i]);
				if (outConfig->PID == 0)
				{
					outConfig->ProcessName = __wargv[i];
				}
				break;
			}
			++posArg;
		}
	}

	if (posArg != 2) success = false;

	return success;
}
