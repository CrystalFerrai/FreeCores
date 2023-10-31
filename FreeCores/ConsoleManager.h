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
#pragma once

#include <cstdio>

/// Manages attaching/creating a console for the application
class ConsoleManager
{
public:
	/// Returns the singleton instance of the console manager
	static inline ConsoleManager& GetInstance() { return sInstance; }

	/// Attempts to attach to the console window of the parent process. If unavailable,
	/// will create a new console window instead. Also redirects stdout, stderr, and
	/// stdin to the console window.
	/// @return true if a console is attached, else false
	bool AttachOrCreateConsole();

	/// Frees a console that was obtained by calling AttachOrCreateConsole
	void ReleaseConsole();

	/// Returns whether this process is currently attached to a console
	bool IsAttachedToConsole() const;

private:
	ConsoleManager();
	~ConsoleManager();

	static void FreeFileHandle(FILE*& file);

	static ConsoleManager sInstance;

	bool mWaitForUser;

	FILE* mOutFile;
	FILE* mErrFile;
	FILE* mInFile;

private:
	// Singleton
	ConsoleManager(const ConsoleManager&) = delete;
	ConsoleManager& operator=(const ConsoleManager&) = delete;
};
