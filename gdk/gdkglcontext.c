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

#include "gdkglprivate.h"
#include "gdkgldrawable.h"
#include "gdkglconfig.h"
#include "gdkglcontext.h"

enum {
  PROP_0,
  PROP_GLDRAWABLE,
  PROP_GLDRAWABLE_READ,
  PROP_GLCONFIG,
  PROP_SHARE_LIST,
  PROP_IS_DIRECT,
  PROP_RENDER_TYPE
};

static void gdk_gl_context_class_init   (GdkGLContextClass *klass);

static void gdk_gl_context_set_property (GObject           *object,
                                         guint              property_id,
                                         const GValue      *value,
                                         GParamSpec        *pspec);
static void gdk_gl_context_get_property (GObject           *object,
                                         guint              property_id,
                                         GValue            *value,
                                         GParamSpec        *pspec);
static void gdk_gl_context_finalize     (GObject           *object);

static gpointer parent_class = NULL;

GType
gdk_gl_context_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo type_info = {
        sizeof (GdkGLContextClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) gdk_gl_context_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,                   /* class_data */
        sizeof (GdkGLContext),
        0,                      /* n_preallocs */
        (GInstanceInitFunc) NULL,
      };

      type = g_type_register_static (G_TYPE_OBJECT,
                                     "GdkGLContext",
                                     &type_info, 0);
    }

  return type;
}

