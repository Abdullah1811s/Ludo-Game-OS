#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "structs.h"
#include <time.h>
#define GRID_SIZE 15
#define CELL_SIZE 40

int turn_index = 0;
int dice_result = 0;
int all_token_in_home;
int isSix = 0;
int is_all_turn_completed[] = {
    0, // make it ture if the red turn is completed
    0, // make it ture if the blue turn is completed
    0, // make it ture if the yellow turn is completed
    0, // make it ture if the green turn is completed
};
int home_coordinate[4][4][2] = {
    // Red tokens' coordinates (x, y)
    {{11, 2}, {11, 3}, {12, 2}, {12, 3}},

    // Blue tokens' coordinates (x, y)
    {{2, 2}, {3, 2}, {3, 3}, {2, 3}},

    // Yellow tokens' coordinates (x, y)
    {{2, 11}, {3, 11}, {3, 12}, {2, 12}},

    // Green tokens' coordinates (x, y)
    {{11, 11}, {12, 11}, {12, 12}, {11, 12}}};
GtkWidget *player_turn_label;
pthread_mutex_t dice_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond;
GtkWidget *grid;
GAsyncQueue *queue;
GAsyncQueue *queue1;
GAsyncQueue *token_button_signal;
GtkWidget *roll_dice_button;
GtkWidget *red_token_button;
GtkWidget *blue_token_button;
GtkWidget *green_token_button;
GtkWidget *yellow_token_button;

struct player_data turn_array[4];

//(x , y) for the backend
// when movement happen we chnage the them they represent like current pos
struct name_and_pos token1_red_name_pos = {11, 2, "R1"};
struct name_and_pos token2_red_name_pos = {11, 3, "R2"};
struct name_and_pos token3_red_name_pos = {12, 2, "R3"};
struct name_and_pos token4_red_name_pos = {12, 3, "R4"};

struct name_and_pos token1_blue_name_pos = {2, 2, "B1"};
struct name_and_pos token2_blue_name_pos = {3, 2, "B2"};
struct name_and_pos token3_blue_name_pos = {3, 3, "B3"};
struct name_and_pos token4_blue_name_pos = {2, 3, "B4"};

struct name_and_pos token1_yellow_name_pos = {2, 11, "Y1"};
struct name_and_pos token2_yellow_name_pos = {3, 11, "Y2"};
struct name_and_pos token3_yellow_name_pos = {3, 12, "Y3"};
struct name_and_pos token4_yellow_name_pos = {2, 12, "Y4"};

struct name_and_pos token1_green_name_pos = {11, 11, "G1"};
struct name_and_pos token2_green_name_pos = {12, 11, "G2"};
struct name_and_pos token3_green_name_pos = {12, 12, "G3"};
struct name_and_pos token4_green_name_pos = {11, 12, "G4"};

struct start_pos start_places[] = {
    {6, 1, "blue"},
    {13, 6, "red"},
    {8, 13, "green"},
    {1, 8, "yellow"},
};

// this is one single token
struct token red_token[] = {
    {&token1_red_name_pos, "red"},
    {&token2_red_name_pos, "red"},
    {&token3_red_name_pos, "red"},
    {&token4_red_name_pos, "red"}};

struct token blue_token[] = {

    {&token1_blue_name_pos, "blue"},
    {&token2_blue_name_pos, "blue"},
    {&token3_blue_name_pos, "blue"},
    {&token4_blue_name_pos, "blue"},
};

struct token yellow_token[] = {
    {&token1_yellow_name_pos, "yellow"},
    {&token2_yellow_name_pos, "yellow"},
    {&token3_yellow_name_pos, "yellow"},
    {&token4_yellow_name_pos, "yellow"},
};

struct token green_token[] = {
    {&token1_green_name_pos, "green"},
    {&token2_green_name_pos, "green"},
    {&token3_green_name_pos, "green"},
    {&token4_green_name_pos, "green"},
};

GtkWidget *make_grid();
void *manage_backend(void *arg);
int is_start_place(int i, int j, const char **color);
void assign_color(GtkStyleContext *context, int i, int j);
void apply_css();
void on_click(GtkWidget *wid, gpointer data);
void roll_dice(GtkWidget *widget, gpointer data);
void shuffle_turn();
void update_dice_label(GtkWidget *label);
void make_player_thread(struct player_data *player_array, size_t size);
void initialize_player(int nummber_of_player);
gboolean update_label(const gchar *message);
void get_player_dice_value(struct player_data *single_player);
gpointer *get_button_signal();
int get_dice_value();
void roll_again(GtkWidget *widget, gpointer data);
void *check_token_selection(void *arg);
int checking_all_turn();
int still_in_home(struct player_data *single_player);
void token_button(GtkWidget *button, gpointer data);
int do_movement(struct player_data *single_player, gchar *selected_token_name);

