#include <gtk/gtk.h>

void web_browser(GtkWidget *widget, gpointer data)
{
    const gchar *url = gtk_entry_get_text(GTK_ENTRY(data));

    // Allocate memory for the full URL
    size_t full_url_length = strlen("https://www.") + strlen(url) + 2;
    gchar *full_url = malloc(full_url_length);

    // Construct the full URL
    strcpy(full_url, "https://www.");
    strcat(full_url, url);
    strcat(full_url, "\\");

    // Create the command string
    gchar *command = g_strdup_printf("xdg-open %s", full_url);
    system(command);

    // Free the allocated memory
    g_free(command);
    free(full_url);
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *entry;
    GtkWidget *button;
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    gtk_init(&argc, &argv);

    // Create a new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Web Browser");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 100);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the entry and the button
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the URL entry
    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, TRUE, 5);

    // Create the button
    button = gtk_button_new_with_label("Open");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 5);

    // Connect the button's clicked event to the web_browser function
    g_signal_connect(button, "clicked", G_CALLBACK(web_browser), entry);

    // Create a CSS provider and load CSS
    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
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
        "}"
        "button {"
        "    background-color: transparent;"
        "    border: 2px solid #000000;"
        "    color: #F0f0f0;"
        "    font-size: 16px;"
        "    padding: 12px 24px;"
        "    outline: none;"
        "    transition: background-color 0.3s, color 0.3s, border-color 0.3s;"
        "}"
        "button:hover {"
        "    background-color: #f0f0f0;"
        "    color: black;"
        "    border-color: #f0f0f0;"
        "}";
    "button:hover {"
    "   background-color: #45a049;"
    "}";

    gtk_css_provider_load_from_data(provider, css, -1, NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
