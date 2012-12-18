#ifndef ULT_LUNAR_H_
#define ULT_LUNAR_H_

#include "lua.hpp"

template <typename T>
class Lunar {

private:

  typedef int (T::*mfp)(lua_State* L);

public:

  struct RegType {
    const char* name;
    mfp mfunc;
  };

  static void Register(lua_State* L) {
    lua_newtable(L);
    int methods = lua_gettop(L);
    luaL_newmetatable(L, T::kLunarId);
    int metatable = lua_gettop(L);
    // store method table in globals so that
    // scripts can add functions written in Lua.

    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, methods);
    lua_settable(L, metatable);  // hide metatable from Lua getmetatable()

    lua_pushliteral(L, "__index");
    lua_pushvalue(L, methods);
    lua_settable(L, metatable);

    lua_pushliteral(L, "__tostring");
    lua_pushcfunction(L, Tostring);
    lua_settable(L, metatable);

    lua_pushliteral(L, "__gc");
    lua_pushcfunction(L, Gc);
    lua_settable(L, metatable);

    // fill method table with methods from class T
    for (const RegType *l = T::kLunarMethods; l->name != NULL; l++) {
      /* edited by Snaily: shouldn't it be const RegType *l ... ? */
      lua_pushstring(L, l->name);
      lua_pushlightuserdata(L, (void*)l);
      lua_pushcclosure(L, Thunk, 1);
      lua_settable(L, methods);
    }

    lua_pop(L, 2);  // drop metatable and method table
  }

  static int New(lua_State* L) {
    T* obj = new T();
    UserdataType* ud = 
        static_cast<UserdataType*>(lua_newuserdata(L, sizeof(UserdataType)));
    ud->pt = obj;
    luaL_getmetatable(L, T::kLunarId);
    lua_setmetatable(L, -2);
    return 1;
  }

private:

  struct UserdataType {
    T* pt;
  };

  static T *GetObject(lua_State *L, int narg) {
    UserdataType *ud =
        static_cast<UserdataType*>(lua_touserdata(L, narg));
    if (ud != NULL) {
      return ud->pt;  // pointer to T object
    } else {
      return NULL;
    }
  }

  static int Thunk(lua_State *L) {
    // stack has userdata, followed by method args
    T *obj = GetObject(L, 1);  // get 'self', or if you prefer, 'this'
    lua_remove(L, 1);  // remove self so member function args start at index 1
    // get member function from upvalue
    RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));
    return (obj->*(l->mfunc))(L);  // call member function
  }

  static int Tostring(lua_State* L) {
    char buffer[32];
    T* obj = GetObject(L, 1);
    sprintf_s(buffer, 32, "%p", obj);
    lua_pushfstring(L, "%s (%s)", T::kLunarId, buffer);
    return 1;
  }

  // garbage collection metamethod
  static int Gc(lua_State *L) {
    T* obj = GetObject(L, 1);
    delete obj;  // call destructor for T objects
    return 0;
  }

};

#define LUNAR_DECLARE_METHOD(Class, Name) {#Name, &Class::Name}

#endif