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

#include "ConsoleManager.h"

#include <Windows.h>

#include <conio.h>

ConsoleManager ConsoleManager::sInstance;

ConsoleManager::ConsoleManager()
	: mWaitForUser(false)
	, mOutFile(nullptr)
	, mErrFile(nullptr)
	, mInFile(nullptr)
{
}

ConsoleManager::~ConsoleManager()
{
	mWaitForUser = false;
	ReleaseConsole();
}

bool ConsoleManager::AttachOrCreateConsole()
{
	mWaitForUser = false;
	if (AttachConsole(ATTACH_PARENT_PROCESS) == FALSE)
	{
		DWORD error = GetLastError();
		if (error == ERROR_ACCESS_DENIED)
		{
			// Already attached
			return true;
		}

		if (AllocConsole() == FALSE)
		{
			// AllocConsole failed. We wont have a console, so don't try to redirect streams
			return false;
		}

		mWaitForUser = true;
	}

	freopen_s(&mOutFile, "CONOUT$", "w", stdout);
	freopen_s(&mErrFile, "CONOUT$", "w", stderr);
	freopen_s(&mInFile, "CONIN$", "r", stdin);

	return true;
}

void ConsoleManager::ReleaseConsole()
{
	if (mWaitForUser)
	{
		(void)_getch();
		mWaitForUser = false;
	}

	FreeFileHandle(mOutFile);
	FreeFileHandle(mErrFile);
	FreeFileHandle(mInFile);

	// Workaround for issue where console prompt does not reappear after exiting when attached to parent console
	HWND consoleHwnd = GetConsoleWindow();
	if (consoleHwnd != NULL)
	{
		SendMessageW(consoleHwnd, WM_CHAR, VK_RETURN, NULL);
		FreeConsole();
	}
}

bool ConsoleManager::IsAttachedToConsole() const
{
	return GetConsoleWindow() != NULL;
}

void ConsoleManager::FreeFileHandle(FILE*& file)
{
	if (file)
	{
		fclose(file);
		file = nullptr;
	}
}
