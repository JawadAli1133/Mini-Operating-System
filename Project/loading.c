#include <gtk/gtk.h>

void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void set_gradient_background(GtkWidget *widget, const gchar *css_gradient) {
    GtkStyleContext *context;
    GtkCssProvider *provider;

    context = gtk_widget_get_style_context(widget);
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(GTK_CSS_PROVIDER(provider), css_gradient, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void show_main_menu() {
    char *args[] = {"./main", NULL};
    execv(args[0], args);
}

static void activate_loading(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *loginButton;
    GtkWidget *userWidget;
    GtkWidget *label;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "LogIn Screen");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);

    const gchar *css_style = "window { background: linear-gradient(to bottom, #42275a , #734b6d); }"
                             "label#jawads_label { font-weight: bold; font-size: 36px; color: white; }";

    fixed = gtk_fixed_new();

    loginButton = gtk_button_new_with_label("Sign In");

    gtk_widget_set_size_request(loginButton, 200, 50);

    label = gtk_label_new("JAWAD's OPERATING SYSTEM");
    gtk_widget_set_name(label, "jawads_label"); 

    gint label_width, label_height;
    gtk_widget_get_size_request(label, &label_width, &label_height);
    gint label_x = ((1920 - label_width) / 2) - 230;
    gint label_y = 50;
    gtk_fixed_put(GTK_FIXED(fixed), label, label_x, label_y);

    userWidget = gtk_event_box_new();
    gtk_widget_set_size_request(userWidget, 300, 300);

    GtkWidget *userImage = gtk_image_new_from_file("/home/jawad/Downloads/user.png");
    gtk_widget_set_size_request(userImage, 300, 300);

    gtk_container_add(GTK_CONTAINER(userWidget), userImage);

    gint window_width, window_height;
    gtk_window_get_size(GTK_WINDOW(window), &window_width, &window_height);

    gint button_width, button_height;
    gtk_widget_get_size_request(loginButton, &button_width, &button_height);
    gint button_x = ((window_width - button_width) / 2) + 5;
    gint button_y = (window_height - button_height) / 2;

    set_gradient_background(window, css_style);
    set_gradient_background(label, css_style);

    gtk_fixed_put(GTK_FIXED(fixed), loginButton, button_x, button_y);

    gint widget_width, widget_height;
    gtk_widget_get_size_request(userWidget, &widget_width, &widget_height);
    gint widget_x = ((window_width - widget_width) / 2);
    gint widget_y = button_y - widget_height - 20;
    gtk_fixed_put(GTK_FIXED(fixed), userWidget, widget_x, widget_y);

    g_signal_connect(G_OBJECT(loginButton), "clicked", G_CALLBACK(show_main_menu), NULL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);

    const gchar *button_css =
        "button {"
        "    background-color: transparent;"
        "    border: 2px solid #6c5ce7;"                                        
        "    color: #6c5ce7;"                                                   
        "    font-size: 16px;"                                                  
        "    padding: 12px 24px;"                                               
        "    outline: none;"                                                    
        "    transition: background-color 0.3s, color 0.3s, border-color 0.3s;" 
        "    border-radius: 20px;"                                              
        "}"
        "button:hover {"
        "    background-color: #6c5ce7;" 
        "    color: white;"              
        "    border-color: #6c5ce7;" 
        "}";

    set_gradient_background(loginButton, button_css);

    gtk_container_add(GTK_CONTAINER(window), fixed);
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.LoadingScreen", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate_loading), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