int main(int arg, char *argv[])
{
    gtk_init(&arg, &argv);
    queue = g_async_queue_new();
    queue1 = g_async_queue_new();
    token_button_signal = g_async_queue_new();
    pthread_t backend_thread;
    pthread_create(&backend_thread, NULL, manage_backend, NULL);
    GtkWidget *window = make_grid();
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

void *manage_backend(void *arg)
{
    initialize_player(4);
}

void shuffle_turn()
{
    srand(time(0));
    size_t size = sizeof(turn_array) / sizeof(turn_array[0]);
    for (int i = size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        struct player_data temp = turn_array[i];
        turn_array[i] = turn_array[j];
        turn_array[j] = temp;
    }
}

gboolean update_label(const gchar *message)
{
    gtk_label_set_text(GTK_LABEL(player_turn_label), message);

    return FALSE;
}

gpointer *get_button_signal()
{
    gpointer message = g_async_queue_pop(queue);
    if (!message)
        return NULL;
    return message;
}

int get_dice_value()
{
    gpointer message = get_button_signal();
    if (message)
    {
        return dice_result;
    }
    return -1;
}

void token_button(GtkWidget *button, gpointer data)
{
    const gchar *label = gtk_button_get_label(GTK_BUTTON(button));
    if (label)
    {
        gchar *label_copy = g_strdup(label);
        g_async_queue_push(token_button_signal, label_copy);
    }
    else
    {
        perror("Button has no label.\n");
    }
}

int get_again_rolled_dice_value()
{
    gpointer message = g_async_queue_pop(queue1);
    if (message)
    {
        return dice_result;
    }
    return -1;
}
int checking_all_turn()
{
    int all_completed = 1;
    for (int i = 0; i < 3; i++)
    {
        if (!is_all_turn_completed[i])
        {
            all_completed = 0;
            return all_completed;
        }
    }
    return all_completed;
}

/*

dice roll
    ask user to select token --> done
    wait for the button signal -->done
    button signale will return tye label -->done
    check the name of lebel in single_player -->done
    store the current x , y
    get the updated x, y
    show in UI
    update the x , y of the token selected\
    return from the movement fucntion
*/
/*
       APPROACH

       check the token selected

       first check if all the token are in home
           if yes then see the dice value at 0 index if 6 ask player to select a token for opening
           if no
               start a counter
               if(counetr > 1)
                   get the (x , y) and push it in golbal array
               ask player to select the token
               check if the token selected belong to the player
               if yes
                   match the (x , y) in golbal array with selected token
                   update the token x and y
                   move
   */
int still_in_home(struct player_data *single_player)
{
    char *player_name = single_player->player_name;
    int all_tokens_in_home = 1; 

    if (strcmp(player_name, "red") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (single_player->all_tokens[i].name_and_pos->x != home_coordinate[0][i][0] ||
                single_player->all_tokens[i].name_and_pos->y != home_coordinate[0][i][1])
            {
                all_tokens_in_home = 0;
                printf("Red token %d is NOT in home position at (%d, %d)\n", i + 1,
                       single_player->all_tokens[i].name_and_pos->x,
                       single_player->all_tokens[i].name_and_pos->y);
            }
        }
    }

    if (strcmp(player_name, "blue") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (single_player->all_tokens[i].name_and_pos->x != home_coordinate[1][i][0] ||
                single_player->all_tokens[i].name_and_pos->y != home_coordinate[1][i][1])
            {
                all_tokens_in_home = 0;
                printf("Blue token %d is NOT in home position at (%d, %d)\n", i + 1,
                       single_player->all_tokens[i].name_and_pos->x,
                       single_player->all_tokens[i].name_and_pos->y);
            }
        }
    }

    if (strcmp(player_name, "green") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (single_player->all_tokens[i].name_and_pos->x != home_coordinate[3][i][0] ||
                single_player->all_tokens[i].name_and_pos->y != home_coordinate[3][i][1])
            {
                all_tokens_in_home = 0;
                printf("Green token %d is NOT in home position at (%d, %d)\n", i + 1,
                       single_player->all_tokens[i].name_and_pos->x,
                       single_player->all_tokens[i].name_and_pos->y);
            }
        }
    }

    if (strcmp(player_name, "yellow") == 0)
    {
        for (int i = 0; i < 4; i++)
        {
            if (single_player->all_tokens[i].name_and_pos->x != home_coordinate[2][i][0] ||
                single_player->all_tokens[i].name_and_pos->y != home_coordinate[2][i][1])
            {
                all_tokens_in_home = 0;
                printf("Yellow token %d is NOT in home position at (%d, %d)\n", i + 1,
                       single_player->all_tokens[i].name_and_pos->x,
                       single_player->all_tokens[i].name_and_pos->y);
            }
        }
    }

    if (all_tokens_in_home)
    {
        printf("All tokens for player %s are in home position.\n", player_name);
        return 1;
        printf("return");
    }
    else
    {
        printf("Some tokens for player %s are NOT in home position.\n", player_name);
        return 0;
    }
}

