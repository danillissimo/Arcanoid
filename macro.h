#pragma once

#define GET(type, name)\
private:\
    type _##name;\
public:\
    inline type get##name() const noexcept {return _##name;}\

#define GET_CONST(type, name)\
private:\
    type _##name;\
public:\
    inline const type& get##name() const noexcept {return _##name;}\

#define GETSET(type, name) \
private: \
    type _##name; \
public: \
    inline type get##name() const noexcept { return _##name; } \
    inline void set##name(type value) { _##name = value; }

#define GETSET_CONST(type, name) \
private: \
    type _##name; \
public: \
    inline const type& get##name() const noexcept { return _##name; } \
    inline void set##name(type value) { _##name= value; }

//Just for better visibility
#define OVERRIDE override