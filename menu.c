#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>

void start_program(GtkWidget *widget, gpointer data) {
    // Path to the program to execute
    char *program = "./m";

    // Replace the current process with the new program
    execl(program, program, (char *)NULL);

    // If exec fails, print an error
    perror("execl failed");
    exit(EXIT_FAILURE);
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
    g_signal_connect(button_start, "clicked", G_CALLBACK(start_program), NULL);
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