int do_movement(struct player_data *single_player, gchar *selected_token_name)
{
    /*
    check if in home or not
    if all 4 in home check dice value
    if dice is 6 ask player to select toke

    */
}

void *check_token_selection(void *arg)
{

    int *done_with_movement = malloc(sizeof(int));
    struct player_data *single_player = (struct player_data *)arg;
    char *selected_token_name = NULL;
    int isMatch = 0;
    if (!single_player || !single_player->all_tokens)
    {
        printf("Error: Invalid player data or tokens\n");
        return NULL;
    }
    all_token_in_home = still_in_home(single_player);
    printf("%d" , all_token_in_home);
    if (all_token_in_home)
    {
        int dice_value = single_player->dice_value[0];
        printf("%d" , dice_value);
        while (!isMatch)
        {
            gpointer selected_token_info = g_async_queue_pop(token_button_signal);
            if (!selected_token_info)
            {
                printf("Error: Received NULL token info\n");
                continue;
            }

            selected_token_name = (char *)selected_token_info;
            if (!selected_token_name)
            {
                printf("Error: Invalid selected token name\n");
                continue;
            }

            printf("Received token info: %s\n", selected_token_name);

            for (int i = 0; i < 3; i++)
            {
                if (single_player->all_tokens[i].name_and_pos &&
                    single_player->all_tokens[i].name_and_pos->name &&
                    strcmp(selected_token_name, single_player->all_tokens[i].name_and_pos->name) == 0)
                {
                    printf("The token is matched with %s\n", single_player->all_tokens[i].name_and_pos->name);
                    isMatch = 1;
                    return selected_token_name;
                }
            }

            if (!isMatch)
            {
                printf("Token mismatch. Please try again.\n");
                g_idle_add((GSourceFunc)update_label, "Please select the right token");
            }
        }
        if (dice_value == 6)
        {
            printf("chosing a token dice > 6\n");
            g_idle_add((GSourceFunc)update_label, "Please select the token for movement");
        }
    }
}

void get_player_dice_value(struct player_data *single_player)
{
    int single_player_dice_value = get_dice_value();
    pthread_mutex_lock(&dice_mutex);
    printf("The dice is locked by the player name %s\n", single_player->player_name);
    printf("The value of dice is %d\n", single_player_dice_value);
    single_player->dice_value[0] = single_player_dice_value;
    g_idle_add((GSourceFunc)update_label, single_player->player_name);

    if (single_player_dice_value == 6)
    {
        int counter = 1;
        g_idle_add((GSourceFunc)update_label, "Roll again\n");
        printf("Player %s rolled a 6. Rolling again...\n", single_player->player_name);
        single_player_dice_value = get_again_rolled_dice_value();
        single_player->dice_value[1] = single_player_dice_value;

        while (single_player_dice_value == 6)
        {
            counter++;
            single_player_dice_value = get_again_rolled_dice_value();
            single_player->dice_value[2] = single_player_dice_value;
            if (counter == 3)
            {
                break;
            }
        }

        printf("Player %s rolled again and got %d\n", single_player->player_name, single_player_dice_value);
    }

    pthread_t movement_thread;
    printf("calling thread\n");
    pthread_create(&movement_thread, NULL, check_token_selection, (void *)single_player);
    int *done_with_movement;
    pthread_join(movement_thread, (void **)&done_with_movement);
    if (done_with_movement && *done_with_movement)
    {
        printf("Player %s completed their turn, unlocking the mutex\n", single_player->player_name);
        free(done_with_movement);
        pthread_mutex_unlock(&dice_mutex);
    }
    else
    {
        printf("Not done with the movement\n");
        free(done_with_movement);
        printf("unclocking the mutex\n");
        pthread_mutex_unlock(&dice_mutex);
    }
}

