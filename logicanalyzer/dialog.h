#ifndef LA_DIALOG
#define LA_DIALOG

// dialogs.c
gchar *LADialogSaveFile(LAWindow *law, const char *title, const char *filterName);
gchar *LADialogOpenFile(LAWindow *law, const char *title, const char *filterName);
void LADialogErrorGeneric(LAWindow *law, char *text);
void LADialogWarningGeneric(LAWindow *law, char *text);
void LADialogNumericEntryError(LAWindow *law);

#endif //LA_DIALOG
