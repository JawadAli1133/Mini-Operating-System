#include <gtk/gtk.h>

static void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *calendar;
    GtkCssProvider *cssProvider;
    GdkDisplay *display;
    GdkScreen *screen;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calender");
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500); 

    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider,
                                    "calendar { font-size: 20px; }",
                                    -1,
                                    NULL);
    gtk_style_context_add_provider_for_screen(screen,
                                               GTK_STYLE_PROVIDER(cssProvider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    calendar = gtk_calendar_new();
    gtk_container_add(GTK_CONTAINER(window), calendar);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}