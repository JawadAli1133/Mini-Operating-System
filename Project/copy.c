#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>

// Function to copy the selected file
void copy_file(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget **)data;
    const gchar *source_path = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const gchar *destination_folder = gtk_entry_get_text(GTK_ENTRY(entries[1]));

    GFile *source = g_file_new_for_path(source_path);
    GFile *destination_folder_gfile = g_file_new_for_path(destination_folder);
    gchar *source_basename = g_path_get_basename(source_path);
    gchar *destination_path = g_build_filename(destination_folder, source_basename, NULL);

    GFile *destination_file = g_file_new_for_path(destination_path);

    if (g_file_query_exists(source, NULL)) {
        if (g_file_query_exists(destination_file, NULL)) {
            g_print("Destination file already exists: %s\n", destination_path);
        } else {
            GError *error = NULL;
            if (g_file_copy(source, destination_file, G_FILE_COPY_NONE, NULL, NULL, NULL, &error)) {
                g_print("File copied successfully to: %s\n", destination_path);
            } else {
                g_print("Error copying file: %s\n", error->message);
                g_error_free(error);
            }
        }
    } else {
        g_print("Source file does not exist: %s\n", source_path);
    }

    g_free(source_basename);
    g_free(destination_path);
    g_object_unref(source);
    g_object_unref(destination_folder_gfile);
    g_object_unref(destination_file);
}

// Function to move the selected file
void move_file(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget **)data;
    const gchar *source_path = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const gchar *destination_folder = gtk_entry_get_text(GTK_ENTRY(entries[1]));

    GFile *source = g_file_new_for_path(source_path);
    GFile *destination_folder_gfile = g_file_new_for_path(destination_folder);
    gchar *source_basename = g_path_get_basename(source_path);
    gchar *destination_path = g_build_filename(destination_folder, source_basename, NULL);

    GFile *destination_file = g_file_new_for_path(destination_path);

    if (g_file_query_exists(source, NULL)) {
        if (g_file_query_exists(destination_file, NULL)) {
            g_print("Destination file already exists: %s\n", destination_path);
        } else {
            GError *error = NULL;
            if (g_file_move(source, destination_file, G_FILE_COPY_NONE, NULL, NULL, NULL, &error)) {
                g_print("File moved successfully to: %s\n", destination_path);
            } else {
                g_print("Error moving file: %s\n", error->message);
                g_error_free(error);
            }
        }
    } else {
        g_print("Source file does not exist: %s\n", source_path);
    }

    g_free(source_basename);
    g_free(destination_path);
    g_object_unref(source);
    g_object_unref(destination_folder_gfile);
    g_object_unref(destination_file);
}

// Function to open file chooser dialog for selecting source file
void select_source_file(GtkWidget *widget, gpointer entry) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Select Source File",
                                        NULL,
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        ("_Cancel"), GTK_RESPONSE_CANCEL,
                                        ("_Open"), GTK_RESPONSE_ACCEPT,
                                        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        gtk_entry_set_text(GTK_ENTRY(entry), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Function to open file chooser dialog for selecting destination folder
void select_destination_folder(GtkWidget *widget, gpointer entry) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Select Destination Folder",
                                        NULL,
                                        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                        ("_Cancel"), GTK_RESPONSE_CANCEL,
                                        ("_Select"), GTK_RESPONSE_ACCEPT,
                                        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *foldername;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        foldername = gtk_file_chooser_get_filename(chooser);
        gtk_entry_set_text(GTK_ENTRY(entry), foldername);
        g_free(foldername);
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *source_entry;
    GtkWidget *destination_entry;
    GtkWidget *select_source_button;
    GtkWidget *select_destination_button;
    GtkWidget *copy_button;
    GtkWidget *move_button;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Copy/Move File");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 300);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    source_entry = gtk_entry_new();
    destination_entry = gtk_entry_new();

    select_source_button = gtk_button_new_with_label("Select Source File");
    select_destination_button = gtk_button_new_with_label("Select Destination Folder");
    copy_button = gtk_button_new_with_label("Copy File");
    move_button = gtk_button_new_with_label("Move File");

    gtk_box_pack_start(GTK_BOX(box), source_entry, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), select_source_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), destination_entry, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), select_destination_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), copy_button, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), move_button, FALSE, FALSE, 5);

    GtkWidget *entries[2] = { source_entry, destination_entry };

    g_signal_connect(select_source_button, "clicked", G_CALLBACK(select_source_file), source_entry);
    g_signal_connect(select_destination_button, "clicked", G_CALLBACK(select_destination_folder), destination_entry);
    g_signal_connect(copy_button, "clicked", G_CALLBACK(copy_file), entries);
    g_signal_connect(move_button, "clicked", G_CALLBACK(move_file), entries);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // CSS for styling
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    const gchar *css = 
        "window {"
        "   background-color: #000000;"
        "}"
        "entry {"
        "   padding: 10px;"
        "   border-radius: 5px;"
        "   border: 1px solid #ccc;"
        "   font-size: 14px;"
        "   color: #61dafb;"
        "   background-color: #282c34;"
        "}"
        "button {"
        "   background-color: #000000;"
        "   color: #f0f0f0;"
        "   font-size: 16px;"
        "   border: 1px solid #ccc;"
        "   padding: 12px 24px;"
        "   outline: none;"
        "   transition: background-color 0.3s, color 0.3s;"
        "}"
        "button:hover {"
        "   background-color: #f0f0f0;"
        "   color: black;"
        "}";

    gtk_css_provider_load_from_data(provider, css, -1, NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