void *manage_player_thread(void *arg)
{
    struct player_data *single_player = (struct player_data *)arg;
    while (1)
    {
        get_player_dice_value(single_player);
    }
}

void make_player_thread(struct player_data *player_array, size_t size)
{
    pthread_t *player_thread_array = (pthread_t *)malloc(size * sizeof(pthread_t));
    for (int i = 0; i < size; i++)
    {
        pthread_create(&player_thread_array[i], NULL, manage_player_thread, (void *)&player_array[i]);
    }
    for (int i = 0; i < size; i++)
    {
        pthread_join(player_thread_array[i], NULL);
    }
    printf("all thread completed\n");
}

void initialize_player(int number_of_player)
{
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
        turn_array[i].can_go_home = 0;
        printf("\n");
    }

    make_player_thread(turn_array, number_of_player);
    printf("All the players have been initialized.\n");
}

void on_click(GtkWidget *widget, gpointer data)
{
    struct xy_coordinate *coordinates = (struct xy_coordinate *)data;
    printf("Button clicked at position row and col: (%d, %d)\n", coordinates->x, coordinates->y);
}

void roll_again(GtkWidget *widget, gpointer data)
{
    srand(time(0));
    dice_result = (rand() % 6) + 1;
    g_async_queue_push(queue1, "Start");
    update_dice_label((GtkWidget *)data);
}

