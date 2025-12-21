#pragma once

#ifndef DISABLE_LOG
#define LOG(x) std::cout << "[LOG] " << x << std::endl
#endif

#ifndef DISABLE_ERROR_LOG
#define ERROR(x) std::cout << "[ERROR] " << x << std::endl
#endif