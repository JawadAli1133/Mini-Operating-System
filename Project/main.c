#include <gtk/gtk.h>
#include <sys/wait.h>
void show_task_manager();
gboolean update_process_info(gpointer user_data);
typedef struct
{
    pid_t pid;
    char command[256];
    time_t start_time;
    unsigned long ram_usage; // in KB
} ProcessInfo;

GList *process_list = NULL;

// Function to set a gradient background using CSS
void set_gradient_background(GtkWidget *widget, const gchar *css_gradient)
{
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css_gradient, -1, NULL);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

// Function to add info when a process is created
void add_process_info(pid_t pid, const char *command)
{
    ProcessInfo *info = malloc(sizeof(ProcessInfo));
    info->pid = pid;
    strncpy(info->command, command, sizeof(info->command));
    info->start_time = time(NULL);

    // Calculate RAM usage based on total RAM
    unsigned long total_ram = 8 * 1024 * 1024;       // Convert GB to KB
    unsigned long ram_per_process = total_ram / 100; // Allocate 1% of total RAM per process
    info->ram_usage = ram_per_process;

    process_list = g_list_append(process_list, info);

    // Schedule the next update
    g_timeout_add_seconds(1, (GSourceFunc)update_process_info, NULL);
}

// function to update process info by time
gboolean update_process_info(gpointer user_data)
{
    GList *l = process_list;
    while (l != NULL)
    {
        ProcessInfo *info = (ProcessInfo *)l->data;
        int status;
        pid_t result = waitpid(info->pid, &status, WNOHANG);
        if (result == 0)
        {
            // Process is still running, update RAM usage
            char proc_file[256];
            snprintf(proc_file, sizeof(proc_file), "/proc/%d/statm", info->pid);
            FILE *fp = fopen(proc_file, "r");
            if (fp)
            {
                unsigned long size, resident;
                fscanf(fp, "%lu %lu", &size, &resident);
                info->ram_usage = resident * sysconf(_SC_PAGESIZE) / 1024;
                fclose(fp);
            }
            else
            {
                info->ram_usage = 0;
            }
            l = l->next;
        }
        else if (result > 0)
        {
            // Process has exited
            process_list = g_list_remove_link(process_list, l);
            free(info);
            GList *next = l->next;
            g_list_free_1(l);
            l = next;
        }
        else
        {
            perror("waitpid");
            break;
        }
    }

    return TRUE;
}

// General function to show the application
int show_application(const char *app_name, char *const args[])
{
    int pid = fork();
    if (pid == 0)
    {
        execvp(args[0], args);
        perror("execvp failed");
        exit(1);
    }
    else if (pid < 0)
    {
        perror("fork failed");
        return 1;
    }
    else
    {
        add_process_info(pid, app_name);
        // waitpid(pid, NULL, 0);
    }
    return 0;
}

// Show calender app
int show_calendar()
{
    char *args[] = {"./calender", "Calendar", NULL};
    return show_application("Calendar", args);
}

// Show clock app
int show_clock()
{
    char *args[] = {"./clock", "Clock", NULL};
    return show_application("Clock", args);
}

// show filemanager app
int show_file()
{
    char *args[] = {"./filemanager", "File Manager", NULL};
    return show_application("File Manager", args);
}

// show web browser app
int show_web()
{
    char *args[] = {"./webbrowser", "Web Browser", NULL};
    return show_application("Web Browser", args);
}

// show calculator app
int show_calculator()
{
    char *args[] = {"./calculator", "Calculator", NULL};
    return show_application("Calculator", args);
}

// Show mine sweeper game
int show_game()
{
    char *args[] = {"./minesweeper", "Mine Sweeper", NULL};
    return show_application("Mine Sweeper", args);
}

// Show video player
int show_video()
{
    char *args[] = {"./video", "Video Player", NULL};
    return show_application("Video Player", args);
}

// Show text to voice
int show_voice()
{
    char *args[] = {"./text", "Text to voice", NULL};
    return show_application("Text to voice", args);
}

int copy_file()
{
    char *args[] = {"./copy", "File Copier", NULL};
    return show_application("File Copier", args);
}

int show_notepad()
{
    char *args[] = {"./notepad", "Notepad", NULL};
    return show_application("Notepad", args);
}
// Take to web browser
void launch_command(const gchar *file_path) {
    gchar *command = g_strdup_printf("gst-launch-1.0 playbin uri=file://%s", file_path);
    system(command);
    g_free(command);
}

