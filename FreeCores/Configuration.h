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

/// Contains program configuration data
struct Configuration
{
	int FreeCount;
	int PID;
	const wchar_t* ProcessName;

	bool IsQuiet;
	bool IsVerbose;
	bool SilentErrors;

	Configuration();

	Configuration(const Configuration&) = default;
	~Configuration() = default;
	Configuration& operator=(const Configuration&) = default;
};

/// Parses the program command line to create a configuration
/// @param outConfig  Pointer to a Configuration to populate
/// @return true if the command line was successfully parsed and the
///         configuration populated. If false, the configuration will
///         not be in a valid state and should not be used.
bool ParseConfiguration(Configuration* outConfig);
