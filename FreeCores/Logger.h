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

struct Configuration;

/// Utility for logging output messages to a console
class Logger
{
public:
	Logger(const Configuration& config);

	/// Logs a message to a console. Will attempt to attach to or create
	/// a console if necessary.
	/// @param fmt  The message format to be passed along to printf
	/// @param ...  Args to be passed along to printf
	void LogMessage(const wchar_t* fmt, ...) const;

	/// Logs an error message to a console. Will attempt to attach to or
	/// create a console if necessary.
	/// @param fmt  The message format to be passed along to printf
	/// @param ...  Args to be passed along to printf
	void LogError(const wchar_t* fmt, ...) const;

	/// Logs an error message to a console. Will attempt to attach to or
	/// create a console if necessary.
	/// @param code  An error code obtained from GetLastError()
	/// @param fmt   The message format to be passed along to printf
	/// @param ...   Args to be passed along to printf
	void LogError(int code, const wchar_t* fmt, ...) const;

private:
	const Configuration& mConfig;

public:
	Logger() = delete;
	Logger(const Logger&) = default;
	~Logger() = default;
	Logger& operator=(const Logger&) = default;
};
