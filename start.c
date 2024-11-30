#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "structs.h"
#define GRID_SIZE 15

int dice_result = 0;
int is_grid_initialized = 0;
pthread_mutex_t dice_mutex;
GtkWidget *grid;
GtkWidget *dice_label;
struct player_data turn_array[4];

void make_grid();
int is_start_place(int i, int j, const char **color);
void assign_color(GtkStyleContext *context, int i, int j);
void apply_css();
void on_click(GtkWidget *wid, gpointer data);
int dice_value();
void roll_dice(GtkWidget *widget, gpointer data);
void shuffle_turn(struct player_data *array);
void update_dice_label(GtkWidget *label);
void make_player_thread(struct player_data *player_array , size_t size);
void initialize_player(int nummber_of_player);

struct name_and_pos token1_red_name_pos = {11, 2, "RT1"};
struct name_and_pos token2_red_name_pos = {11, 3, "RT2"};
struct name_and_pos token3_red_name_pos = {12, 2, "RT3"};
struct name_and_pos token4_red_name_pos = {12, 3, "RT4"};

struct name_and_pos token1_blue_name_pos = {2, 2, "BT1"};
struct name_and_pos token2_blue_name_pos = {3, 2, "BT2"};
struct name_and_pos token3_blue_name_pos = {3, 3, "BT3"};
struct name_and_pos token4_blue_name_pos = {2, 3, "BT4"};

struct name_and_pos token1_yellow_name_pos = {2, 11, "YT1"};
struct name_and_pos token2_yellow_name_pos = {3, 11, "YT2"};
struct name_and_pos token3_yellow_name_pos = {3, 12, "YT3"};
struct name_and_pos token4_yellow_name_pos = {2, 12, "YT4"};

struct name_and_pos token1_green_name_pos = {11, 11, "GT1"};
struct name_and_pos token2_green_name_pos = {12, 11, "GT2"};
struct name_and_pos token3_green_name_pos = {12, 12, "GT3"};
struct name_and_pos token4_green_name_pos = {11, 12, "GT4"};

struct start_pos start_places[] = {
    {6, 1, "blue"},
    {13, 6, "red"},
    {8, 13, "green"},
    {1, 8, "yellow"}};

struct token red_token[] = {
    {&token1_red_name_pos, "red"},
    {&token2_red_name_pos, "red"},
    {&token3_red_name_pos, "red"},
    {&token4_red_name_pos, "red"}};

struct token blue_token[] = {
    {&token1_blue_name_pos, "blue"},
    {&token2_blue_name_pos, "blue"},
    {&token3_blue_name_pos, "blue"},
    {&token4_blue_name_pos, "blue"}};

struct token yellow_token[] = {
    {&token1_yellow_name_pos, "yellow"},
    {&token2_yellow_name_pos, "yellow"},
    {&token3_yellow_name_pos, "yellow"},
    {&token4_yellow_name_pos, "yellow"}};

struct token green_token[] = {
    {&token1_green_name_pos, "green"},
    {&token2_green_name_pos, "green"},
    {&token3_green_name_pos, "green"},
    {&token4_green_name_pos, "green"}};

