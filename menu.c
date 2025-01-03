#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>

void start_program(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog, *content_area, *entry, *label;
    GtkDialogFlags flags = GTK_DIALOG_MODAL;
    int result;

    // Create a dialog box
    dialog = gtk_dialog_new_with_buttons("Enter Number of Players",
                                         GTK_WINDOW(data),
                                         flags,
                                         "_OK",
                                         GTK_RESPONSE_OK,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         NULL);

    // Get the content area of the dialog
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Create a label and entry field
    label = gtk_label_new("Enter number of players (1 to 4):");
    entry = gtk_entry_new();

    // Add the label and entry to the dialog's content area
    gtk_box_pack_start(GTK_BOX(content_area), label, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(content_area), entry, TRUE, TRUE, 5);

    // Show the dialog's widgets
    gtk_widget_show_all(dialog);

    // Run the dialog and wait for a response
    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_OK) {
        const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
        int num_players = atoi(text); // Convert the input to an integer

        // Validate the number of players
        if (num_players >= 1 && num_players <= 4) {
            g_print("Number of players: %d\n", num_players);

            // Construct a string for the player count to pass as an argument
            char num_players_str[10];
            snprintf(num_players_str, sizeof(num_players_str), "%d", num_players);

            // Path to the program to execute
            char *program = "./s";

            // Execute the new program with the player count as an argument
            execl(program, program, num_players_str, (char *)NULL);

            // If execl fails, print an error message
            perror("execl failed");
            exit(EXIT_FAILURE);
        } else {
            g_print("Invalid number of players. Please enter a value between 1 and 4.\n");
        }
    }

    // Destroy the dialog after response
    gtk_widget_destroy(dialog);
}

void exit_program(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button_start;
    GtkWidget *button_exit;

    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create a new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Menu");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Connect the close button to exit the program
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to arrange buttons
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the Start button
    button_start = gtk_button_new_with_label("Start");
    g_signal_connect(button_start, "clicked", G_CALLBACK(start_program), window);
    gtk_box_pack_start(GTK_BOX(vbox), button_start, TRUE, TRUE, 0);

    // Create the Exit button
    button_exit = gtk_button_new_with_label("Exit");
    g_signal_connect(button_exit, "clicked", G_CALLBACK(exit_program), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_exit, TRUE, TRUE, 0);

    // Show all widgets
    gtk_widget_show_all(window);

    // Run the GTK main loop
    gtk_main();

    return 0;
}
