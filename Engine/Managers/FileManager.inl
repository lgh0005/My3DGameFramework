#pragma once

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#endif

inline fs::path FileManager::GetExecutablePath()
{
/*=================================================//
//   absolute file directory getter (for windows)  //
//=================================================*/
#if defined(_WIN32)
	wchar_t buffer[MAX_PATH];
	if (GetModuleFileNameW(nullptr, buffer, MAX_PATH) == 0) return fs::current_path();
	return fs::path(buffer).parent_path();

/*===============================================//
//   absolute file directory getter (for linux)  //
//===============================================*/
#elif defined(__linux__)
	char buffer[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (len == -1) return fs::current_path();
	buffer[len] = '\0';
	return fs::path(buffer).parent_path();

#else
	return fs::current_path();

#endif
}