static void
gdk_gl_context_class_init (GdkGLContextClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  GDK_GL_NOTE (FUNC, g_message (" - gdk_gl_context_class_init ()"));

  parent_class = g_type_class_peek_parent (klass);

  object_class->set_property = gdk_gl_context_set_property;
  object_class->get_property = gdk_gl_context_get_property;
  object_class->finalize     = gdk_gl_context_finalize;

  g_object_class_install_property (object_class,
                                   PROP_GLDRAWABLE,
                                   g_param_spec_pointer ("gldrawable",
                                                         "GL drawable",
                                                         "GdkGLDrawable object.",
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  g_object_class_install_property (object_class,
                                   PROP_GLDRAWABLE_READ,
                                   g_param_spec_pointer ("gldrawable_read",
                                                         "Read GL drawable",
                                                         "Read GdkGLDrawable object.",
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  g_object_class_install_property (object_class,
                                   PROP_GLCONFIG,
                                   g_param_spec_object ("glconfig",
                                                        "Frame buffer configuration",
                                                        "OpenGL frame buffer configuration object.",
                                                        GDK_TYPE_GL_CONFIG,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property (object_class,
                                   PROP_SHARE_LIST,
                                   g_param_spec_pointer ("share_list",
                                                         "Share list",
                                                         "Rendering context with which to share display lists.",
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property (object_class,
                                   PROP_IS_DIRECT,
                                   g_param_spec_boolean ("is_direct",
                                                         "Is direct",
                                                         "Whether rendering is to be done with a direct connection to the graphics system.",
                                                         FALSE,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property (object_class,
                                   PROP_RENDER_TYPE,
                                   g_param_spec_int ("render_type",
                                                     "Render type",
                                                     "Type of the rendering context to be created.",
                                                     GDK_GL_RGBA_TYPE,
                                                     GDK_GL_COLOR_INDEX_TYPE,
                                                     GDK_GL_RGBA_TYPE,
                                                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void
gdk_gl_context_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GdkGLContext *glcontext = GDK_GL_CONTEXT (object);

  GDK_GL_NOTE (FUNC, g_message (" - gdk_gl_context_set_property ()"));

  switch (property_id)
    {
    case PROP_GLDRAWABLE:
      _gdk_gl_context_set_gl_drawable (glcontext, g_value_get_pointer (value));
      break;
    case PROP_GLDRAWABLE_READ:
      _gdk_gl_context_set_gl_drawable_read (glcontext, g_value_get_pointer (value));
      break;
    case PROP_GLCONFIG:
      glcontext->glconfig = g_value_get_object (value);
      g_object_ref (G_OBJECT (glcontext->glconfig));
      g_object_notify (object, "glconfig");
      break;
    case PROP_SHARE_LIST:
      glcontext->share_list = g_value_get_pointer (value);
      if (glcontext->share_list != NULL &&
          GDK_IS_GL_CONTEXT (glcontext->share_list))
        g_object_ref (G_OBJECT (glcontext->share_list));
      else
        glcontext->share_list = NULL;
      g_object_notify (object, "share_list");
      break;
    case PROP_IS_DIRECT:
      glcontext->is_direct = g_value_get_boolean (value);
      g_object_notify (object, "is_direct");
      break;
    case PROP_RENDER_TYPE:
      glcontext->render_type = g_value_get_int (value);
      g_object_notify (object, "render_type");
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gdk_gl_context_get_property (GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GdkGLContext *glcontext = GDK_GL_CONTEXT (object);

  GDK_GL_NOTE (FUNC, g_message (" - gdk_gl_context_get_property ()"));

  switch (property_id)
    {
    case PROP_GLDRAWABLE:
      g_value_set_pointer (value, glcontext->gldrawable);
      break;
    case PROP_GLDRAWABLE_READ:
      g_value_set_pointer (value, glcontext->gldrawable_read);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gdk_gl_context_finalize (GObject *object)
{
  GdkGLContext *glcontext = GDK_GL_CONTEXT (object);

  GDK_GL_NOTE (FUNC, g_message (" - gdk_gl_context_finalize ()"));

  if (glcontext->gldrawable != NULL)
    {
      g_object_unref (G_OBJECT (glcontext->gldrawable));
      glcontext->gldrawable = NULL;
    }

  if (glcontext->gldrawable_read != NULL)
    {
      g_object_unref (G_OBJECT (glcontext->gldrawable_read));
      glcontext->gldrawable_read = NULL;
    }

  if (glcontext->glconfig != NULL)
    {
      g_object_unref (G_OBJECT (glcontext->glconfig));
      glcontext->glconfig = NULL;
    }

  if (glcontext->share_list != NULL)
    {
      g_object_unref (G_OBJECT (glcontext->share_list));
      glcontext->share_list = NULL;
    }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

/**
 * gdk_gl_context_new:
 * @gldrawable: a #GdkGLDrawable.
 * @glconfig: a #GdkGLConfig.
 * @share_list: the #GdkGLContext which to share display lists. NULL indicates that no sharing is to take place.
 * @direct: whether rendering is to be done with a direct connection to the graphics system.
 * @render_type: GDK_GL_RGBA_TYPE or GDK_GL_COLOR_INDEX_TYPE (currently not used).
 *
 * Create a new OpenGL rendering context.
 *
 * Return value: the new #GdkGLContext.
 **/
GdkGLContext *
gdk_gl_context_new (GdkGLDrawable *gldrawable,
                    GdkGLConfig   *glconfig,
                    GdkGLContext  *share_list,
                    gboolean       direct,
                    int            render_type)
{
  g_return_val_if_fail (GDK_IS_GL_DRAWABLE (gldrawable), NULL);

  return GDK_GL_DRAWABLE_GET_CLASS (gldrawable)->create_new_context (gldrawable,
                                                                     glconfig,
                                                                     share_list,
                                                                     direct,
                                                                     render_type);
}

/*< private >*/
void
_gdk_gl_context_set_gl_drawable (GdkGLContext  *glcontext,
                                 GdkGLDrawable *gldrawable)
{
  GDK_GL_NOTE (FUNC, g_message (" - _gdk_gl_context_set_gl_drawable ()"));

  g_return_if_fail (GDK_IS_GL_CONTEXT (glcontext));

  if (glcontext->gldrawable == gldrawable)
    return;

  if (glcontext->gldrawable != NULL)
    {
      g_object_unref (G_OBJECT (glcontext->gldrawable));
      glcontext->gldrawable = NULL;
    }

  if (gldrawable != NULL && GDK_IS_GL_DRAWABLE (gldrawable))
    {
      glcontext->gldrawable = gldrawable;
      g_object_ref (G_OBJECT (glcontext->gldrawable));
    }

  g_object_notify (G_OBJECT (glcontext), "gldrawable");
}

/*< private >*/
void
_gdk_gl_context_set_gl_drawable_read (GdkGLContext  *glcontext,
                                      GdkGLDrawable *gldrawable_read)
{
  GDK_GL_NOTE (FUNC, g_message (" - _gdk_gl_context_set_gl_drawable_read ()"));

  g_return_if_fail (GDK_IS_GL_CONTEXT (glcontext));

  if (glcontext->gldrawable_read == gldrawable_read)
    return;

  if (glcontext->gldrawable_read != NULL)
    {
      g_object_unref (G_OBJECT (glcontext->gldrawable_read));
      glcontext->gldrawable_read = NULL;
    }

  if (gldrawable_read != NULL && GDK_IS_GL_DRAWABLE (gldrawable_read))
    {
      glcontext->gldrawable_read = gldrawable_read;
      g_object_ref (G_OBJECT (glcontext->gldrawable_read));
    }

  g_object_notify (G_OBJECT (glcontext), "gldrawable_read");
}

/**
 * gdk_gl_context_get_gl_drawable:
 * @glcontext: a #GdkGLContext.
 *
 * Get #GdkGLDrawable to which the @glcontext is binded.
 *
 * Return value: the #GdkGLDrawable.
 **/
GdkGLDrawable *
gdk_gl_context_get_gl_drawable (GdkGLContext *glcontext)
{
  g_return_val_if_fail (GDK_IS_GL_CONTEXT (glcontext), NULL);

  return glcontext->gldrawable;
}

/**
 * gdk_gl_context_get_gl_config:
 * @glcontext: a #GdkGLContext.
 *
 * Get #GdkGLConfig with which the @glcontext is configured.
 *
 * Return value: the #GdkGLConfig.
 **/
GdkGLConfig *
gdk_gl_context_get_gl_config (GdkGLContext *glcontext)
{
  g_return_val_if_fail (GDK_IS_GL_CONTEXT (glcontext), NULL);

  return glcontext->glconfig;
}

/**
 * gdk_gl_context_get_share_list:
 * @glcontext: a #GdkGLContext.
 *
 * Get #GdkGLContext with which the @glcontext shares the display lists.
 *
 * Return value: the #GdkGLContext.
 **/
GdkGLContext *
gdk_gl_context_get_share_list (GdkGLContext *glcontext)
{
  g_return_val_if_fail (GDK_IS_GL_CONTEXT (glcontext), NULL);

  return glcontext->share_list;
}

/**
 * gdk_gl_context_is_direct:
 * @glcontext: a #GdkGLContext.
 *
 * Returns whether the @glcontext is a direct rendering context.
 *
 * Return value: TRUE if the @glcontext is a direct rendering contest.
 **/
gboolean
gdk_gl_context_is_direct (GdkGLContext *glcontext)
{
  g_return_val_if_fail (GDK_IS_GL_CONTEXT (glcontext), FALSE);

  return glcontext->is_direct;
}

/**
 * gdk_gl_context_get_render_type:
 * @glcontext: a #GdkGLContext.
 *
 * Get render_type of the @glcontext.
 *
 * Return value: GDK_GL_RGBA_TYPE or GDK_GL_COLOR_INDEX_TYPE.
 **/
int
gdk_gl_context_get_render_type (GdkGLContext *glcontext)
{
  g_return_val_if_fail (GDK_IS_GL_CONTEXT (glcontext), 0);

  return glcontext->render_type;
}

/**
 * gdk_gl_context_get_colormap:
 * @glcontext: a #GdkGLContext.
 *
 * Get #GdkColormap with which the @glcontext is configured.
 *
 * Return value: the #GdkColormap.
 **/
GdkColormap *
gdk_gl_context_get_colormap (GdkGLContext *glcontext)
{
  g_return_val_if_fail (GDK_IS_GL_CONTEXT (glcontext), NULL);

  return gdk_gl_config_get_colormap (glcontext->glconfig);
}
