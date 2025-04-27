#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SNEAKERS 1000

typedef struct {
    char brand[50];
    char model[50];
    char style_code[20];
    int sku; // Nike, Adidas only
    int color_num; // Nike, Adidas Only
    int room_number; // 1, 2, 3
} Sneaker;

// Functions
void add_sneaker(Sneaker *list, int *count);
void parse_sneaker_style(Sneaker *s);
void print_list(const Sneaker *list, int count);

int main() {
    Sneaker sneakers[MAX_SNEAKERS];
    int count = 0;
    char choice;

    printf("zapato\n");
    printf("======\n");

    do
    {
        add_sneaker(sneakers, &count);

        printf("Add another sneaker? (y/n): ");
        scanf(" %c", &choice);
    } while (choice == 'y' || choice == 'Y');

    printf("\nSneaker List:\n");
    printf("=============\n");
    print_list(sneakers, count);

    char export_choice;
    printf("\nExport list to a file? (y/n): ");
    scanf(" %c", &export_choice);

    if (export_choice == 'y' || export_choice == 'Y') {
        char filename[100];
        printf("Enter filename (example: sneakers.txt): ");
        scanf(" %[^\n]", filename);

        export_list(sneakers, count, filename);
    }

    return 0;
    
}

void add_sneaker(Sneaker *list, int *count) {
    Sneaker *s = &list[*count];

    printf("Enter Brand: ");
    scanf(" %[^\n]", s->brand);

    printf("Enter Model: ");
    scanf(" %[^\n]", s->model);

    printf("Enter Style/Color Code: ");
    scanf(" %[^\n]", s->style_code);

    // Check if Nike
    if (strcasecmp(s->brand, "Nike") == 0) {
        parse_sneaker_style(s);
    } else {
        s->sku = -1;
        s->color_num = -1;
    }

    // For now: manually assign room (later we can automate this)
    printf("Enter Room Number (1=Running/Retro Tech, 2=Men's Basketball/Women's, 3=Kids): ");
    scanf("%d", &s->room_number);

    (*count)++;
}

void parse_sneaker_style(Sneaker *s) {
    char *token;
    char style_copy[50];
    strcpy(style_copy, s->style_code);

    // Parse SKU and Color Number
    token = strtok(style_copy, "-");
    if (token != NULL) {
        s->sku = atoi(token);
        token = strtok(NULL, "-");
        if (token != NULL) {
            s->color_num = atoi(token);
        }
    }
}

void print_list(const Sneaker *list, int count) {
    printf("Current Sneakers:\n");
    printf("-----------------\n");
    for (int i = 0; i < count; i++) {
        printf("%d. %s | %s | %s | Room %d\n", 
            i+1, 
            list[i].brand, 
            list[i].model, 
            list[i].style_code, 
            list[i].room_number
        );
    }
}

int compare_sneakers(const void *a, const void *b) {
    const Sneaker *s1 = (const Sneaker *)a;
    const Sneaker *s2 = (const Sneaker *)b;

    // Step 1: Compare brand names (alphabetically)
    int brand_cmp = strcasecmp(s1->brand, s2->brand);
    if (brand_cmp != 0)
        return brand_cmp;

    // Step 2: If Nike, compare by SKU and Color Code numerically
    if (strcasecmp(s1->brand, "Nike") == 0) {
        if (s1->sku != s2->sku)
            return s1->sku - s2->sku;
        else
            return s1->color_num - s2->color_num;
    }
    else if (strcasecmp(s1->brand, "Adidas") == 0) {
        if (s1->sku != s2->sku)
            return s1->sku - s2->sku;
        else
            return s1->color_num - s2->color_num;
    }
    else if (strcasecmp(s1->brand, "Puma") == 0) {
        if (s1->sku != s2->sku)
            return s1->sku - s2->sku;
        else
            return s1->color_num - s2->color_num;
    }
    else if (strcasecmp(s1->brand, "Reebok") == 0)
    {
        if (s1->sku != s2->sku)
            return s1->sku - s2->sku;
        else
            return s1->color_num - s2->color_num;
    }
    

    // Step 3: Otherwise, compare by Model Name
    int model_cmp = strcasecmp(s1->model, s2->model);
    if (model_cmp != 0)
        return model_cmp;

    // Step 4: Then by full style_code (lexicographically)
    return strcmp(s1->style_code, s2->style_code);
}

void export_list(const Sneaker *list, int count, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    fprintf(fp, "Sorted Sneaker Inventory\n");
    fprintf(fp, "=========================\n\n");

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d. %s | %s | %s | Room %d\n",
            i+1,
            list[i].brand,
            list[i].model,
            list[i].style_code,
            list[i].room_number
        );
    }

    fclose(fp);
    printf("List successfully exported to %s\n", filename);
}
