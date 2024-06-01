#include <gtk/gtk.h>

// Callback function for "Save" menu item
void save_file(GtkWidget *widget, gpointer window) {
    GtkFileChooserDialog *dialog;
    gint res;
    gchar *filename;
    gchar *text;
    GtkTextBuffer *buffer;
    GtkTextIter start, end;

    dialog = GTK_FILE_CHOOSER_DIALOG(gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Save", GTK_RESPONSE_ACCEPT,
                                         NULL));
    
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(window));
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        g_file_set_contents(filename, text, -1, NULL);
        g_free(text);
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

// Callback function for "Open" menu item
void open_file(GtkWidget *widget, gpointer window) {
    GtkFileChooserDialog *dialog;
    gint res;
    gchar *filename;
    gchar *text;
    GtkTextBuffer *buffer;
    GtkTextIter iter;
    
    dialog = GTK_FILE_CHOOSER_DIALOG(gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL));
    
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        g_file_get_contents(filename, &text, NULL, NULL);
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(window));
        gtk_text_buffer_set_text(buffer, text, -1);
        g_free(text);
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *text_view;
    GtkWidget *menu_bar;
    GtkWidget *file_menu;
    GtkWidget *file_menu_item;
    GtkWidget *save_menu_item;
    GtkWidget *open_menu_item;
    GtkTextBuffer *buffer;
    GtkBox *vbox;
    GtkCssProvider *css_provider;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Notepad");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);

    vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(vbox));

    menu_bar = gtk_menu_bar_new();
    file_menu = gtk_menu_new();

    file_menu_item = gtk_menu_item_new_with_label("File");
    save_menu_item = gtk_menu_item_new_with_label("Save");
    open_menu_item = gtk_menu_item_new_with_label("Open");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);

    gtk_box_pack_start(vbox, menu_bar, FALSE, FALSE, 0);

    text_view = gtk_text_view_new();
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    
    // Set font size using CSS
    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "textview { font-size: 12pt; }",
        -1,
        NULL
    );
    GtkStyleContext *context = gtk_widget_get_style_context(text_view);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    gtk_box_pack_start(vbox, text_view, TRUE, TRUE, 0);

    g_signal_connect(save_menu_item, "activate", G_CALLBACK(save_file), text_view);
    g_signal_connect(open_menu_item, "activate", G_CALLBACK(open_file), text_view);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    g_object_unref(css_provider);

    return 0;
}