// exit button function to close application
void on_button_clicked(GtkWidget *widget, gpointer data)
{
    // Exit the GTK main loop, which effectively closes the application
    gtk_main_quit();
}

// Function to end a task by its name
void end_task_by_name(const char *process_name)
{
    GList *l = process_list;
    while (l != NULL)
    {
        ProcessInfo *info = (ProcessInfo *)l->data;
        if (strcmp(info->command, process_name) == 0)
        {
            // Terminate the process
            if (kill(info->pid, SIGTERM) == 0)
            {
                printf("Process %s (PID: %d) terminated successfully.\n", info->command, info->pid);
            }
            else
            {
                perror("Failed to terminate process");
            }
            return;
        }
        l = l->next;
    }
    printf("Process %s not found.\n", process_name);
}

// When an end task button is clicked
void on_end_task_button_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *dialog, *content_area, *entry;
    GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    // Create a new dialog with an entry for the process name
    dialog = gtk_dialog_new_with_buttons("End Task",
                                         GTK_WINDOW(user_data),
                                         flags,
                                         ("_OK"),
                                         GTK_RESPONSE_OK,
                                         ("_Cancel"),
                                         GTK_RESPONSE_CANCEL,
                                         NULL);

    // Set dialog size
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 150);

    // Add CSS provider for styling buttons
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

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter process name");
    gtk_container_add(GTK_CONTAINER(content_area), entry);
    gtk_widget_show_all(dialog);

    // Wait for a response from the dialog
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK)
    {
        const gchar *process_name = gtk_entry_get_text(GTK_ENTRY(entry));
        GList *l;
        ProcessInfo *info = NULL;

        // Find and remove the process from the list
        for (l = process_list; l != NULL; l = l->next)
        {
            info = (ProcessInfo *)l->data;
            if (g_strcmp0(info->command, process_name) == 0)
            {
                process_list = g_list_remove(process_list, info);
                kill(info->pid, SIGKILL);
                free(info);
                break;
            }
        }

        // Refresh the task manager
        if (info)
        {
            gtk_widget_destroy(user_data);
            show_task_manager();
        }
    }

    gtk_widget_destroy(dialog);
}

// Fucntion to show the task manager
void show_task_manager()
{

    // Intializing Widgets
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_tree_view_new());
    GtkListStore *list_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_INT, G_TYPE_UINT, G_TYPE_STRING);
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkWidget *end_task_button = gtk_button_new_with_label("End Task");

    // Setting the widgets
    gtk_window_set_title(GTK_WINDOW(window), "Task Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_tree_view_set_headers_visible(tree_view, TRUE);
    gtk_tree_view_set_model(tree_view, GTK_TREE_MODEL(list_store));

    // Create a vertical box to hold the tree view and the button
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a scrolled window to contain the tree view
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Create a tree view and a list store
    g_object_unref(list_store); // Unref the list store when done

    // Adding coloumns for the task manager
    // Name column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(tree_view, column);
    gtk_tree_view_column_set_fixed_width(column, 300);

    // PID column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("PID", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(tree_view, column);
    gtk_tree_view_column_set_fixed_width(column, 100);

    // RAM column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("RAM (KB)", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(tree_view, column);
    gtk_tree_view_column_set_fixed_width(column, 150);

    // Time column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Start Time", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(tree_view, column);
    gtk_tree_view_column_set_fixed_width(column, 200);

    // Applying CSS
    // Set font size and hover effect
    const gchar *css_style =
        "window { background-color: ##E5E4E2; }"
        "treeview { background-color: #C2B280; font-size: 16px; color: #000000;}"
        "treeview row:nth-child(odd) { background-color: #000000; }"
        "treeview row:hover { background-color: #d3d3d3; }";

    const gchar *button_css1 =
        "button {"
        "    background-color: transparent;"
        "    border: 2px solid #f0f0f0;"
        "    color: #f0f0f0;"
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

    set_gradient_background(window, css_style);
    set_gradient_background(end_task_button, button_css1);
    set_gradient_background(GTK_WIDGET(tree_view), css_style);

    // Populate the list store with process data
    for (GList *l = process_list; l != NULL; l = l->next)
    {
        ProcessInfo *info = (ProcessInfo *)l->data;
        char start_time_str[30];
        strftime(start_time_str, sizeof(start_time_str), "%Y-%m-%d %H:%M:%S", localtime(&info->start_time));
        GtkTreeIter iter;
        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter, 0, info->command, 1, info->pid, 2, info->ram_usage, 3, start_time_str, -1);
    }

    // Show the tree view
    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(tree_view));

    // Create the "End Task" button
    gtk_box_pack_start(GTK_BOX(vbox), end_task_button, FALSE, FALSE, 0);
    g_signal_connect(end_task_button, "clicked", G_CALLBACK(on_end_task_button_clicked), window);

    // Show all widgets
    gtk_widget_show_all(window);
}

