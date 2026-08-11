#ifndef LA_CLOCK_SET
#define LA_CLOCK_SET

#ifndef LAZoomSetWindow
	typedef struct _la_zoom_set_window LAZoomSetWindow;
#endif

struct _la_zoom_set_window {
	GtkWidget *window;
	GtkWidget *content;
	GtkWidget *vbox;

	GtkWidget *label;
	GtkWidget *spin;

	uint8_t isActive;
};


// clockset.c
void LACreateZoomSetWindow(LAWindow *law, LAZoomSetWindow *laz);
// status.c
void LAPlaceStatusBar(LAWindow *law);
void LAUpdateStatusBar(LAWindow *law);

#endif //LA_CLOCK_SET
