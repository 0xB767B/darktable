/*
    This file is part of darktable,
    copyright (c) 2010 Tobias Ellinghaus.

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

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string.h>
#include <gtk/gtk.h>

/** replace all occurences of pattern by substitute. the returned value has to be freed after use. */
gchar* dt_util_str_replace(const gchar* string, const gchar* pattern, const gchar* substitute);
/** count the number of occurences of needle in haystack */
guint dt_util_str_occurence(const gchar *haystack,const gchar *needle);
#endif