int main(int arg, char *argv[])
{
    gtk_init(&arg, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Ludo Game");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    make_grid();

    GtkWidget *dice_controls_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *dice_label = gtk_label_new("Dice Result: ");
    GtkWidget *roll_dice_button = gtk_button_new_with_label("Roll Dice");

    gtk_style_context_add_class(gtk_widget_get_style_context(roll_dice_button), "roll-dice-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(dice_label), "dice-result-label");

    g_signal_connect(roll_dice_button, "clicked", G_CALLBACK(roll_dice), dice_label);

    gtk_box_pack_start(GTK_BOX(dice_controls_box), roll_dice_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dice_controls_box), dice_label, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), dice_controls_box, FALSE, FALSE, 10);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    apply_css();
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

void shuffle_turn(struct player_data *array)
{
    srand(time(0));
    size_t size = sizeof(array) / sizeof(array[0]);
    for (int i = size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        struct player_data temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void make_player_thread(struct player_data *player_array , size_t size)
{
    
}

void initialize_player(int number_of_player)
{

    /*
        now the player intialize function get called only when the grid is done
        sleep work fine

    */
    printf("in fucntion\n");

    struct token all_tokens[4][4] = {
        {
            {&token1_red_name_pos, "red"},
            {&token2_red_name_pos, "red"},
            {&token3_red_name_pos, "red"},
            {&token4_red_name_pos, "red"},
        },
        {
            {&token1_blue_name_pos, "blue"},
            {&token2_blue_name_pos, "blue"},
            {&token3_blue_name_pos, "blue"},
            {&token4_blue_name_pos, "blue"},
        },
        {
            {&token1_yellow_name_pos, "yellow"},
            {&token2_yellow_name_pos, "yellow"},
            {&token3_yellow_name_pos, "yellow"},
            {&token4_yellow_name_pos, "yellow"},
        },
        {
            {&token1_green_name_pos, "green"},
            {&token2_green_name_pos, "green"},
            {&token3_green_name_pos, "green"},
            {&token4_green_name_pos, "green"},
        }};

    for (int i = 0; i < number_of_player; i++)
    {

        turn_array[i].player_name = all_tokens[i][0].color;
        turn_array[i].all_tokens = all_tokens[i];
        turn_array[i].dice_value = dice_result;
        turn_array[i].is_turn_completed = -1;
        turn_array[i].can_go_home = -1;
        printf("\n");
    }
    
    make_player_thread(turn_array , number_of_player);
    printf("All the players have been initialized.\n");
}

gboolean initialize_player_after_delay(gpointer data)
{
    initialize_player(4);
    printf("All the players have been initialized.\n");
    return FALSE;
}

void on_click(GtkWidget *widget, gpointer data)
{
    struct xy_coordinate *coordinates = (struct xy_coordinate *)data;
    printf("Button clicked at position row and col: (%d, %d)\n", coordinates->x, coordinates->y);
}

int dice_value()
{
    srand(time(0));
    return (rand() % 6) + 1;
}

void roll_dice(GtkWidget *widget, gpointer data)
{
    dice_result = dice_value();
    printf("Dice Rolled: %d\n", dice_result);
    update_dice_label((GtkWidget *)data);
}

void update_dice_label(GtkWidget *label)
{
    char dice_text[50];
    sprintf(dice_text, "Dice Result: %d", dice_result);
    gtk_label_set_text(GTK_LABEL(label), dice_text);
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

void make_grid()
{
    grid = gtk_grid_new();

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            GtkWidget *button = gtk_button_new_with_label("");
            GtkStyleContext *context = gtk_widget_get_style_context(button);
            struct xy_coordinate *coordinates = g_malloc(sizeof(struct xy_coordinate));
            coordinates->x = i;
            coordinates->y = j;

            const char *start_color = NULL;
            if (is_start_place(i, j, &start_color))
            {
                gtk_style_context_add_class(context, start_color);
            }
            else
            {
                assign_color(context, i, j);
            }

            gtk_grid_attach(GTK_GRID(grid), button, j, i, 1, 1);
            g_signal_connect(button, "clicked", G_CALLBACK(on_click), coordinates);
        }
    }
    printf("The grid has been initialized changing the value of global varaiable\n");
    g_timeout_add(1000, initialize_player_after_delay, NULL);
}

int is_start_place(int i, int j, const char **color)
{
    for (int k = 0; k < sizeof(start_places) / sizeof(start_places[0]); k++)
    {
        if (start_places[k].x == i && start_places[k].y == j)
        {
            *color = start_places[k].color;
            return 1;
        }
    }
    return 0;
}

void assign_color(GtkStyleContext *context, int i, int j)
{
    if (j == 7)
    {
        if (i >= 1 && i <= 5)
        {
            gtk_style_context_add_class(context, "yellow");
        }
        else if (i >= 9 && i <= 13)
        {
            gtk_style_context_add_class(context, "red");
        }
    }
    else if (i == 7)
    {
        if (j >= 1 && j <= 5)
        {
            gtk_style_context_add_class(context, "blue");
        }
        else if (j >= 9 && j <= 13)
        {
            gtk_style_context_add_class(context, "green");
        }
    }
    else if (i <= 5 && j <= 5)
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
    else if (i <= 5 && j >= 9)
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
    else if (i >= 9 && j <= 5)
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
    else if (i >= 9 && j >= 9)
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
}
