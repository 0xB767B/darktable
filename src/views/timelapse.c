// https://github.com/fhrtms/darktable.git
// https://www.youtube.com/watch?v=vUGtla-q8OE

#include "common/collection.h"
#include "common/darktable.h"
#include "control/conf.h"
#include "control/control.h"
#include "control/signal.h"
#include "views/view.h"
#include "views/view_api.h"

DT_MODULE(1)

typedef struct dt_timelapse_t
{
  int dummy;
  GtkWidget* button;
} dt_timelapse_t;


////////////////////////////////////////////////////////////////////////////////
// view api functions
////////////////////////////////////////////////////////////////////////////////

// get translatable name
const char *name(const dt_view_t *self)
{
  return C_("view", "timelapse");
}

// get the view type
uint32_t view(const dt_view_t *self)
{
  return DT_VIEW_TIMELAPSE;
}

// init *data
void init(struct dt_view_t *self)
{
  self->data = calloc(1, sizeof(dt_timelapse_t));
}

// cleanup *data
void cleanup(struct dt_view_t *self)
{
  free(self->data);
}

// test if enter can succeed
int try_enter(dt_view_t *self)
{
  (void)self;
  // verify that there are images to display
  if(dt_collection_get_count(darktable.collection) != 1) // TODO should be > 1 for a minimal timelapse ;-)
  {
    return 0;
  }
  else
  {
    dt_control_log(_("there are no images in this collection"));
    return 1;
  }
}

// mode entered, this module got focus. return non-null on failure.
void enter(dt_view_t *self)
{
  dt_timelapse_t *d = (dt_timelapse_t *)self->data;

  d->dummy = 0;

  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_LEFT, TRUE, TRUE);
  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_RIGHT, FALSE, TRUE);
  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_TOP, FALSE, TRUE);
  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_BOTTOM, FALSE, TRUE);
  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_CENTER_TOP, FALSE, TRUE);
  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_CENTER_BOTTOM, FALSE, TRUE);

  // also hide arrows --> TODO this does not actually hide the arrows
  dt_control_queue_redraw();



  //  /* connect to ui pipe finished signal for redraw */
  //  dt_control_signal_connect(darktable.signals, DT_SIGNAL_DEVELOP_UI_PIPE_FINISHED,
  //                            G_CALLBACK(_darkroom_ui_pipe_finish_signal_callback), (gpointer)self);

  //  dt_control_signal_connect(darktable.signals, DT_SIGNAL_TIMELAPSE_RESET,
  //                            G_CALLBACK(_reset_frames), (gpointer)self);

  //  dt_control_signal_connect(darktable.signals, DT_SIGNAL_TIMELAPSE_INITIALIZE,
  //                            G_CALLBACK(_read_frames), (gpointer)self);

  //  dt_control_signal_connect(darktable.signals, DT_SIGNAL_TIMELAPSE_EQUALIZE,
  //                            G_CALLBACK(_equalize_frames), (gpointer)self);

  //  dt_control_signal_connect(darktable.signals, DT_SIGNAL_TIMELAPSE_RESET_REGION,
  //                            G_CALLBACK(_reset_region), (gpointer)self);

  //  dt_control_signal_connect(darktable.signals, DT_SIGNAL_TIMELAPSE_SET_REGION,
  //                            G_CALLBACK(_set_region), (gpointer)self);

  //  dt_control_signal_connect(darktable.signals, DT_SIGNAL_TIMELAPSE_SET_EXPOSURE_GAIN_START,
  //                            G_CALLBACK(_set_exposure_gain_start), (gpointer)self);

  //  dt_control_signal_connect(darktable.signals, DT_SIGNAL_TIMELAPSE_SET_EXPOSURE_GAIN_END,
  //                            G_CALLBACK(_set_exposure_gain_end), (gpointer)self);
////////////////////////////////////////////////////////////////////////////////

  d->button = gtk_button_new_with_label("HELLO");

  /* add map to center widget */
  gtk_overlay_add_overlay(GTK_OVERLAY(dt_ui_center_base(darktable.gui->ui)), GTK_WIDGET(d->button));

  // ensure the log msg widget stay on top
  /* gtk_overlay_reorder_overlay(GTK_OVERLAY(dt_ui_center_base(darktable.gui->ui)), */
  /*                             gtk_widget_get_parent(dt_ui_log_msg(darktable.gui->ui)), -1); */

  gtk_widget_show_all(GTK_WIDGET(d->button));
  ////////////////////////////////////////////////////////////////////////////////
}

// mode left (is called after the new try_enter has succeeded)
void leave(struct dt_view_t *self)
{
}

// expose the module (gtk callback)
void expose(struct dt_view_t *self, cairo_t *cr, int32_t width, int32_t height, int32_t pointerx, int32_t pointery)
{
  // dt_timelapse_t *lib = (dt_timelapse_t *)self->data;
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_paint(cr);
}

void reset(struct dt_view_t *self);    // reset default appearance




//uint32_t view(const dt_view_t *self)
//{
//  return DT_VIEW_TIMELAPSE;
//}
