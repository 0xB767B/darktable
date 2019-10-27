// https://github.com/fhrtms/darktable.git

#include "common/collection.h"
#include "common/darktable.h"
#include "control/control.h"

DT_MODULE(1)

enum
{
  COL_KEYFRAME,
  COL_FRAME,
  COL_FILENAME,
  COL_CORRECTION,
  COL_BRIGHTNESS,
  COL_EXPOSURE,
  COL_APERTURE,
  COL_ISO,
  COL_FOCAL_LENGTH,
};

typedef struct dt_timelapse_t
{
  int dummy;

  GtkWidget *framelist_tree;
  GtkCellRenderer *framelist_renderer;
  GtkListStore *framelist_store;
  GtkTreeIter framelist_iter;

} dt_timelapse_t;

////////////////////////////////////////////////////////////////////////////////
// view api functions
////////////////////////////////////////////////////////////////////////////////

// get translatable name
const char *name(const dt_view_t *self)
{
  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);
  return _("timelapse");
}

// get the view type
uint32_t view(const struct dt_view_t *self)
{
  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);
  return DT_VIEW_TIMELAPSE;
}

// get flags of the view
//uint32_t flags();

// init *data
void init(struct dt_view_t *self)
{
  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);
  self->data = calloc(1, sizeof(dt_timelapse_t));
}


// create gtk elements, called after libs are created
void gui_init(struct dt_view_t *self)
{
  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);
  dt_timelapse_t *timelapse = (dt_timelapse_t *)self->data;

  timelapse->dummy = 0;
}

// cleanup *data
void cleanup(struct dt_view_t *self)
{
  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);
  dt_timelapse_t *data = (dt_timelapse_t *)self->data;
  free(data);
}

// expose the module (gtk callback)
void expose(struct dt_view_t *self, cairo_t *cr, int32_t width, int32_t height,
            int32_t pointerx, int32_t pointery)
{
  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);

  // clear the current surface
  /* dt_gui_gtk_set_source_rgb(cr, DT_GUI_COLOR_PREVIEW_HOVER_BORDER); */
  /* cairo_paint(cr); */
}

// test if enter can succeed.
int try_enter(dt_view_t *self)
{
  (void)self;

  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);

  // verify that there are images to display
  if(dt_collection_get_count(darktable.collection) > 1) {
    return 0;
  } else {
    dt_control_log(_("there are no images in this collection"));
    return 1;
  }
}