// Function to set an image as background
// Function to apply a CSS style with a background image to the window
void set_background_image(GtkWidget *window, const gchar *file_path)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;

    // Create CSS style string
    gchar *css = g_strdup_printf(
        "window {"
        "    background-image: url('%s');"
        "    background-size: cover;"
        "}",
        file_path);

    // Load the CSS style
    gtk_css_provider_load_from_data(provider, css, -1, &error);
    if (error)
    {
        g_warning("Error loading CSS: %s", error->message);
        g_clear_error(&error);
    }

    // Apply the CSS style to the window
    GtkStyleContext *context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Free resources
    g_free(css);
    g_object_unref(provider);
}

// To apply CSS
void apply_css(GtkWidget *widget, const gchar *css)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;

    gtk_css_provider_load_from_data(provider, css, -1, &error);
    if (error)
    {
        g_warning("Error loading CSS: %s", error->message);
        g_clear_error(&error);
    }

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);
}

// Set background Image
void display_button_clicked(GtkWidget *button, gpointer user_data)
{
    GtkWidget *dialog;
    GtkWidget *window = GTK_WIDGET(user_data);
    GtkCssProvider *css_provider;
    gint response;

    // Create the CSS provider and load the CSS
    css_provider = gtk_css_provider_new();
    const gchar *dialog_button_css =
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
          "    background-color: #E5E4E2;"
          "    color: #000000;"
          "    border: 1px solid #CCCCCC;"
          "    margin: 5px;"
          "}"
          ".my-dialog button:hover {"
          "    background-color: #f0f0f0;"
          "    color: #000000;"
          "}";
    gtk_css_provider_load_from_data(css_provider, dialog_button_css, -1, NULL);

    // Apply the CSS provider to the default screen
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Create the file chooser dialog
    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    // Add custom CSS class to the dialog
    GtkStyleContext *context = gtk_widget_get_style_context(dialog);
    gtk_style_context_add_class(context, "my-dialog");

    // Get the buttons in the dialog and apply custom CSS class
    GtkWidget *cancel_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    GtkWidget *open_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

    if (cancel_button) {
        GtkStyleContext *button_context = gtk_widget_get_style_context(cancel_button);
        gtk_style_context_add_class(button_context, "my-dialog");
    }

    if (open_button) {
        GtkStyleContext *button_context = gtk_widget_get_style_context(open_button);
        gtk_style_context_add_class(button_context, "my-dialog");
    }

    // Run the dialog and handle the response
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        set_background_image(window, filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);

    // Clean up
    g_object_unref(css_provider);
}

