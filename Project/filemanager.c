#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

GtkWidget *listbox;
GtkWidget *window;
char *selected_file = NULL;

static void populate_file_list(GtkWidget *listbox, const char *directory);
static void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

static void delete_file(GtkWidget *widget, gpointer user_data) {
    if (selected_file != NULL) {
        if (remove(selected_file) == 0) {
            g_print("File deleted: %s\n", selected_file);
            populate_file_list(listbox, "/home/jawad/MiniOS/Project");
            gtk_widget_show_all(window);
        } else {
            g_print("Failed to delete file: %s\n", selected_file);
        }
        g_free(selected_file);
        selected_file = NULL;
    } else {
        g_print("No file selected for deletion!\n");
    }
}

static void create_file(GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog;
    gint response;

    // Create a file chooser dialog
    dialog = gtk_file_chooser_dialog_new("Create File",
                                         GTK_WINDOW(user_data),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "Save",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);

    // Set default file name
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "Untitled.txt");

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    // If the user chooses to save the file
    if (response == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        // Create the file
        FILE *file = fopen(filename, "w");
        if (file != NULL) {
            fclose(file);
            g_print("File created: %s\n", filename);
            populate_file_list(listbox, "/home/jawad/MiniOS/Project");
            gtk_widget_show_all(window);
        } else {
            g_print("Failed to create file!\n");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Function to populate a list box with file names
static void populate_file_list(GtkWidget *listbox, const char *directory) {
    DIR *dir;
    struct dirent *ent;

    // Clear the existing list
    gtk_list_box_unselect_all(GTK_LIST_BOX(listbox));
    gtk_container_foreach(GTK_CONTAINER(listbox), (GtkCallback)gtk_widget_destroy, NULL);

    dir = opendir(directory);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                GtkWidget *row = gtk_list_box_row_new();
                GtkWidget *label = gtk_label_new(ent->d_name);
                gtk_style_context_add_class(gtk_widget_get_style_context(label), "file-label");
                gtk_container_add(GTK_CONTAINER(row), label);
                gtk_list_box_insert(GTK_LIST_BOX(listbox), row, -1);
            }
        }
        closedir(dir);
    }
}

static void file_selected(GtkListBox *box, GtkListBoxRow *row, gpointer user_data) {
    if (selected_file != NULL) {
        g_free(selected_file);
        selected_file = NULL;
    }

    if (row != NULL) {
        GtkWidget *label = gtk_bin_get_child(GTK_BIN(row));
        const gchar *filename = gtk_label_get_text(GTK_LABEL(label));
        selected_file = g_strdup_printf("/home/jawad/MiniOS/Project/%s", filename);
        g_print("Selected file: %s\n", selected_file);
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *button_delete;
    GtkWidget *button_create;
    GtkWidget *hbox_buttons;
    GtkWidget *vbox_main;
    GtkCssProvider *css_provider;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "File Manager");
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 650);

    // Applying CSS
    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
          "button {"
          "    background-color: transparent;"
          "    border: 2px solid #FFFFFF;"
          "    color: #FFFFFF;"
          "    font-size: 16px;"
          "    padding: 12px 24px;"
          "    outline: none;"
          "    transition: background-color 0.3s, color 0.3s, border-color 0.3s;"
          "}"
          "button:hover {"
          "    background-color: #FFFFFF;"
          "    color: black;"
          "    border-color: #FFFFFF;"
          "}"
          ".my-dialog {"
          "    background-color: #F5F5F5;"
          "}"
          ".my-dialog button {"
          "    background-color: #FFFFFF;"
          "    color: #333333;"
          "    border: 1px solid #CCCCCC;"
          "    margin: 5px;"
          "}"
          ".my-dialog button:hover {"
          "    background-color: #EEEEEE;"
          "}"
          ".file-label {"
          "    font-size: 17px;"
          "}",
          -1, NULL);


    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Create main vertical box
    vbox_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox_main);

    // Create list box
    listbox = gtk_list_box_new();
    g_signal_connect(listbox, "row-selected", G_CALLBACK(file_selected), NULL);
    gtk_box_pack_start(GTK_BOX(vbox_main), listbox, TRUE, TRUE, 0);

    // Create buttons horizontal box
    hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_end(GTK_BOX(vbox_main), hbox_buttons, FALSE, FALSE, 0);

    // Delete File button
    button_delete = gtk_button_new_with_label("Delete File");
    g_signal_connect(button_delete, "clicked", G_CALLBACK(delete_file), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), button_delete, TRUE, TRUE, 0);

    // Create File button
    button_create = gtk_button_new_with_label("Create File");
    g_signal_connect(button_create, "clicked", G_CALLBACK(create_file), window);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), button_create, TRUE, TRUE, 0);

    // Populate list box with file names
    populate_file_list(listbox, "/home/jawad/MiniOS/Project"); // Change the path as needed

    // Show all widgets
    gtk_widget_show_all(window);

    // Run the GTK main loop
    gtk_main();

    return 0;
}