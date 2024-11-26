#include <gtk/gtk.h>
#include <stdio.h>

#define GRID_SIZE 15

void make_grid(int arg, char *argv[]);
void on_click(GtkWidget *wid, gpointer data);
void make_red_player_grid(GtkWidget *grid);
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
void make_red_player_grid(GtkWidget *grid)
{

}
void make_grid(int arg, char *argv[])
{
    gtk_init(&arg, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Window");
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
            gtk_grid_attach(GTK_GRID(grid), button, i, j, 1, 1);
            g_signal_connect(button, "clicked", G_CALLBACK(on_click), coordinates);
        }
    }
    // make the red part

    /*
        lets make an array like this ['TL' , 'TR' , 'BL' , 'BR'] --> home place for player
        then we add a swith case and base on like if 'TL' we add color on the four place based on x and y cooridnate 
    */
    make_red_player_grid(grid);
    gtk_container_add(GTK_CONTAINER(window), grid);
    gtk_widget_show_all(window);
    gtk_main();
}
