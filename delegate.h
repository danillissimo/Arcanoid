#pragma once

#ifndef forceinline
#ifdef _MSC_VER
#define forceinline __forceinline
#else
#define forceinline __attribute__((always_inline))
#endif
#endif

#pragma push_macro(stdcall)
#ifdef _MSC_VER
#define stdcall __stdcall
#else
#define stdcall __attribute__((stdcall))
#endif

#pragma push_macro(cdecl)
#ifdef _MSC_VER
#define cdecl __cdecl
#else
#define cdecl __attribute__((cdecl))
#endif


#pragma push_macro(fastcall)
#ifdef _MSC_VER
#define fastcall __fastcall
#else
#define fastcall __attribute__((fastcall))
#endif

#pragma push_macro(thiscall)
#ifdef _MSC_VER
#define thiscall __thiscall
#else
#define thiscall __attribute__((thiscall))
#endif

#pragma push_macro(vectorcall)
#ifdef _MSC_VER
#define vectorcall __vectorcall
#else
#define thiscall __attribute__((vectorcall))
#endif

#if _MANAGED
#pragma push_macro(clrcall)
#define clrcall __clrcall
#endif

//__pascal
//__fortran
//__syscall

//=================================================================================================

#define DELEGATE_INVOKE_BY_REFERENCE 1

#if _MANAGED
#define IMPLEMENT_ALL IMPLEMENT(stdcall) IMPLEMENT(cdecl) IMPLEMENT(fastcall) IMPLEMENT(thiscall) IMPLEMENT(vectorcall) IMPLEMENT(clrcall)
#else
#define IMPLEMENT_ALL IMPLEMENT(stdcall) IMPLEMENT(cdecl) IMPLEMENT(fastcall) IMPLEMENT(thiscall) IMPLEMENT(vectorcall)
#endif

//=================================================================================================

template<typename ReturnType, typename ...FunctionArguments>
struct UniversalDelegate {
private:
	class Dummy {};

#if DELEGATE_INVOKE_BY_REFERENCE == 1
#define IMPLEMENT(conventionName)\
	ReturnType cc_##conventionName(FunctionArguments... args){\
		if (entity != nullptr) return (*entity.*(method.member_##conventionName))(args...);\
		return (*(method.static_##conventionName))(args...);\
	}

	IMPLEMENT_ALL;
#else
#define IMPLEMENT(conventionName) cc_##conventionName,
	enum CallingConvetion : char {
		IMPLEMENT_ALL
	};
#endif
#undef IMPLEMENT

#define IMPLEMENT(conventionName)\
	ReturnType(conventionName Dummy::*member_##conventionName)(FunctionArguments...);\
	ReturnType(conventionName        *static_##conventionName)(FunctionArguments...);


	Dummy* entity;
	union {
		IMPLEMENT_ALL;
	} method;
#if DELEGATE_INVOKE_BY_REFERENCE == 1
	ReturnType(UniversalDelegate<ReturnType, FunctionArguments...>::* caller)(FunctionArguments...);
#define UPDATE_SELECTOR(value) caller = &UniversalDelegate<ReturnType, FunctionArguments...>::value
#else
	CallingConvetion convention;
#define UPDATE_SELECTOR(value) convention = value
#endif

public:

#undef IMPLEMENT
#define IMPLEMENT(conventionName)\
	template<typename T>\
	void assign(const T& object, ReturnType(conventionName T::* function)(FunctionArguments...)) {\
		entity = (Dummy*)&object;\
		method.member_##conventionName = (ReturnType(conventionName Dummy::*)(FunctionArguments...))(function);\
		UPDATE_SELECTOR(cc_##conventionName);\
	}\
\
	void assign(ReturnType(conventionName * function)(FunctionArguments...)) {\
		entity = nullptr;\
		method.static_##conventionName = function;\
		UPDATE_SELECTOR(cc_##conventionName);\
	}

	IMPLEMENT_ALL;
#undef IMPLEMENT

#if DELEGATE_INVOKE_BY_REFERENCE == 1
	inline ReturnType invoke(FunctionArguments... args) const {
		return (*this.*(caller))(args...);
	}
#else
#define IMPLEMENT(conventionName) case cc_##conventionName: return (*entity.*(method.member_##conventionName))(args...);
	ReturnType invoke(FunctionArguments... args) const {
		if (entity != nullptr) {
			switch (convention)
			{
				IMPLEMENT_ALL;
			}
		}
		else {
#undef IMPLEMENT
#define IMPLEMENT(conventionName) case cc_##conventionName: return (*(method.static_##conventionName))(args...);
			switch (convention)
			{
				IMPLEMENT_ALL;
			}
		}
	}
#endif
};

//=================================================================================================

template<typename ReturnType, typename ...FunctionArguments>
struct Delegate {
private:
	class Dummy {};

#if DELEGATE_INVOKE_BY_REFERENCE == 1
#define IMPLEMENT(conventionName)\
	ReturnType cc_##conventionName(FunctionArguments... args){\
		return (*entity.*(method.member_##conventionName))(args...);\
	}

	IMPLEMENT_ALL;
#else
#define IMPLEMENT(conventionName) cc_##conventionName,
	enum CallingConvetion : char {
		IMPLEMENT_ALL
	};
#endif
#undef IMPLEMENT

#define IMPLEMENT(conventionName)\
	ReturnType(conventionName Dummy::*member_##conventionName)(FunctionArguments...);


	Dummy* entity;
	union {
		IMPLEMENT_ALL;
	} method;
#if DELEGATE_INVOKE_BY_REFERENCE == 1
	ReturnType(UniversalDelegate<ReturnType, FunctionArguments...>::* caller)(FunctionArguments...);
#define UPDATE_SELECTOR(value) caller = &UniversalDelegate<ReturnType, FunctionArguments...>::value
#else
	CallingConvetion convention;
#define UPDATE_SELECTOR(value) convention = value
#endif

public:

#undef IMPLEMENT
#define IMPLEMENT(conventionName)\
	template<typename T>\
	void assign(const T& object, ReturnType(conventionName T::* function)(FunctionArguments...)) {\
		entity = (Dummy*)&object;\
		method.member_##conventionName = (ReturnType(conventionName Dummy::*)(FunctionArguments...))(function);\
		UPDATE_SELECTOR(cc_##conventionName);\
	}

	IMPLEMENT_ALL;
#undef IMPLEMENT

#if DELEGATE_INVOKE_BY_REFERENCE == 1
	inline ReturnType invoke(FunctionArguments... args) const {
		return (*this.*(caller))(args...);
	}
#else
#define IMPLEMENT(conventionName) case cc_##conventionName: return (*entity.*(method.member_##conventionName))(args...);
	ReturnType invoke(FunctionArguments... args) const {
		switch (convention)
		{
			IMPLEMENT_ALL;
		}
	}
#endif
};

#undef UPDATE_SELECTOR
#undef IMPLEMENT
#undef IMPLEMENT_ALL

#pragma pop_macro(stdcall)
#pragma pop_macro(cdecl)
#pragma pop_macro(fastcall)
#pragma pop_macro(thiscall)
#pragma pop_macro(vectorcall)
#if _MANAGED
#pragma pop_macro(clrcall)
#endif