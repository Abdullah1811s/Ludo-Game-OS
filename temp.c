#include <gtk/gtk.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

GtkWidget *grid; // Global grid pointer

// Function to add a label to the grid (runs in the main thread)
gboolean add_to_grid(gpointer data) {
    static int count = 0; // Track row number
    char *text = (char *)data;

    GtkWidget *label = gtk_label_new(text);

    // Attach label to the grid
    gtk_grid_attach(GTK_GRID(grid), label, 0, count++, 1, 1);
    gtk_widget_show(label); // Ensure the label is visible

    return G_SOURCE_REMOVE; // Remove this callback after execution
}

// Worker thread function
void *worker_thread(void *arg) {
   
    for (int i = 1; i <= 5; i++) {
        char *text = g_strdup_printf("Row %d added by thread", i);
        
        // Queue the task to the GTK main loop
        g_idle_add(add_to_grid, text);

        sleep(1); // Simulate delay between updates
    }

    return NULL;
}

// Main function
int main(int argc, char *argv[]) {
    GtkWidget *window;
    pthread_t thread;

    gtk_init(&argc, &argv);

    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Grid Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // Create a grid and attach it to the window
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Connect the "destroy" signal to quit the GTK main loop
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Start the worker thread
    pthread_create(&thread, NULL, worker_thread, NULL);

    // Show the window and grid
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    // Wait for the worker thread to finish
    pthread_join(thread, NULL);

    return 0;
}
