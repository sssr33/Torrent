#pragma once

#include <memory>

template<class Fn>
struct ScopeDeleter {
	explicit ScopeDeleter(Fn&& fn)
		: fn(std::forward<Fn>(fn))
	{}

	void operator()(void*) {
		fn();
	}

	Fn fn;
};

template<class Fn>
using Scope = std::unique_ptr<void, ScopeDeleter<Fn>>;

template<class Fn>
Scope<Fn> MakeScope(Fn&& fn) {
	// reinterpret_cast<void*>(1) so the unique_ptr will call its deleter in destructor
	return Scope<Fn>(reinterpret_cast<void*>(1), ScopeDeleter<Fn>(std::forward<Fn>(fn)));
}