void roll_dice(GtkWidget *widget, gpointer data)
{
    srand(time(0));
    dice_result = (rand() % 6) + 1;
    g_async_queue_push(queue, "Start");
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

GtkWidget *make_grid()
{
    GtkWidget *window;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Ludo Grid");
    int window_size = 900;
    int cell_size = window_size / GRID_SIZE;

    gtk_window_set_default_size(GTK_WINDOW(window), window_size, window_size);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    grid = gtk_fixed_new();
    GtkWidget *button;

    int num_tokens = sizeof(tokens) / sizeof(tokens[0]);
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            button = gtk_button_new();
            GtkStyleContext *context = gtk_widget_get_style_context(button);

            struct xy_coordinate *coordinates = g_malloc(sizeof(struct xy_coordinate));
            coordinates->x = i;
            coordinates->y = j;

            int x_position = j * cell_size;
            int y_position = i * cell_size;

            const char *start_color = NULL;
            int is_token_position = 0;
            for (int k = 0; k < num_tokens; k++)
            {
                if (i == tokens[k].x && j == tokens[k].y)
                {
                    is_token_position = 1;
                    break;
                }
            }
            if (is_token_position)
                continue;
            else
            {
                if (is_start_place(i, j, &start_color))
                {
                    gtk_style_context_add_class(context, start_color);
                }
                else
                {
                    assign_color(context, i, j);
                }

                gtk_fixed_put(GTK_FIXED(grid), button, x_position, y_position);
                gtk_widget_set_size_request(button, cell_size, cell_size);
                g_signal_connect(button, "clicked", G_CALLBACK(on_click), coordinates);
            }
        }
    }

    // adding red token
    for (int i = 0; i < 4; i++)
    {
        red_token_button = gtk_button_new_with_label(red_token[i].name_and_pos->name);
        g_signal_connect(red_token_button, "clicked", G_CALLBACK(token_button), NULL);
        GtkStyleContext *context = gtk_widget_get_style_context(red_token_button);
        gtk_style_context_add_class(context, "red");
        gtk_fixed_put(GTK_FIXED(grid),
                      red_token_button,
                      red_token[i].name_and_pos->y * cell_size,
                      red_token[i].name_and_pos->x * cell_size);
    }
    // adding blue token
    for (int i = 0; i < 4; i++)
    {

        blue_token_button = gtk_button_new_with_label(blue_token[i].name_and_pos->name);
        g_signal_connect(blue_token_button, "clicked", G_CALLBACK(token_button), NULL);
        GtkStyleContext *context = gtk_widget_get_style_context(blue_token_button);
        gtk_style_context_add_class(context, "blue");
        gtk_fixed_put(GTK_FIXED(grid),
                      blue_token_button,
                      blue_token[i].name_and_pos->y * cell_size,
                      blue_token[i].name_and_pos->x * cell_size);
    }
    // adding yellow token
    for (int i = 0; i < 4; i++)
    {
        yellow_token_button = gtk_button_new_with_label(yellow_token[i].name_and_pos->name);
        g_signal_connect(yellow_token_button, "clicked", G_CALLBACK(token_button), NULL);
        GtkStyleContext *context = gtk_widget_get_style_context(yellow_token_button);
        gtk_style_context_add_class(context, "yellow");
        gtk_fixed_put(GTK_FIXED(grid),
                      yellow_token_button,
                      yellow_token[i].name_and_pos->y * cell_size,
                      yellow_token[i].name_and_pos->x * cell_size);
    }
    // adding green token
    for (int i = 0; i < 4; i++)
    {
        green_token_button = gtk_button_new_with_label(green_token[i].name_and_pos->name);
        g_signal_connect(green_token_button, "clicked", G_CALLBACK(token_button), NULL);
        GtkStyleContext *context = gtk_widget_get_style_context(green_token_button);
        gtk_style_context_add_class(context, "green");
        gtk_fixed_put(GTK_FIXED(grid),
                      green_token_button,
                      green_token[i].name_and_pos->y * cell_size,
                      green_token[i].name_and_pos->x * cell_size);
    }

    GtkWidget *dice_controls_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *dice_label = gtk_label_new("Dice Result: ");
    roll_dice_button = gtk_button_new_with_label("Roll Dice");
    GtkWidget *roll_again_button = gtk_button_new_with_label("Roll Again");

    gtk_style_context_add_class(gtk_widget_get_style_context(roll_dice_button), "roll-dice-button");
    gtk_style_context_add_class(gtk_widget_get_style_context(dice_label), "dice-result-label");
    gtk_style_context_add_class(gtk_widget_get_style_context(roll_again_button), "roll-again-button");

    g_signal_connect(roll_dice_button, "clicked", G_CALLBACK(roll_dice), dice_label);
    g_signal_connect(roll_again_button, "clicked", G_CALLBACK(roll_again), dice_label);

    gtk_box_pack_start(GTK_BOX(dice_controls_box), roll_dice_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dice_controls_box), roll_again_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dice_controls_box), dice_label, FALSE, FALSE, 0);

    player_turn_label = gtk_label_new("Turn: ");
    GtkWidget *bottom_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(bottom_box), player_turn_label, FALSE, FALSE, 10);

    GtkWidget *bottom_controls_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(bottom_controls_box), dice_controls_box, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(bottom_controls_box), bottom_box, FALSE, FALSE, 0);

    gtk_widget_set_halign(player_turn_label, GTK_ALIGN_END);
    gtk_widget_set_valign(player_turn_label, GTK_ALIGN_CENTER);
    gtk_style_context_add_class(gtk_widget_get_style_context(player_turn_label), "dice-result-label");

    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), bottom_controls_box, FALSE, FALSE, 10);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    apply_css();
    return window;
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
            gtk_style_context_add_class(context, "yellow");
        else if (i >= 9 && i <= 13)
            gtk_style_context_add_class(context, "red");
    }
    else if (i == 7)
    {
        if (j >= 1 && j <= 5)
            gtk_style_context_add_class(context, "blue");
        else if (j >= 9 && j <= 13)
            gtk_style_context_add_class(context, "green");
    }
    else if (i <= 5 && j <= 5)
    {
        if (i > 0 && i < 5 && j > 0 && j < 5)
            gtk_style_context_add_class(context, "transparent-inner");
        else
            gtk_style_context_add_class(context, "top-left");
    }
    else if (i <= 5 && j >= 9)
    {
        if (i > 0 && i < 5 && j > 9 && j < 14)
            gtk_style_context_add_class(context, "transparent-inner");
        else
            gtk_style_context_add_class(context, "top-right");
    }
    else if (i >= 9 && j <= 5)
    {
        if (i > 9 && i < 14 && j > 0 && j < 5)
            gtk_style_context_add_class(context, "transparent-inner");
        else
            gtk_style_context_add_class(context, "bottom-left");
    }
    else if (i >= 9 && j >= 9)
    {
        if (i > 9 && i < 14 && j > 9 && j < 14)
            gtk_style_context_add_class(context, "transparent-inner");
        else
            gtk_style_context_add_class(context, "bottom-right");
    }
}
