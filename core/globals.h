#pragma once

#ifdef __EMSCRIPTEN__
#define EXPORT
#else
#ifdef SGE_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif
#endif

#define PAK_VERSION 1
#define RND_BUFSIZE 512 * 6

#define Bind0(func) std::bind(func, &this);
#define Bind1(func) std::bind(func, &this, std::placeholder::_1);
#define Bind2(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2);
#define Bind3(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3);
#define Bind4(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3, std::placeholder::_4);
#define Bind5(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3, std::placeholder::_4, std::placeholder::_5);
#define Bind6(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3, std::placeholder::_4, std::placeholder::_5, std::placeholder::_6);
#define Bind7(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3, std::placeholder::_4, std::placeholder::_5, std::placeholder::_6, std::placeholder::_7);
#define Bind8(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3, std::placeholder::_4, std::placeholder::_5, std::placeholder::_6, std::placeholder::_7, std::placeholder::_8);
#define Bind9(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3, std::placeholder::_4, std::placeholder::_5, std::placeholder::_6, std::placeholder::_7, std::placeholder::_8, std::placeholder::_9);
#define Bind10(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3, std::placeholder::_4, std::placeholder::_5, std::placeholder::_6, std::placeholder::_7, std::placeholder::_8, std::placeholder::_9, std::placeholder::_10);
#define Bind11(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3, std::placeholder::_4, std::placeholder::_5, std::placeholder::_6, std::placeholder::_7, std::placeholder::_8, std::placeholder::_9, std::placeholder::_10, std::placeholder::_11);
#define Bind12(func) std::bind(func, &this, std::placeholder::_1, std::placeholder::_2, std::placeholder::_3, std::placeholder::_4, std::placeholder::_5, std::placeholder::_6, std::placeholder::_7, std::placeholder::_8, std::placeholder::_9, std::placeholder::_10, std::placeholder::_11, std::placeholder::_12);