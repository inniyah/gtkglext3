/* GdkGLExt - OpenGL Extension to GDK
 * Copyright (C) 2002  Naofumi Yasufuku
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA.
 */

#ifndef __GDK_GL_PRIVATE_H__
#define __GDK_GL_PRIVATE_H__

#include <gdk/gdkgldebug.h>
#include <gdk/gdkgltokens.h>
#include <gdk/gdkgltypes.h>
#include <gdk/gdkgldrawable.h>

/* I may perhaps support gettext in future. */
#define _(String) (String)

G_BEGIN_DECLS

#define  GDK_GL_CONFIG_AS_SINGLE_MODE(glconfig) ((glconfig)->as_single_mode)

void     _gdk_gl_context_set_gl_drawable   (GdkGLContext  *glcontext,
                                            GdkGLDrawable *gldrawable);

gboolean _gdk_gl_pixmap_is_double_buffered (GdkGLDrawable *gldrawable);

gboolean _gdk_gl_window_is_double_buffered (GdkGLDrawable *gldrawable);

G_END_DECLS

#endif /* __GDK_GL_PRIVATE_H__ */
