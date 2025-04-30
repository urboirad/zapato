#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SNEAKERS 500

void on_add_sneaker(GtkButton *button, gpointer user_data);
void on_delete_sneaker(GtkButton *button, gpointer user_data);
void on_export_list(GtkButton *button, gpointer user_data);
void on_edit_sneaker(GtkButton *button, gpointer user_data);

typedef struct {
    char brand[50];
    char model[50];
    char style_code[20];
    int room_number; // 1, 2, 3
} Sneaker;

Sneaker sneakers[MAX_SNEAKERS];
int sneaker_count = 0;

// Widgets
// Widgets
GtkWidget *sneaker_tree_view;
GtkListStore *sneaker_list_store;
GtkWidget *brand_entry;
GtkWidget *model_entry;
GtkWidget *style_entry;

int assign_room(const Sneaker *s) {
    // Make brand/model lowercase to search
    char brand_lower[50], model_lower[50];
    strcpy(brand_lower, s->brand);
    strcpy(model_lower, s->model);
    for (char *p = brand_lower; *p; ++p) *p = tolower(*p);
    for (char *p = model_lower; *p; ++p) *p = tolower(*p);

    if (strstr(model_lower, "9060") || strstr(model_lower, "runner") || strstr(model_lower, "ultraboost")) {
        return 1; // Running / Retro Tech
    }
    if (strstr(brand_lower, "jordan") || strstr(model_lower, "dunk") || strstr(model_lower, "lebron")) {
        return 2; // Men's Basketball / Women's
    }
    if (strstr(model_lower, "kids") || strstr(model_lower, "gs") || strstr(model_lower, "ps")) {
        return 3; // Kids
    }
    return 2; // Default
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window, *grid, *add_button, *export_button, *scroll_window;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "zapato");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Brand input
    brand_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(brand_entry), "Brand");
    gtk_grid_attach(GTK_GRID(grid), brand_entry, 0, 0, 1, 1);

    // Model input
    model_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(model_entry), "Model");
    gtk_grid_attach(GTK_GRID(grid), model_entry, 1, 0, 1, 1);

    // Style Code input
    style_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(style_entry), "Style Code");
    gtk_grid_attach(GTK_GRID(grid), style_entry, 2, 0, 1, 1);

    // Add button
    add_button = gtk_button_new_with_label("Add Sneaker");
    gtk_grid_attach(GTK_GRID(grid), add_button, 0, 1, 3, 1);
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_sneaker), NULL);

    // Export button
    export_button = gtk_button_new_with_label("Export to CSV");
    gtk_grid_attach(GTK_GRID(grid), export_button, 0, 2, 3, 1);
    g_signal_connect(export_button, "clicked", G_CALLBACK(on_export_list), NULL);

    // Delete button
    GtkWidget *delete_button;
    delete_button = gtk_button_new_with_label("Delete Selected Sneaker");
    gtk_grid_attach(GTK_GRID(grid), delete_button, 0, 4, 3, 1);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_sneaker), NULL);

    // Edit button
    GtkWidget *edit_button;
    edit_button = gtk_button_new_with_label("Edit Selected Sneaker");
    gtk_grid_attach(GTK_GRID(grid), edit_button, 0, 5, 3, 1);
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_sneaker), NULL);



    // Sneaker List (scrollable text view)
    scroll_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scroll_window, TRUE);
    gtk_grid_attach(GTK_GRID(grid), scroll_window, 0, 3, 3, 1);

    // Create list store (4 columns: brand, model, style_code, room_number)
    sneaker_list_store = gtk_list_store_new(4,
        G_TYPE_STRING, // Brand
        G_TYPE_STRING, // Model
        G_TYPE_STRING, // Style Code
        G_TYPE_INT     // Room Number
    );

    // Create TreeView and link to the list store
    sneaker_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(sneaker_list_store));
    gtk_container_add(GTK_CONTAINER(scroll_window), sneaker_tree_view);

    // Add columns to the TreeView
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Brand column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Brand", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_sort_column_id(column, 0);
    gtk_tree_view_append_column(GTK_TREE_VIEW(sneaker_tree_view), column);

    // Model column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Model", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_sort_column_id(column, 1);
    gtk_tree_view_append_column(GTK_TREE_VIEW(sneaker_tree_view), column);

    // Style Code column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Style Code", renderer, "text", 2, NULL);
    gtk_tree_view_column_set_sort_column_id(column, 2);
    gtk_tree_view_append_column(GTK_TREE_VIEW(sneaker_tree_view), column);

    // Room column
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Room", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_sort_column_id(column, 3);
    gtk_tree_view_append_column(GTK_TREE_VIEW(sneaker_tree_view), column);


    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