// mode entered, this module got focus. return non-null on failure.
void enter(dt_view_t *self)
{
  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);

  dt_timelapse_t *timelapse = (dt_timelapse_t *)self->data;

  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_LEFT, TRUE, TRUE);
  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_RIGHT, FALSE, FALSE);
  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_TOP, TRUE, TRUE);
  dt_ui_panel_show(darktable.gui->ui, DT_UI_PANEL_BOTTOM, TRUE, FALSE);

  // also hide arrows
  gtk_widget_hide(darktable.gui->widgets.right_border);
  gtk_widget_hide(darktable.gui->widgets.bottom_border);


  GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_set_vexpand(scroll,TRUE);
  timelapse->framelist_tree = gtk_tree_view_new();
  timelapse->framelist_renderer = gtk_cell_renderer_toggle_new();

  GtkTreeViewColumn *column;
  column = gtk_tree_view_column_new_with_attributes(
    _("keyframe"),
    timelapse->framelist_renderer, "active",
    COL_KEYFRAME,
    NULL);

  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column, 50);
  gtk_tree_view_append_column(GTK_TREE_VIEW(timelapse->framelist_tree), column);

  timelapse->framelist_renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(
    _("frame"),
    timelapse->framelist_renderer,
    "text", COL_FRAME,
    "foreground",
    "Red",
    "foreground-set",
    TRUE,
    NULL);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column, 50);
  gtk_tree_view_append_column(GTK_TREE_VIEW(timelapse->framelist_tree), column);

  timelapse->framelist_renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(
    _("filename"),
    timelapse->framelist_renderer,
    "text",
    COL_FILENAME,
    NULL);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column, 200);
  gtk_tree_view_append_column(GTK_TREE_VIEW(timelapse->framelist_tree), column);

  column = gtk_tree_view_column_new_with_attributes(
    _("correction"),
    timelapse->framelist_renderer,
    "text",
    COL_CORRECTION, NULL);
  gtk_tree_view_column_set_sizing(column , GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column , 100);
  gtk_tree_view_append_column(GTK_TREE_VIEW(timelapse->framelist_tree), column);

  timelapse->framelist_renderer = gtk_cell_renderer_text_new();
  g_object_set(timelapse->framelist_renderer, "editable", TRUE, NULL);
  column = gtk_tree_view_column_new_with_attributes(
    _("brightness"),
    timelapse->framelist_renderer,
    "text",
    COL_BRIGHTNESS,
    NULL);
  gtk_tree_view_column_set_sizing(column , GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column , 100);
  gtk_tree_view_append_column(GTK_TREE_VIEW(timelapse->framelist_tree), column);

  timelapse->framelist_renderer = gtk_cell_renderer_text_new();
  g_object_set(timelapse->framelist_renderer, "editable", TRUE, NULL);
  column = gtk_tree_view_column_new_with_attributes(
    _("exposure"),
    timelapse->framelist_renderer,
    "text",
    COL_EXPOSURE,
    NULL);
  gtk_tree_view_column_set_sizing(column , GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column , 100);
  gtk_tree_view_append_column(GTK_TREE_VIEW(timelapse->framelist_tree), column);

  timelapse->framelist_renderer = gtk_cell_renderer_text_new();
  g_object_set(timelapse->framelist_renderer, "editable", TRUE, NULL);
  column = gtk_tree_view_column_new_with_attributes(
    _("aperture"),
    timelapse->framelist_renderer,
    "text",
    COL_APERTURE,
    NULL);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column, 100);
  gtk_tree_view_append_column(GTK_TREE_VIEW(timelapse->framelist_tree), column);

  timelapse->framelist_renderer = gtk_cell_renderer_text_new();
  g_object_set(timelapse->framelist_renderer, "editable", TRUE, NULL);
  column = gtk_tree_view_column_new_with_attributes(
    _("ISO"),
    timelapse->framelist_renderer,
    "text",
    COL_ISO,
    NULL);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column, 100);
  gtk_tree_view_append_column(GTK_TREE_VIEW(timelapse->framelist_tree), column);

  timelapse->framelist_renderer = gtk_cell_renderer_text_new();
  g_object_set(timelapse->framelist_renderer, "editable", TRUE, NULL);
  column = gtk_tree_view_column_new_with_attributes(
    _("focal length"),
    timelapse->framelist_renderer,
    "text",
    COL_FOCAL_LENGTH,
    NULL);
  gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column, 100);
  gtk_tree_view_append_column(GTK_TREE_VIEW(timelapse->framelist_tree), column);

  // Attaching the model to the treeview
  gtk_tree_view_set_model(
    GTK_TREE_VIEW(timelapse->framelist_tree),
    GTK_TREE_MODEL(timelapse->framelist_store));

  GtkWidget *parent = gtk_widget_get_parent(dt_ui_center(darktable.gui->ui));
  GtkWidget *fixed;

  fixed = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(parent), fixed);
  gtk_widget_show(fixed);

  timelapse->framelist_store = gtk_list_store_new(9,
      G_TYPE_BOOLEAN,
      G_TYPE_INT,
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING
  );

  gtk_tree_view_set_model(
    GTK_TREE_VIEW(timelapse->framelist_tree),
    GTK_TREE_MODEL(timelapse->framelist_store));
  gtk_list_store_append(timelapse->framelist_store, &timelapse->framelist_iter);

  char exposure_correction[256];
  snprintf(exposure_correction, sizeof(exposure_correction), "%.2g", 0.1454);

  char average_brightness[256];
  snprintf(average_brightness, sizeof(average_brightness), "%.6Lg", (long double)104.4785211);

  char exif_exposure[256];
  snprintf(exif_exposure, sizeof(exif_exposure), "1/%.2g", 1 / 0.5);

  char exif_aperture[256];
  snprintf(exif_aperture, sizeof(exif_aperture), "%.2g", 16.0);

  char exif_iso[256];
  snprintf(exif_iso, sizeof(exif_iso), "%.0f", 100.0);

  char exif_focal_length[256];
  snprintf(exif_focal_length, sizeof(exif_focal_length), "%.0f mm", 16.0);

  gtk_list_store_set(timelapse->framelist_store, &timelapse->framelist_iter, 
                     COL_KEYFRAME, FALSE, 
                     COL_FRAME, 158, 
                     COL_FILENAME, "photocopy 4", 
                     COL_CORRECTION, exposure_correction, 
                     COL_BRIGHTNESS, average_brightness, 
                     COL_EXPOSURE, exif_exposure, 
                     COL_APERTURE, exif_aperture, 
                     COL_ISO, exif_iso, 
                     COL_FOCAL_LENGTH, exif_focal_length, 
                     NULL);

  gtk_container_add(GTK_CONTAINER(fixed), timelapse->framelist_tree);
  gtk_widget_show(timelapse->framelist_tree);
}

// mode left (is called after the new try_enter has succeeded)
void leave(struct dt_view_t *self)
{
  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);
}

// reset default appearance
void reset(struct dt_view_t *self)
{
  fprintf(stderr, "-- %s:%s\n", __FILE__, __FUNCTION__);
}

