#include <gtk/gtk.h>
#include <stdlib.h>

void launch_command(const gchar *file_path) {
    gchar *command = g_strdup_printf("gst-launch-1.0 playbin uri=file://%s", file_path);
    system(command);
    g_free(command);
}

// Declaration of on_file_set function
void on_file_set(GtkFileChooserDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        gchar *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (file_path != NULL) {
            gtk_widget_destroy(GTK_WIDGET(dialog));
            launch_command(file_path);
            g_free(file_path);
        }
    } else if (response_id == GTK_RESPONSE_CANCEL) {
        gtk_widget_destroy(GTK_WIDGET(dialog));
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *dialog;
    GtkWindow *parent_window = NULL;

    dialog = gtk_file_chooser_dialog_new("Open File",
                                         parent_window,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 150);

    // Apply CSS styling to the file chooser dialog buttons
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
                                    ".my-dialog button {"
                                    "    background-color: #FFFFFF;"
                                    "    color: #333333;"
                                    "    border: 1px solid #CCCCCC;"
                                    "    margin: 5px;"
                                    "}"
                                    ".my-dialog button:hover {"
                                    "    background-color: #EEEEEE;"
                                    "}",
                                    -1,
                                    NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(dialog);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_signal_connect(dialog, "response", G_CALLBACK(on_file_set), NULL);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(dialog);

    gtk_main();

    return 0;
}