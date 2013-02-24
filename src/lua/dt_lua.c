/*
   This file is part of darktable,
   copyright (c) 2012 Jeremy Rosen

   darktable is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   darktable is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with darktable.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "common/darktable.h"
#include "common/file_location.h"
#include "control/control.h"
#include "lautoc.h"
#include "lua/dt_lua.h"
#include "lua/stmt.h"
#include "lua/events.h"
#include "lua/image.h"
#include "lua/database.h"
#include "lua/gui.h"
#include "lua/glist.h"
#include "lua/styles.h"
#include "common/imageio_module.h"
#include "config.h"


int dt_lua_do_chunk(lua_State *L,int loadresult,int nargs,int nresults) {
  int result;
  if(loadresult){
    dt_control_log("LUA ERROR %s",lua_tostring(L,-1));
    dt_print(DT_DEBUG_LUA,"LUA ERROR %s\n",lua_tostring(L,-1));
    lua_pop(L,1);
    return 0;
  }
  result = lua_gettop(L)-(nargs+1); // remember the stack size to findout the number of results in case of multiret
  if(lua_pcall(L, nargs, nresults,0)) {
    dt_control_log("LUA ERROR %s",lua_tostring(L,-1));
    dt_print(DT_DEBUG_LUA,"LUA ERROR %s\n",lua_tostring(L,-1));
    lua_pop(L,1);
    if(nresults !=LUA_MULTRET) {
      for(int i= 0 ; i < nresults; i++) {
        lua_pushnil(L);
      }
    }
  }
  result= lua_gettop(L) -result;

  lua_gc(L,LUA_GCCOLLECT,0);
  if(darktable.gui!=NULL) dt_control_queue_redraw();
  return result;
}
void dt_lua_protect_call(lua_State *L,lua_CFunction func) {
  lua_pushcfunction(L,func);
  dt_lua_do_chunk(L,0,0,0);
}
void dt_lua_dostring(const char* command) {
  dt_lua_do_chunk(darktable.lua_state,luaL_loadstring(darktable.lua_state, command),0,0);
}

// closed on GC of the dt lib, usually when the lua interpreter closes
static int dt_luacleanup(lua_State*L) {
  const int init_gui = (darktable.gui != NULL);
  if(!init_gui)
    dt_cleanup();
  return 0;
}

static int lua_print(lua_State *L) {
  const int init_gui = (darktable.gui != NULL);
  if(init_gui)
    dt_control_log("%s",luaL_checkstring(L,-1));
  else
    printf("%s\n",luaL_checkstring(L,-1));

  return 0;
}


#if 0
printf("%s %d\n",__FUNCTION__,__LINE__);
for(int i=1 ;i<=lua_gettop(L);i++) {printf("\t%d:%s %s\n",i,lua_typename(L,lua_type(L,i)),luaL_tolstring(L,i,NULL));lua_pop(L,1);}
static void debug_table(lua_State * L,int t) {
  /* table is in the stack at index 't' */
  lua_pushnil(L);  /* first key */
  while (lua_next(L, t-1) != 0) {
    /* uses 'key' (at index -2) and 'value' (at index -1) */
    printf("%s - %s\n",
        luaL_checkstring(L,-2),
        lua_typename(L, lua_type(L, -1)));
    /* removes 'value'; keeps 'key' for next iteration */
    lua_pop(L, 1);
  }
}
#endif

/**
  hardcoded list of types to register
  other types can be added dynamically
 */
static lua_CFunction init_funcs[] = {
  dt_lua_init_stmt,
  dt_lua_init_glist,
  dt_lua_init_styles,
  dt_lua_init_image,
  dt_lua_init_database,
  NULL
};
static int load_darktable_lib(lua_State *L) {

  lua_CFunction* cur_type = init_funcs;
  while(*cur_type) {
    dt_lua_protect_call(L,*cur_type);
    cur_type++;
  }
  return 1;
}




void dt_lua_init_early(lua_State*L){
  if(!L)
    L= luaL_newstate();
  darktable.lua_state= L;
  luaA_open();
  luaL_openlibs(darktable.lua_state);
  dt_lua_push_darktable_lib(L);
  // set the metatable
  lua_newtable(L);
  lua_pushcfunction(L,dt_luacleanup);
  lua_setfield(L,-2,"__gc");
  lua_setmetatable(L,-2);

  lua_pushstring(L,"print");
  lua_pushcfunction(L,&lua_print);
  lua_settable(L,-3);

  lua_pop(L,1);

  dt_lua_initialize_types(L);

}


