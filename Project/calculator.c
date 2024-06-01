#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Function declarations
static void destroy(GtkWidget *widget, gpointer data);
static void button_clicked(GtkWidget *button, gpointer data);
static void append_to_display(const gchar *text);
static void clear_display();
static int evaluate_expression(const char *expression);


GtkWidget *display_entry;

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    GtkCssProvider *cssProvider;
    GdkDisplay *display;
    GdkScreen *screen;

    gtk_init(&argc, &argv);

    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calculator");
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 600);

    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider,
                                    "button { font-size: 30px; padding: 20px 40px;}"
                                    "button:hover {"
                                    "background-color: #FFFFFF;"
                                    "color: black;"
                                    "border-color: #FFFFFF;"
                                    "}"
                                    "entry { font-size: 36px; }",
                                    -1,
                                    NULL);
    gtk_style_context_add_provider_for_screen(screen ,GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    display_entry = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(display_entry), 1);
    gtk_grid_attach(GTK_GRID(grid), display_entry, 0, 0, 4, 2);

    // Array to hold button labels
    gchar *button_labels[] =
        {
            "7", "8", "9", "/",
            "4", "5", "6", "*",
            "1", "2", "3", "-",
            "AC", "0", "=", "+"};

    // Attaching buttons to grid
    int row, col;
    for (int i = 0; i < 16; i++)
    {
        col = i % 4;
        row = i / 4 + 2;
        button = gtk_button_new_with_label(button_labels[i]);
        gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
        g_signal_connect(button, "clicked", G_CALLBACK(button_clicked), button_labels[i]);
    }

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

static void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

// Function for button clicks
static void button_clicked(GtkWidget *button, gpointer data)
{
    const gchar *label = gtk_button_get_label(GTK_BUTTON(button));

    if (strcmp(label, "AC") == 0)
    {
        clear_display();
    }
    else if (strcmp(label, "=") == 0)
    {
        const gchar *expression = gtk_entry_get_text(GTK_ENTRY(display_entry));
        int result = evaluate_expression(expression);

        gchar *result_str = g_strdup_printf("%d", result);
        gtk_entry_set_text(GTK_ENTRY(display_entry), result_str);
        g_free(result_str);
    }
    else
    {
        append_to_display(label);
    }
}

// Function to append text to the display entry
static void append_to_display(const gchar *text)
{
    gchar *current_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(display_entry)));
    gchar *new_text = g_strconcat(current_text, text, NULL);
    gtk_entry_set_text(GTK_ENTRY(display_entry), new_text);
    g_free(current_text);
    g_free(new_text);
}

// Function to clear the display entry
static void clear_display()
{
    gtk_entry_set_text(GTK_ENTRY(display_entry), "");
}

// Function to evaluate the expression
static int evaluate_expression(const char *expression)
{
    int num1, num2, result = 0;
    char op;

    if (sscanf(expression, "%d%c%d", &num1, &op, &num2) == 3)
    {
        switch (op)
        {
        case '+':
            result = num1 + num2;
            break;
        case '-':
            result = num1 - num2;
            break;
        case '*':
            result = num1 * num2;
            break;
        case '/':
            if (num2 != 0)
                result = num1 / num2;
            else
                result = 0;
            break;
        default:
            result = 0;
            break;
        }
    }
    else
    {
        result = 0;
    }

    return result;
}
