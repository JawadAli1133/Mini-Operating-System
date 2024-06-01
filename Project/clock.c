#include <gtk/gtk.h>
#include <time.h>

static gboolean update_time(gpointer label) {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "<span font_desc='Lato 60'><b>%H:%M:%S</b></span>", timeinfo);
    gtk_label_set_markup(GTK_LABEL(label), buffer);

    return TRUE; 
}

static void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *label;
    GtkCssProvider *cssProvider;
    GdkDisplay *display;
    GdkScreen *screen;

    gtk_init(&argc, &argv);

    // Setting Window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Clock");
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 250);

    // Adding CSS
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider,
                                    "label { color: #FFFFFF; }",
                                    -1,NULL);
    gtk_style_context_add_provider_for_screen(screen,GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    label = gtk_label_new("");
    gtk_container_add(GTK_CONTAINER(window), label);
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);

    // Update the time every second
    g_timeout_add(1000, update_time, label);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
