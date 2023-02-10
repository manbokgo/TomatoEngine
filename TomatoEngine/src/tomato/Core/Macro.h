#pragma once

#ifdef NDEBUG
# define ASSERT(e)    ( __assume(e) )
#else
# include <cassert>
# define ASSERT(e)    ( assert(e) )
#endif

#if __cplusplus >= 201703L // C++17
#define FALLTHROUGH [[fallthrough]];
#else
#define FALLTHROUGH /* FALLTHROUGH */
#endif

#define BIT(x) (1 << x)

#define TOMATO_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define DT CTimeMgr::GetInst()->GetDeltaTime()

#define DEVICE Device::GetInst()->GetDevice()
#define CONTEXT Device::GetInst()->GetContext()

#define KEY_PRESSED(key)	CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::PRESS
#define KEY_TAP(key)		CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::TAP
#define KEY_RELEASE(key)	CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::RELEASE

#define CLONE(type) public: type* Clone() { return new type(*this); }
#define CLONE_DUMMY(type) public: type* Clone() { return nullptr; }
#define CLONE_ASSERT(type) public: type* Clone() { assert(nullptr); return nullptr; }