void refresh_sneaker_list() {
    gtk_list_store_clear(sneaker_list_store); // Clear old rows

    for (int i = 0; i < sneaker_count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(sneaker_list_store, &iter);
        gtk_list_store_set(sneaker_list_store, &iter,
            0, sneakers[i].brand,
            1, sneakers[i].model,
            2, sneakers[i].style_code,
            3, sneakers[i].room_number,
            -1);
    }
}

void on_add_sneaker(GtkButton *button, gpointer user_data) {
    if (sneaker_count >= MAX_SNEAKERS)
        return;

    Sneaker *s = &sneakers[sneaker_count];

    strcpy(s->brand, gtk_entry_get_text(GTK_ENTRY(brand_entry)));
    strcpy(s->model, gtk_entry_get_text(GTK_ENTRY(model_entry)));
    strcpy(s->style_code, gtk_entry_get_text(GTK_ENTRY(style_entry)));
    s->room_number = assign_room(s);

    sneaker_count++;

    refresh_sneaker_list();

    // Clear input fields
    gtk_entry_set_text(GTK_ENTRY(brand_entry), "");
    gtk_entry_set_text(GTK_ENTRY(model_entry), "");
    gtk_entry_set_text(GTK_ENTRY(style_entry), "");
}

void on_delete_sneaker(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(sneaker_tree_view));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        // Get the selected row number
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        int *indices = gtk_tree_path_get_indices(path);
        int row = indices[0];

        // Shift all sneakers after the selected one up by one
        for (int i = row; i < sneaker_count - 1; i++) {
            sneakers[i] = sneakers[i+1];
        }

        sneaker_count--;

        gtk_tree_path_free(path);

        refresh_sneaker_list();
    }
}

void on_edit_sneaker(GtkButton *button, gpointer user_data) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(sneaker_tree_view));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        int *indices = gtk_tree_path_get_indices(path);
        int row = indices[0];

        // Create a popup window for editing
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Edit Sneaker",
                                                        NULL,
                                                        GTK_DIALOG_MODAL,
                                                        "_Cancel", GTK_RESPONSE_CANCEL,
                                                        "_Save", GTK_RESPONSE_ACCEPT,
                                                        NULL);
        GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *grid = gtk_grid_new();
        gtk_container_add(GTK_CONTAINER(content), grid);

        GtkWidget *edit_brand = gtk_entry_new();
        GtkWidget *edit_model = gtk_entry_new();
        GtkWidget *edit_style = gtk_entry_new();

        gtk_entry_set_text(GTK_ENTRY(edit_brand), sneakers[row].brand);
        gtk_entry_set_text(GTK_ENTRY(edit_model), sneakers[row].model);
        gtk_entry_set_text(GTK_ENTRY(edit_style), sneakers[row].style_code);

        gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Brand:"), 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), edit_brand, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Model:"), 0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), edit_model, 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Style Code:"), 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), edit_style, 1, 2, 1, 1);

        gtk_widget_show_all(dialog);

        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            strcpy(sneakers[row].brand, gtk_entry_get_text(GTK_ENTRY(edit_brand)));
            strcpy(sneakers[row].model, gtk_entry_get_text(GTK_ENTRY(edit_model)));
            strcpy(sneakers[row].style_code, gtk_entry_get_text(GTK_ENTRY(edit_style)));
            sneakers[row].room_number = assign_room(&sneakers[row]); // Reassign room if needed

            refresh_sneaker_list();
        }

        gtk_widget_destroy(dialog);
        gtk_tree_path_free(path);
    }
}


void on_export_list(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Save File",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Save", GTK_RESPONSE_ACCEPT,
                                         NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "CSV files");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        FILE *fp = fopen(filename, "w");
        if (fp != NULL) {
            fprintf(fp, "Brand,Model,Style Code,Room\n");
            for (int i = 0; i < sneaker_count; i++) {
                fprintf(fp, "\"%s\",\"%s\",\"%s\",%d\n",
                    sneakers[i].brand,
                    sneakers[i].model,
                    sneakers[i].style_code,
                    sneakers[i].room_number);
            }
            fclose(fp);
            g_print("Exported to %s\n", filename);
        } else {
            g_print("Failed to save file.\n");
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

#ifdef _WIN32
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}
#endif