// Activate function for the main application
static void activate(GtkApplication *app, gpointer user_data)
{
    // Widgets Initialization
    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *calendarButton;
    GtkWidget *fileManagerButton;
    GtkWidget *clockButton;
    GtkWidget *gameButton;
    GtkWidget *calculatorButton;
    GtkWidget *weatherButton;
    GtkWidget *taskManagerButton;
    GtkWidget *displayButton;
    GtkWidget *picButton;
    GtkWidget *voiceButton;
    GtkWidget *fileButton;
    GtkWidget *notepadButton;
    GtkWidget *exitButton;
    GtkWidget *calendarImage;
    GtkWidget *fileManagerImage;
    GtkWidget *clockImage;
    GtkWidget *gameImage;
    GtkWidget *calculatorImage;
    GtkWidget *weatherImage;
    GtkWidget *taskManagerImage;
    GtkWidget *displayImage;
    GtkWidget *picImage;
    GtkWidget *voiceImage;
    GtkWidget *fileImage;
    GtkWidget *notepadImage;
    GtkWidget *exitImage;

    // Setting Windows
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Main Menu");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);

    // Set background gradient color using CSS
    const gchar *css_style = "window { background: linear-gradient(to bottom, #da4453, #89216b); }";
    set_gradient_background(window, css_style);

    // Setting buttons
    fixed = gtk_fixed_new();

    // Creating buttons
    calendarButton = gtk_button_new();
    fileManagerButton = gtk_button_new();
    clockButton = gtk_button_new();
    gameButton = gtk_button_new();
    calculatorButton = gtk_button_new();
    weatherButton = gtk_button_new();
    taskManagerButton = gtk_button_new();
    displayButton = gtk_button_new();
    picButton = gtk_button_new();
    voiceButton = gtk_button_new();
    fileButton = gtk_button_new();
    notepadButton = gtk_button_new();
    exitButton = gtk_button_new();

    // Setting buttons size
    gtk_widget_set_size_request(calendarButton, 100, 100);
    gtk_widget_set_size_request(fileManagerButton, 100, 100);
    gtk_widget_set_size_request(clockButton, 100, 100);
    gtk_widget_set_size_request(gameButton, 100, 100);
    gtk_widget_set_size_request(calculatorButton, 100, 100);
    gtk_widget_set_size_request(weatherButton, 100, 100);
    gtk_widget_set_size_request(taskManagerButton, 100, 100);
    gtk_widget_set_size_request(displayButton, 100, 100);
    gtk_widget_set_size_request(picButton, 100, 100);
    gtk_widget_set_size_request(voiceButton, 100, 100);
    gtk_widget_set_size_request(fileButton, 100, 100);
    gtk_widget_set_size_request(notepadButton, 100, 100);
    gtk_widget_set_size_request(exitButton, 30, 20);

    // Creating images for buttons
    calendarImage = gtk_image_new_from_icon_name("x-office-calendar", GTK_ICON_SIZE_DIALOG);
    fileManagerImage = gtk_image_new_from_icon_name("system-file-manager", GTK_ICON_SIZE_DIALOG);
    clockImage = gtk_image_new_from_icon_name("alarm", GTK_ICON_SIZE_DIALOG);
    gameImage = gtk_image_new_from_icon_name("applications-games", GTK_ICON_SIZE_DIALOG);
    calculatorImage = gtk_image_new_from_icon_name("accessories-calculator", GTK_ICON_SIZE_DIALOG);
    weatherImage = gtk_image_new_from_icon_name("web-browser", GTK_ICON_SIZE_DIALOG);
    taskManagerImage = gtk_image_new_from_icon_name("utilities-system-monitor", GTK_ICON_SIZE_DIALOG);
    displayImage = gtk_image_new_from_icon_name("preferences-desktop-wallpaper", GTK_ICON_SIZE_DIALOG);
    picImage = gtk_image_new_from_icon_name("video-display", GTK_ICON_SIZE_DIALOG);
    voiceImage = gtk_image_new_from_icon_name("audio-input-microphone", GTK_ICON_SIZE_DIALOG);
    fileImage = gtk_image_new_from_icon_name("gtk-copy-file", GTK_ICON_SIZE_DIALOG);
    notepadImage = gtk_image_new_from_icon_name("document-new", GTK_ICON_SIZE_DIALOG);
    exitImage = gtk_image_new_from_icon_name("system-log-out", GTK_ICON_SIZE_DIALOG);

    // Setting images
    gtk_button_set_image(GTK_BUTTON(calendarButton), calendarImage);
    gtk_button_set_image(GTK_BUTTON(fileManagerButton), fileManagerImage);
    gtk_button_set_image(GTK_BUTTON(clockButton), clockImage);
    gtk_button_set_image(GTK_BUTTON(gameButton), gameImage);
    gtk_button_set_image(GTK_BUTTON(calculatorButton), calculatorImage);
    gtk_button_set_image(GTK_BUTTON(weatherButton), weatherImage);
    gtk_button_set_image(GTK_BUTTON(taskManagerButton), taskManagerImage);
    gtk_button_set_image(GTK_BUTTON(displayButton), displayImage);
    gtk_button_set_image(GTK_BUTTON(picButton), picImage);
    gtk_button_set_image(GTK_BUTTON(voiceButton), voiceImage);
    gtk_button_set_image(GTK_BUTTON(fileButton), fileImage);
    gtk_button_set_image(GTK_BUTTON(notepadButton), notepadImage);
    gtk_button_set_image(GTK_BUTTON(exitButton), exitImage);

    // Placing buttons
    gtk_fixed_put(GTK_FIXED(fixed), calendarButton, 5, 5);
    gtk_fixed_put(GTK_FIXED(fixed), fileManagerButton, 5, 110);
    gtk_fixed_put(GTK_FIXED(fixed), clockButton, 5, 215);
    gtk_fixed_put(GTK_FIXED(fixed), gameButton, 5, 320);
    gtk_fixed_put(GTK_FIXED(fixed), calculatorButton, 5, 425);
    gtk_fixed_put(GTK_FIXED(fixed), weatherButton, 5, 530);
    gtk_fixed_put(GTK_FIXED(fixed), taskManagerButton, 5, 635);
    gtk_fixed_put(GTK_FIXED(fixed), displayButton, 130, 5);
    gtk_fixed_put(GTK_FIXED(fixed), picButton, 130, 110);
    gtk_fixed_put(GTK_FIXED(fixed), voiceButton, 130, 215);
    gtk_fixed_put(GTK_FIXED(fixed), fileButton, 130, 320);
    gtk_fixed_put(GTK_FIXED(fixed), notepadButton, 130, 425);
    gtk_fixed_put(GTK_FIXED(fixed), exitButton, 5, 840);

    // Connecting signals
    g_signal_connect(G_OBJECT(calendarButton), "clicked", G_CALLBACK(show_calendar), NULL);
    g_signal_connect(G_OBJECT(clockButton), "clicked", G_CALLBACK(show_clock), NULL);
    g_signal_connect(G_OBJECT(fileManagerButton), "clicked", G_CALLBACK(show_file), NULL);
    g_signal_connect(G_OBJECT(gameButton), "clicked", G_CALLBACK(show_game), NULL);
    g_signal_connect(G_OBJECT(calculatorButton), "clicked", G_CALLBACK(show_calculator), NULL);
    g_signal_connect(G_OBJECT(weatherButton), "clicked", G_CALLBACK(show_web), NULL);
    g_signal_connect(G_OBJECT(taskManagerButton), "clicked", G_CALLBACK(show_task_manager), NULL);
    g_signal_connect(G_OBJECT(displayButton), "clicked", G_CALLBACK(display_button_clicked), window);
    g_signal_connect(G_OBJECT(picButton), "clicked", G_CALLBACK(show_video), NULL);
    g_signal_connect(G_OBJECT(voiceButton), "clicked", G_CALLBACK(show_voice), NULL);
    g_signal_connect(G_OBJECT(fileButton), "clicked", G_CALLBACK(copy_file), NULL);
    g_signal_connect(G_OBJECT(notepadButton), "clicked", G_CALLBACK(show_notepad), NULL);
    g_signal_connect(G_OBJECT(exitButton), "clicked", G_CALLBACK(on_button_clicked), NULL);

    const gchar *button_css =
        "button {"
        "    background-color: f0f0f0;"
        "    border: 1px solid f0f0f0;"                                   
        "    color: #000000;"                                                   
        "    font-size: 16px;"                                                  
        "    padding: 12px 24px;"                                               
        "    outline: none;"                                                    
        "    transition: background-color 0.3s, color 0.3s, border-color 0.3s;" 
        "    border-radius: 20px;"                                            
        "}"
        "button:hover {"
        "    background-color: #34495e;"
        "    color: white;"              
        "    border-color: #34495e;"    
        "}";

    set_gradient_background(calendarButton, button_css);
    set_gradient_background(fileManagerButton, button_css);
    set_gradient_background(clockButton, button_css);
    set_gradient_background(gameButton, button_css);
    set_gradient_background(calculatorButton, button_css);
    set_gradient_background(weatherButton, button_css);
    set_gradient_background(taskManagerButton, button_css);
    set_gradient_background(displayButton, button_css);
    set_gradient_background(picButton, button_css);
    set_gradient_background(voiceButton, button_css);
    set_gradient_background(fileButton, button_css);
    set_gradient_background(notepadButton, button_css);
    set_gradient_background(exitButton, button_css);

    // Adding in the container
    gtk_container_add(GTK_CONTAINER(window), fixed);

    gtk_widget_show_all(window);
}

// Main function
int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("in.Jawad", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}