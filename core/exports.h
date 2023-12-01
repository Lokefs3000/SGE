#pragma once

#ifdef SGE_EXPORTS
#define SGE __declspec(dllexport)
#else
#define SGE __declspec(dllimport)
#endif

#define EVENT_HANDLER_BIND1(function, pointer) std::bind(&function, pointer, std::placeholders::_1)

#ifdef _DEBUG
#define ASSERT(condition, value, returnval) \
								if (!(condition)) { \
									std::cout << value << std::endl; \
									return returnval; \
								}
#else
#define ASSERT(condition, value, returnval)
#endif