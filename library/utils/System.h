#pragma once

// Operating System
#ifdef _WIN32
#define TARGET_WINDOWS
#elif defined _DARWIN
#define TARGET_MACOSX
#elif defined _LINUX
#define TARGET_LINUX
#endif // _WIN32
