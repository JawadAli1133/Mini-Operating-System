#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#define ROWS 10
#define COLS 10
#define MINES 15

GtkWidget *buttons[ROWS][COLS];
int mines[ROWS][COLS];
int revealed[ROWS][COLS];

void initialize_game()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            mines[i][j] = 0;
            revealed[i][j] = 0;
        }
    }

    // Place mines randomly
    int count = 0;
    while (count < MINES)
    {
        int row = rand() % ROWS;
        int col = rand() % COLS;
        if (mines[row][col] == 0)
        {
            mines[row][col] = 1;
            count++;
        }
    }
}

void reveal_cell(GtkWidget *widget, gpointer data)
{
    int row = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "row"));
    int col = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "col"));

    if (revealed[row][col])
        return;

    revealed[row][col] = 1;
    gtk_widget_set_sensitive(widget, FALSE);

    if (mines[row][col])
    {
        gtk_button_set_label(GTK_BUTTON(widget), "*");
        // Game over, reveal all mines
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                if (mines[i][j])
                    gtk_button_set_label(GTK_BUTTON(buttons[i][j]), "*");
            }
        }
        // Display "Game Over"
        GtkWidget *label = gtk_label_new("Game Over");
        gtk_grid_attach(GTK_GRID(gtk_widget_get_parent(GTK_WIDGET(widget))), label, 0, ROWS + 1, COLS, 1);
        gtk_widget_show(label);
        // Disable further interaction with the grid
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                gtk_widget_set_sensitive(buttons[i][j], FALSE);
            }
        }
    }
    else
    {
        // Count adjacent mines
        int count = 0;
        for (int i = row - 1; i <= row + 1; i++)
        {
            for (int j = col - 1; j <= col + 1; j++)
            {
                if (i >= 0 && i < ROWS && j >= 0 && j < COLS && mines[i][j])
                    count++;
            }
        }
        if (count > 0)
            gtk_button_set_label(GTK_BUTTON(widget), g_strdup_printf("%d", count));
        else
            gtk_button_set_label(GTK_BUTTON(widget), "");
    }
}

void setup_ui(GtkWidget *window)
{
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // CSS for styling buttons
    const gchar *button_css =
        "button {"
        "    background-color: #e0e0e0;"
        "    color: #000000;"
        "    font-size: 16px;"
        "    padding: 10px 20px;"
        "    border: none;"
        "}"
        "button:hover {"
        "    background-color: #c0c0c0;"
        "}";

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, button_css, -1, NULL);

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            buttons[i][j] = gtk_button_new();
            gtk_widget_set_size_request(buttons[i][j], 60, 60);
            gtk_grid_attach(GTK_GRID(grid), buttons[i][j], j, i, 1, 1);
            g_object_set_data(G_OBJECT(buttons[i][j]), "row", GINT_TO_POINTER(i));
            g_object_set_data(G_OBJECT(buttons[i][j]), "col", GINT_TO_POINTER(j));
            g_signal_connect(buttons[i][j], "clicked", G_CALLBACK(reveal_cell), NULL);
        }
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Minesweeper");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    initialize_game();
    setup_ui(window);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
