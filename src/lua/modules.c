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
#include "lua/modules.h"
#include "lua/types.h"
typedef enum {
  GET_NAME,
  GET_EXTENSION,
  GET_MIME,
  GET_MAX_WIDTH,
  GET_MAX_HEIGHT,
  LAST_FORMAT_FIELD
} format_fields;
static const char *format_fields_name[] = {
  "name",
  "extension",
  "mime",
  "max_width",
  "max_height",
  NULL
};

static int format_index(lua_State*L) {
  uint32_t width,height;
  int index = lua_tonumber(L,-1);
  luaL_getmetafield(L,-2,"__format_object");
  dt_imageio_module_format_t * format = lua_touserdata(L,-1);
  dt_imageio_module_data_t * data = lua_touserdata(L,-3);
  switch(index) {
    case GET_NAME:
      lua_pushstring(L,format->name());
      return 1;
    case GET_EXTENSION:
      lua_pushstring(L,format->extension(data));
      return 1;
    case GET_MIME:
      lua_pushstring(L,format->mime(data));
      return 1;
    case GET_MAX_WIDTH:
      format->dimension(format,&width,&height);
      lua_pushinteger(L,width);
      return 1;
    case GET_MAX_HEIGHT:
      format->dimension(format,&width,&height);
      lua_pushinteger(L,height);
      return 1;
    default:
      return luaL_error(L,"should never happen %d",index);
  }
}

luaA_Type dt_lua_init_format_internal(lua_State* L, dt_imageio_module_format_t* module, char*type_name,size_t size){
  luaA_type_add(type_name,size);
  luaA_Type my_type = dt_lua_init_type_internal(L,type_name,format_fields_name,format_index,NULL,size);
  luaA_struct_typeid(L,my_type);
  //luaA_struct_member_typeid(L,my_type,"style",luaA_type_id(const char*),offsetof(dt_imageio_module_data_t,style));
  lua_pushlightuserdata(L,module);
	lua_setfield(L,-2,"__format_object");
  lua_pop(L,1); // pop the metatable for the type, we don't want users to change it
  return my_type;
};


static int get_format_params(lua_State *L) {
  int size;
  dt_imageio_module_format_t *format_module = lua_touserdata(L,lua_upvalueindex(1));
  dt_imageio_module_data_t *fdata = format_module->get_params(format_module, &size);
  luaA_push_typeid(L,format_module->parameter_lua_type,fdata);
  format_module->free_params(format_module,fdata);
  return 1;
}

int dt_lua_init_modules(lua_State *L) {
  dt_lua_push_darktable_lib(L);
  dt_lua_goto_subtable(L,"modules");
  dt_lua_goto_subtable(L,"format");
  GList * elt = darktable.imageio->plugins_format;
  while(elt) {
    dt_imageio_module_format_t* format_module = (dt_imageio_module_format_t*)elt->data;
    lua_pushlightuserdata(L,format_module);
    lua_pushcclosure(L,get_format_params,1);
    lua_setfield(L,-2,format_module->name());
    elt = g_list_next(elt);
  }
  
  lua_pop(L,-1);
  return 0;
}
// modelines: These editor modelines have been set for all relevant files by tools/update_modelines.sh
// vim: shiftwidth=2 expandtab tabstop=2 cindent
// kate: tab-indents: off; indent-width 2; replace-tabs on; indent-mode cstyle; remove-trailing-space on;
