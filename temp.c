#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct
{
    GtkWidget *button;
    int target_x;
    int target_y;
    int current_x;
    int current_y;
    int step_x;
    int step_y;
    gboolean is_moving;
} ButtonData;

gboolean move_button_step(gpointer data)
{
    ButtonData *button_data = (ButtonData *)data;

    if (!button_data->is_moving)
    {
        return G_SOURCE_REMOVE;
    }

    // Calculate the new position
    if (button_data->current_x != button_data->target_x)
    {
        button_data->current_x += button_data->step_x;
        if ((button_data->step_x > 0 && button_data->current_x >= button_data->target_x) ||
            (button_data->step_x < 0 && button_data->current_x <= button_data->target_x))
        {
            button_data->current_x = button_data->target_x;
        }
    }

    if (button_data->current_y != button_data->target_y)
    {
        button_data->current_y += button_data->step_y;
        if ((button_data->step_y > 0 && button_data->current_y >= button_data->target_y) ||
            (button_data->step_y < 0 && button_data->current_y <= button_data->target_y))
        {
            button_data->current_y = button_data->target_y;
        }
    }

    // Move the button to the current position
    gtk_fixed_move(GTK_FIXED(gtk_widget_get_parent(button_data->button)),
                   button_data->button,
                   button_data->current_x,
                   button_data->current_y);

    
    if (button_data->current_x == button_data->target_x && button_data->current_y == button_data->target_y)
    {
        button_data->is_moving = FALSE;
        g_free(button_data);
        return G_SOURCE_REMOVE;
    }

    return G_SOURCE_CONTINUE;
}

void on_button_clicked(GtkButton *button, gpointer user_data)
{
    int target_x = 200; 
    int target_y = 200; 
    GtkAllocation allocation;
    gtk_widget_get_allocation(GTK_WIDGET(button), &allocation); // Cast to GtkWidget*

    int current_x = allocation.x;
    int current_y = allocation.y;

    ButtonData *button_data = g_malloc(sizeof(ButtonData));
    button_data->button = GTK_WIDGET(button); // Store as GtkWidget*
    button_data->target_x = target_x;
    button_data->target_y = target_y;
    button_data->current_x = current_x;
    button_data->current_y = current_y;
    button_data->is_moving = TRUE;
    button_data->step_x = (target_x - current_x) / 10;
    button_data->step_y = (target_y - current_y) / 10;
    g_timeout_add(80, move_button_step, button_data);
}

void load_css(const char *css_path)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(
        screen, GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GError *error = NULL;
    gtk_css_provider_load_from_path(provider, css_path, &error);

    if (error)
    {
        g_warning("Error loading CSS file: %s", error->message);
        g_error_free(error);
    }

    g_object_unref(provider);
}
GtkWidget *make_grid(int grid_rows, int grid_cols, int cell_size)
{
    GtkWidget *fixed = gtk_fixed_new();
    GtkWidget *button;
    for (int row = 0; row < grid_rows; row++)
    {
        for (int col = 0; col < grid_cols; col++)
        {
            button = gtk_button_new();

            int x_position = col * cell_size;
            int y_position = row * cell_size;
            gtk_fixed_put(GTK_FIXED(fixed), button, x_position, y_position);
            gtk_widget_set_size_request(button, cell_size - 2, cell_size - 2);
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
        }
    }
    button = gtk_button_new();
    GtkStyleContext *context = gtk_widget_get_style_context(button);
    gtk_style_context_add_class(context, "blue"); 
    gtk_fixed_put(GTK_FIXED(fixed), button, 40, 40);
    gtk_widget_set_size_request(button, cell_size - 2, cell_size - 2);
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
    return fixed;
}


int main(int argc, char *argv[])
{
    GtkWidget *window;
    gtk_init(&argc, &argv);
    load_css("style.css");
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "15x15 Ludo Grid");
    gtk_window_set_default_size(GTK_WINDOW(window), 0, 650);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget *grid = make_grid(15, 15, 40);
    gtk_container_add(GTK_CONTAINER(window), grid);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}



/*
 if (all_token_in_home)
    { 
        g_idle_add((GSourceFunc)update_label, "get 6 to unlock token");
        done_with_movement = 0;
        return (void *)done_with_movement;
    }
    else
    {
        gchar *selected_token_name;
        int isMatch = 0;
        while (!isMatch)
        {
            gpointer selected_token_info = g_async_queue_pop(token_button_signal);
            if (selected_token_info != NULL)
            {

                selected_token_name = (gchar *)selected_token_info;
                printf("The selected token is %s\n", selected_token_name);
                for (int i = 0; i < 4; i++)
                {
                    if (strcmp(selected_token_name, single_player->all_tokens[i].name_and_pos->name) == 0)
                    {
                        printf("The token is matched with %s ", single_player->all_tokens[i].name_and_pos->name);
                        isMatch = 1;
                        break;
                    }
                    else
                    {
                        g_idle_add((GSourceFunc)update_label, "please select the right token");
                    }
                }
            }
            g_free(selected_token_info);
            printf("player select the wrong token going in loop again\n");
        }
      
        g_idle_add((GSourceFunc)update_label, "player doing the movement");
        // add movement login
        printf("Player Name: %s\n", single_player->player_name);
        printf("Dice Values: %d, %d, %d\n", single_player->dice_value[0], single_player->dice_value[1], single_player->dice_value[2]);
        printf("Can Go Home: %s\n", single_player->can_go_home ? "Yes" : "No");

        // g_free(selected_token_info);
        *done_with_movement = 1;
        g_idle_add((GSourceFunc)update_label, "next player turn");
        return (void *)done_with_movement;
    }
*/