void dt_lua_init(lua_State*L,const int init_gui){
  char tmp_path[PATH_MAX];

  // init the lua environment
  load_darktable_lib(darktable.lua_state);
  dt_lua_push_darktable_lib(L);
  // build the table containing the configuration info 
  lua_pushstring(L,"configuration");
  lua_newtable(L);

  lua_pushstring(L,"tmp_dir");
  dt_loc_get_tmp_dir(tmp_path, PATH_MAX);
  lua_pushstring(L,tmp_path);
  lua_settable(L,-3);

  lua_pushstring(L,"config_dir");
  dt_loc_get_user_config_dir(tmp_path, PATH_MAX);
  lua_pushstring(L,tmp_path);
  lua_settable(L,-3);

  lua_pushstring(L,"cache_dir");
  dt_loc_get_user_cache_dir(tmp_path, PATH_MAX);
  lua_pushstring(L,tmp_path);
  lua_settable(L,-3);

  lua_pushstring(L,"version");
  lua_pushstring(darktable.lua_state,PACKAGE_VERSION);
  lua_settable(L,-3);

  lua_settable(L,-3);//add the table to the darktable data
  lua_pop(L,-1); //remove the global dt module from the stack
  
  lua_getglobal(L,"package");
  dt_lua_goto_subtable(L,"loaded");
  lua_pushstring(L,"darktable");
  dt_lua_push_darktable_lib(L);
  lua_settable(L,-3);
  lua_pop(L,1);

  lua_getglobal(L,"package");
  lua_getfield(L,-1,"path");
  lua_pushstring(L,";");
  dt_loc_get_user_config_dir(tmp_path, PATH_MAX);
  lua_pushstring(L,tmp_path);
  lua_pushstring(L,"/lua");
  lua_concat(L,4);
  lua_setfield(L,-2,"path");
  lua_pop(L,1);

  if(init_gui) {
    dt_lua_init_events(darktable.lua_state);
    dt_lua_init_gui(darktable.lua_state);
  }



  // if we have a UI, we need to load the modules
  if(init_gui) {
    dt_loc_get_user_config_dir(tmp_path, PATH_MAX);
    g_strlcat(tmp_path,"/rc.lua",PATH_MAX);
    dt_lua_do_chunk(darktable.lua_state,luaL_loadfile(darktable.lua_state,tmp_path),0,0);
  }

}


int dt_lua_push_darktable_lib(lua_State* L) {
  lua_getfield(L,LUA_REGISTRYINDEX,"dt_lua_dtlib");
  if(lua_isnil(L,-1)) {
    lua_pop(L,1);
    lua_newtable(L);
    lua_pushvalue(L,-1);
    lua_setfield(L,LUA_REGISTRYINDEX,"dt_lua_dtlib");
  }
  return 1;
}


void dt_lua_goto_subtable(lua_State *L,const char* sub_name) {
  luaL_checktype(L,-1,LUA_TTABLE);
  lua_getfield(L,-1,sub_name);
  if(lua_isnil(L,-1)) {
    lua_pop(L,1);
    lua_newtable(L);
    lua_setfield(L,-2,sub_name);
    lua_getfield(L,-1,sub_name);
  }
  lua_remove(L,-2);
}

// function used by the lua interpreter to load darktable
int luaopen_darktable(lua_State *L) {
  int tmp_argc = 0;
  char *tmp_argv[]={"lua",NULL};
  char **tmp_argv2 = &tmp_argv[0];
  gtk_init (&tmp_argc, &tmp_argv2);
  char *m_arg[] = {"lua", "--library", ":memory:", NULL};
  // init dt without gui:
  if(dt_init(3, m_arg, 0,L)) exit(1);
  return 1;
}

// modelines: These editor modelines have been set for all relevant files by tools/update_modelines.sh
// vim: shiftwidth=2 expandtab tabstop=2 cindent
// kate: tab-indents: off; indent-width 2; replace-tabs on; indent-mode cstyle; remove-trailing-space on;
