#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define GRID_SIZE 15

void make_grid(int arg, char *argv[]);
void on_click(GtkWidget *wid, gpointer data);

struct xy_coordinate
{
    int x;
    int y;
};

int main(int arg, char *argv[])
{
    make_grid(arg, argv);
    return 0;
}

void on_click(GtkWidget *widget, gpointer data)
{
    struct xy_coordinate *coordinates = (struct xy_coordinate *)data;
    printf("Button clicked at position row and col: (%d, %d)\n", coordinates->x, coordinates->y);
}

void apply_css()
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    if (!gtk_css_provider_load_from_path(provider, "style.css", NULL))
    {
        g_printerr("Failed to load CSS file\n");
        return;
    }

    gtk_style_context_add_provider_for_screen(
        screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);
}
void make_grid(int arg, char *argv[])
{
    gtk_init(&arg, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Dynamic Ludo Game Grid");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            GtkWidget *button = gtk_button_new_with_label("");
            struct xy_coordinate *coordinates = g_malloc(sizeof(struct xy_coordinate));
            coordinates->x = i;
            coordinates->y = j;

            GtkStyleContext *context = gtk_widget_get_style_context(button);
            if (i <= 5 && j <= 5) 
            {
                if ((i == 2 && j == 2) || (i == 2 && j == 3) || (i == 3 && j == 2) || (i == 3 && j == 3))
                {
                    gtk_style_context_add_class(context, "blue");
                }
                else if (i > 0 && i < 5 && j > 0 && j < 5)
                {
                    gtk_style_context_add_class(context, "transparent-inner");
                }
                else
                {
                    gtk_style_context_add_class(context, "top-left");
                }
            }
            else if (i <= 5 && j >= 9) // Top-right zone
            {
                if ((i == 2 && j == 11) || (i == 3 && j == 11) || (i == 3 && j == 12) || (i == 2 && j == 12))
                {
                    gtk_style_context_add_class(context, "yellow");
                }
                else if (i > 0 && i < 5 && j > 9 && j < 14)
                {
                    gtk_style_context_add_class(context, "transparent-inner");
                }
                else
                {
                    gtk_style_context_add_class(context, "top-right");
                }
            }
            else if (i >= 9 && j <= 5) // Bottom-left zone
            {
                if ((i == 11 && j == 2) || (i == 12 && j == 2) || (i == 12 && j == 3) || (i == 11 && j == 3))
                {
                    gtk_style_context_add_class(context, "red");
                }

                else if (i > 9 && i < 14 && j > 0 && j < 5)
                {
                    gtk_style_context_add_class(context, "transparent-inner");
                }
                else
                {
                    gtk_style_context_add_class(context, "bottom-left");
                }
            }
            else if (i >= 9 && j >= 9) // Bottom-right zone
            {
                if ((i == 11 && j == 11) || (i == 12 && j == 11) || (i == 12 && j == 12) || (i == 11 && j == 12))
                {
                    gtk_style_context_add_class(context, "green");
                }
                else if (i > 9 && i < 14 && j > 9 && j < 14)
                {
                    gtk_style_context_add_class(context, "transparent-inner");
                }
                else
                {
                    gtk_style_context_add_class(context, "bottom-right");
                }
            }
            else if (i >= 6 && i <= 8 && j >= 6 && j <= 8) // Middle zone
            {
                gtk_style_context_add_class(context, "middle");
            }
            if ((i == 8 && j == 6) || (i == 8 && j == 7))
            {
                gtk_style_context_add_class(context, "red");
            }
            if ((i == 6 && j == 6) || (i == 7 && j == 6))
            {
                gtk_style_context_add_class(context, "blue");
            }
            if ((i == 8 && j == 8) || (i == 7 && j == 8))
            {
                gtk_style_context_add_class(context, "green");
            }
            if ((i == 6 && j == 7) || (i == 6 && j == 8))
            {
                gtk_style_context_add_class(context, "yellow");
            }

            if ((i == 7 && j == 7) )
            {
                gtk_button_set_label(GTK_BUTTON(button) , "Winner");
            }
            gtk_grid_attach(GTK_GRID(grid), button, j, i, 1, 1);
            g_signal_connect(button, "clicked", G_CALLBACK(on_click), coordinates);
        }
    }

    gtk_container_add(GTK_CONTAINER(window), grid);
    apply_css();
    gtk_widget_show_all(window);
    gtk_main();
}
