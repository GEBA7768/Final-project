#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MIN_NUM 1
#define MAX_NUM 100

static GtkBuilder* builder;
static GtkWidget* window, * guess_entry, * result_label, * computer_label, * range_label, * guess_button;

gint Goal_Number, player_attempts, computer_attempts;
gint player_low, player_high;  // Player's range
gint computer_low, computer_high;  // Computer's range
gboolean player_won = FALSE;
GString* computer_guesses = NULL;  // To store all computer guesses

gboolean validate_guess(const gchar* guess_text) {
    if (g_ascii_strtoll(guess_text, NULL, 10) == 0 && guess_text[0] != '0') {
        return FALSE;  // Invalid input (non-numeric or empty)
    }
    return TRUE;
}

void computer_guess_all() {
    computer_attempts = 0;
    computer_low = MIN_NUM;
    computer_high = MAX_NUM;

    g_string_assign(computer_guesses, "");  // Reset guesses log

    while (computer_low <= computer_high) {
        gint guess = (computer_low + computer_high) / 2;
        computer_attempts++;

        // Log the guess
        gchar buffer[32];
        g_snprintf(buffer, sizeof(buffer), "%d ", guess);
        g_string_append(computer_guesses, buffer);

        if (guess < Goal_Number) {
            computer_low = guess + 1;
        }
        else if (guess > Goal_Number) {
            computer_high = guess - 1;
        }
        else {
            break;  // Computer found the number
        }
    }

    // Update computer's guesses and results
    gchar result_text[256];
    g_snprintf(result_text, sizeof(result_text),
        "Computer guessed %d in %d attempts.\nPlayer: %d attempts.",
        Goal_Number, computer_attempts, player_attempts);
    gtk_label_set_text(GTK_LABEL(result_label), result_text);

    gtk_label_set_text(GTK_LABEL(computer_label), computer_guesses->str);
}

void on_guess_button_clicked() {
    if (player_won) {
        // If player has already won, let the computer guess
        computer_guess_all();
        return;
    }

    const gchar* guess_text = gtk_entry_get_text(GTK_ENTRY(guess_entry));
    if (!validate_guess(guess_text)) {
        gtk_label_set_text(GTK_LABEL(result_label), "Please enter a valid number.");
        return;
    }

    gint player_guess = atoi(guess_text);
    player_attempts++;

    gchar result_text[128];
    if (player_guess < Goal_Number) {
        gtk_label_set_text(GTK_LABEL(result_label), "Too low! Try again.");
        player_low = (player_guess > player_low) ? player_guess + 1 : player_low;
    }
    else if (player_guess > Goal_Number) {
        gtk_label_set_text(GTK_LABEL(result_label), "Too high! Try again.");
        player_high = (player_guess < player_high) ? player_guess - 1 : player_high;
    }
    else {
        g_snprintf(result_text, sizeof(result_text), "Congratulations! You found the number %d in %d attempts.",
            Goal_Number, player_attempts);
        gtk_label_set_text(GTK_LABEL(result_label), result_text);
        player_won = TRUE;  // Player guessed correctly, start computer guessing on next button click
    }

    // Update player's range
    gchar range_text[128];
    g_snprintf(range_text, sizeof(range_text), "Your range: [%d, %d]", player_low, player_high);
    gtk_label_set_text(GTK_LABEL(range_label), range_text);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    // Initialize UI from Glade file
    builder = gtk_builder_new_from_file("game.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    guess_entry = GTK_WIDGET(gtk_builder_get_object(builder, "guess_entry"));
    result_label = GTK_WIDGET(gtk_builder_get_object(builder, "result_label"));
    computer_label = GTK_WIDGET(gtk_builder_get_object(builder, "computer_label"));
    range_label = GTK_WIDGET(gtk_builder_get_object(builder, "range_label"));
    guess_button = GTK_WIDGET(gtk_builder_get_object(builder, "guess_button"));

    // Connect signals
    g_signal_connect(G_OBJECT(guess_button), "clicked", G_CALLBACK(on_guess_button_clicked), NULL);

    // Initialize game variables
    srand(time(NULL));  // Seed random number generator
    Goal_Number = rand() % (MAX_NUM - MIN_NUM + 1) + MIN_NUM;
    player_attempts = 0;
    player_low = MIN_NUM;
    player_high = MAX_NUM;
    computer_low = MIN_NUM;
    computer_high = MAX_NUM;
    computer_guesses = g_string_new("");  // Initialize computer guesses log

    // Show the window and start GTK main loop
    gtk_widget_show(window);
    gtk_main();

    g_string_free(computer_guesses, TRUE);  // Free memory when program exits
    return 0;
}




/*
#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#define MIN_NUM 1
#define MAX_NUM 100

static GtkBuilder* builder;
static GtkWidget* window, * guess_entry, * result_label, * computer_label, * range_label, * guess_button;

gint Goal_Number, player_attempts, computer_attempts;
gint player_low, player_high;  // Player's range
gint computer_low, computer_high;  // Computer's range
gboolean player_won = FALSE;

gboolean validate_guess(const gchar* guess_text) {
    if (g_ascii_strtoll(guess_text, NULL, 10) == 0 && guess_text[0] != '0') {
        return FALSE;  // Invalid input (non-numeric or empty)
    }
    return TRUE;
}

void computer_guess() {
    gint guess = (computer_low + computer_high) / 2;
    computer_attempts++;

    gchar computer_text[128];
    g_snprintf(computer_text, sizeof(computer_text), "Computer's guess: %d", guess);
    gtk_label_set_text(GTK_LABEL(computer_label), computer_text);

    if (guess < Goal_Number) {
        computer_low = guess + 1;
    }
    else if (guess > Goal_Number) {
        computer_high = guess - 1;
    }
    else {
        gchar result_text[256];
        g_snprintf(result_text, sizeof(result_text),
            "Computer found the number %d in %d attempts.\n"
            "Player: %d attempts.",
            Goal_Number, computer_attempts, player_attempts);
        gtk_label_set_text(GTK_LABEL(result_label), result_text);
    }
}

void on_guess_button_clicked() {
    if (player_won) {
        // Player has already guessed correctly, start computer guessing
        computer_guess();
        return;
    }

    const gchar* guess_text = gtk_entry_get_text(GTK_ENTRY(guess_entry));
    if (!validate_guess(guess_text)) {
        gtk_label_set_text(GTK_LABEL(result_label), "Please enter a valid number.");
        return;
    }

    gint player_guess = atoi(guess_text);
    player_attempts++;

    gchar result_text[128];
    if (player_guess < Goal_Number) {
        gtk_label_set_text(GTK_LABEL(result_label), "Too low! Try again.");
        player_low = (player_guess > player_low) ? player_guess + 1 : player_low;
    }
    else if (player_guess > Goal_Number) {
        gtk_label_set_text(GTK_LABEL(result_label), "Too high! Try again.");
        player_high = (player_guess < player_high) ? player_guess - 1 : player_high;
    }
    else {
        g_snprintf(result_text, sizeof(result_text), "Congratulations! You found the number %d in %d attempts.",
            Goal_Number, player_attempts);
        gtk_label_set_text(GTK_LABEL(result_label), result_text);
        player_won = TRUE;  // Player guessed correctly, start computer guessing on next button click
    }

    // Update player's range
    gchar range_text[128];
    g_snprintf(range_text, sizeof(range_text), "Your range: [%d, %d]", player_low, player_high);
    gtk_label_set_text(GTK_LABEL(range_label), range_text);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    // Initialize UI from Glade file
    builder = gtk_builder_new_from_file("game.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    guess_entry = GTK_WIDGET(gtk_builder_get_object(builder, "guess_entry"));
    result_label = GTK_WIDGET(gtk_builder_get_object(builder, "result_label"));
    computer_label = GTK_WIDGET(gtk_builder_get_object(builder, "computer_label"));
    range_label = GTK_WIDGET(gtk_builder_get_object(builder, "range_label"));
    guess_button = GTK_WIDGET(gtk_builder_get_object(builder, "guess_button"));

    // Connect signals
    g_signal_connect(G_OBJECT(guess_button), "clicked", G_CALLBACK(on_guess_button_clicked), NULL);

    // Initialize game variables
    srand(time(NULL));  // Seed random number generator
    Goal_Number = rand() % (MAX_NUM - MIN_NUM + 1) + MIN_NUM;
    player_attempts = computer_attempts = 0;
    player_low = computer_low = MIN_NUM;
    player_high = computer_high = MAX_NUM;

    // Show the window and start GTK main loop
    gtk_widget_show(window);
    gtk_main();

    return 0;
}*/











/*
#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

#define MIN_NUM 1
#define MAX_NUM 100

// 全域變數
static GtkBuilder* builder;
static GtkWidget* window, * guess_entry, * result_label, * range_label, * guess_button;
static gint goal_number, user_attempts, current_min, current_max;

// 驗證輸入的數字是否有效
gboolean validate_guess(const gchar* guess_text) {
    gchar* end_ptr = NULL;
    gint64 guess = g_ascii_strtoll(guess_text, &end_ptr, 10);

    // 檢查數字是否有效並在範圍內
    if (*end_ptr != '\0' || guess < current_min || guess > current_max) {
        return FALSE;
    }
    return TRUE;
}

// 按下按鈕時的邏輯
void on_guess_button_clicked() {
    const gchar* guess_text = gtk_entry_get_text(GTK_ENTRY(guess_entry));

    // 驗證輸入是否有效
    if (!validate_guess(guess_text)) {
        gtk_label_set_text(GTK_LABEL(result_label), "Invalid input! Enter a valid number.");
        return;
    }

    gint guess = atoi(guess_text); // 將輸入轉為整數
    user_attempts++;

    // 更新範圍與結果
    if (guess < goal_number) {
        current_min = guess + 1;
        gtk_label_set_text(GTK_LABEL(result_label), "Too low! Try again.");
    }
    else if (guess > goal_number) {
        current_max = guess - 1;
        gtk_label_set_text(GTK_LABEL(result_label), "Too high! Try again.");
    }
    else {
        gchar result_text[128];
        snprintf(result_text, sizeof(result_text), "Congratulations! You found the number %d in %d attempts.", goal_number, user_attempts);
        gtk_label_set_text(GTK_LABEL(result_label), result_text);
        gtk_label_set_text(GTK_LABEL(range_label), "Game over! Restart to play again.");
        gtk_widget_set_sensitive(guess_button, FALSE); // 禁用按鈕
        return;
    }

    // 更新範圍顯示
    gchar range_text[128];
    snprintf(range_text, sizeof(range_text), "Range: [%d, %d]", current_min, current_max);
    gtk_label_set_text(GTK_LABEL(range_label), range_text);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    // 載入 Glade 介面
    builder = gtk_builder_new_from_file("game.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    guess_entry = GTK_WIDGET(gtk_builder_get_object(builder, "guess_entry"));
    result_label = GTK_WIDGET(gtk_builder_get_object(builder, "result_label"));
    range_label = GTK_WIDGET(gtk_builder_get_object(builder, "range_label"));
    guess_button = GTK_WIDGET(gtk_builder_get_object(builder, "guess_button"));

    // 設定按鈕事件
    g_signal_connect(G_OBJECT(guess_button), "clicked", G_CALLBACK(on_guess_button_clicked), NULL);

    // 初始化遊戲數據
    srand(time(NULL));
    goal_number = rand() % (MAX_NUM - MIN_NUM + 1) + MIN_NUM; // 隨機目標數字
    current_min = MIN_NUM;
    current_max = MAX_NUM;
    user_attempts = 0;

    // 初始範圍顯示
    gchar range_text[128];
    snprintf(range_text, sizeof(range_text), "Range: [%d, %d]", current_min, current_max);
    gtk_label_set_text(GTK_LABEL(range_label), range_text);

    // 顯示主視窗
    gtk_widget_show(window);
    gtk_main();

    return 0;
}*/












/*
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_NUM 1
#define MAX_NUM 100

static GtkBuilder* builder;
static GtkWidget* window, * guess_entry, * result_label, * computer_label, * range_label, * guess_button;

gint Goal_Number, user_attempts;

gboolean validate_guess(const gchar* guess_text) {
    gint64 guess = 0;
    if (g_ascii_strtoll(guess_text, NULL, 10) == 0 || guess_text[0] == '\0') {
        return FALSE;
    }
    return TRUE;
}

void on_guess_button_clicked() {
    const gchar* guess_text = gtk_entry_get_text(GTK_ENTRY(guess_entry));
    if (!validate_guess(guess_text)) {
        gtk_label_set_text(GTK_LABEL(result_label), "Please enter a valid number.");
        return;
    }

    gint guess = atoi(guess_text);
    user_attempts++;

    if (guess < Goal_Number) {
        gtk_label_set_text(GTK_LABEL(result_label), "Too low! Try again.");
    }
    else if (guess > Goal_Number) {
        gtk_label_set_text(GTK_LABEL(result_label), "Too high! Try again.");
    }
    else {
        gchar* result_text = g_strdup(gtk_label_get_text(GTK_LABEL(result_label)));
        g_snprintf(result_text, 128, "Congratulations! You found the number %d in %d attempts.", Goal_Number, user_attempts);
        gtk_label_set_text(GTK_LABEL(result_label), result_text);
        g_free(result_text);
        return; // Exit the function once the number is guessed
    }
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("game.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    guess_entry = GTK_WIDGET(gtk_builder_get_object(builder, "guess_entry"));
    result_label = GTK_WIDGET(gtk_builder_get_object(builder, "result_label"));
    computer_label = GTK_WIDGET(gtk_builder_get_object(builder, "computer_label"));
    range_label = GTK_WIDGET(gtk_builder_get_object(builder, "range_label"));
    guess_button = GTK_WIDGET(gtk_builder_get_object(builder, "guess_button"));

    g_signal_connect(G_OBJECT(guess_button), "clicked", G_CALLBACK(on_guess_button_clicked), NULL);

    srand(time(NULL));  // Initialize random seed
    Goal_Number = rand() % (MAX_NUM - MIN_NUM + 1) + MIN_NUM;  // Random number between MIN_NUM and MAX_NUM

    gtk_widget_show(window);
    gtk_main();

    return 0;
}*/

/*
#include <gtk/gtk.h>

int Goal_Number = 50; // You should set this to the correct value
int user_attempts = 0;

GtkWidget* result_label;
GtkWidget* guess_entry;

void on_guess_button_clicked(GtkWidget* widget, gpointer data) {
    const char* text = gtk_entry_get_text(GTK_ENTRY(guess_entry));
    int guess = atoi(text);
    user_attempts++;

    gchar buffer[128];

    if (guess < Goal_Number) {
        g_snprintf(buffer, 128, "Too low! Try again.");
        gtk_label_set_text(GTK_LABEL(result_label), buffer);
    }
    else if (guess > Goal_Number) {
        g_snprintf(buffer, 128, "Too high! Try again.");
        gtk_label_set_text(GTK_LABEL(result_label), buffer);
    }
    else {
        g_snprintf(buffer, 128, "Congratulations! You found the number %d in %d attempts.", Goal_Number, user_attempts);
        gtk_label_set_text(GTK_LABEL(result_label), buffer);
    }
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Guess the Number Game");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    result_label = gtk_label_new("Make your guess:");
    gtk_box_pack_start(GTK_BOX(vbox), result_label, TRUE, TRUE, 0);

    guess_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), guess_entry, TRUE, TRUE, 0);

    GtkWidget* guess_button = gtk_button_new_with_label("Guess");
    gtk_box_pack_start(GTK_BOX(vbox), guess_button, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(guess_button), "clicked", G_CALLBACK(on_guess_button_clicked), NULL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
*/
/*#include <gtk/gtk.h>
#include <glib.h> // Ensure glib.h is included

int Goal_Number;
int user_attempts = 0;
GtkWidget* guess_button, * result_label, * range_label, * computer_label, * guess_entry;

// Computer guessing logic
gboolean computer_guess(gpointer data) {
    static int low = 1, high = 100, guess, attempts = 0;
    int mid;

    mid = (low + high) / 2;
    guess = mid;
    attempts++;

    if (guess < Goal_Number) {
        low = guess + 1;
    }
    else if (guess > Goal_Number) {
        high = guess - 1;
    }
    else {
        gchar* text = g_strdup(gtk_label_get_text(GTK_LABEL(computer_label)));
        g_snprintf(text, 128, "Computer's guess: %d, Attempts: %d", guess, attempts);
        gtk_label_set_text(GTK_LABEL(computer_label), text);
        g_free(text);
        return FALSE;
    }

    gchar* range_text = g_strdup(gtk_label_get_text(GTK_LABEL(range_label)));
    g_snprintf(range_text, 128, "Range: [%d, %d]", low, high);
    gtk_label_set_text(GTK_LABEL(range_label), range_text);
    g_free(range_text);

    g_idle_add(computer_guess, NULL);
    return FALSE;
}

static void on_guess_button_clicked(GtkWidget* widget, gpointer data) {
    const gchar* text = gtk_entry_get_text(GTK_ENTRY(guess_entry));
    int user_guess = atoi(text);
    user_attempts++;

    gchar* result_text = g_strdup(gtk_label_get_text(GTK_LABEL(result_label)));
    if (user_guess < Goal_Number) {
        g_snprintf(result_text, 128, "Too low! Try again.");
    }
    else if (user_guess > Goal_Number) {
        g_snprintf(result_text, 128, "Too high! Try again.");
    }
    else {
        g_snprintf(result_text, 128, "Congratulations! You found the number %d in %d attempts.", Goal_Number, user_attempts);
        g_idle_add(computer_guess, NULL); // Start the computer guessing process
    }
    gtk_label_set_text(GTK_LABEL(result_label), result_text);
    g_free(result_text);
}

int main(int argc, char* argv[]) {
    GtkBuilder* builder;
    GtkWidget* window;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("game.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));

    guess_button = GTK_WIDGET(gtk_builder_get_object(builder, "guess_button"));
    result_label = GTK_WIDGET(gtk_builder_get_object(builder, "result_label"));
    range_label = GTK_WIDGET(gtk_builder_get_object(builder, "range_label"));
    computer_label = GTK_WIDGET(gtk_builder_get_object(builder, "computer_label"));
    guess_entry = GTK_WIDGET(gtk_builder_get_object(builder, "guess_entry"));  // This should come last

    g_signal_connect(guess_button, "clicked", G_CALLBACK(on_guess_button_clicked), NULL);

    gtk_widget_show(window);
    gtk_main();

    return 0;
}
*/
/*
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_NUM 1
#define MAX_NUM 100

static GtkBuilder* builder;
static GtkWidget* window, * guess_entry, * result_label, * computer_label, * range_label, * guess_button;

gint Goal_Number, user_attempts;

gboolean validate_guess(const gchar* guess_text) {
    gint64 guess = 0;
    if (g_ascii_strtoll(guess_text, NULL, 10) == 0 || guess_text[0] == '\0') {
        return FALSE;
    }
    return TRUE;
}

void computer_guess() {
    static gboolean first_guess = TRUE;
    static gint low = MIN_NUM, high = MAX_NUM, guess, attempts = 0;
    if (first_guess) {
        guess = (low + high) / 2;
        first_guess = FALSE;
    }
    else {
        guess = (low + high) / 2;
    }
    attempts++;

    gchar* text = g_strdup(gtk_label_get_text(GTK_LABEL(computer_label)));  // g_strdup ensures safe copying
    g_snprintf(text, 128, "Computer's guess: %d, Attempts: %d", guess, attempts);
    gtk_label_set_text(GTK_LABEL(computer_label), text);
    g_free(text);

    if (guess < Goal_Number) {
        low = guess + 1;
    }
    else if (guess > Goal_Number) {
        high = guess - 1;
    }

    gchar* range_text = g_strdup(gtk_label_get_text(GTK_LABEL(range_label)));
    g_snprintf(range_text, 128, "Range: [%d, %d]", low, high);
    gtk_label_set_text(GTK_LABEL(range_label), range_text);
    g_free(range_text);

    if (guess == Goal_Number) {
        gchar* result_text = g_strdup(gtk_label_get_text(GTK_LABEL(result_label)));
        g_snprintf(result_text, 128, "Congratulations! Computer found the number %d in %d attempts.", Goal_Number, attempts);
        gtk_label_set_text(GTK_LABEL(result_label), result_text);
        g_free(result_text);
    }
}

void on_guess_button_clicked() {
    const gchar* guess_text = gtk_entry_get_text(GTK_ENTRY(guess_entry));
    if (!validate_guess(guess_text)) {
        gtk_label_set_text(GTK_LABEL(result_label), "Please enter a valid number.");
        return;
    }

    gint guess = atoi(guess_text);
    user_attempts++;

    if (guess < Goal_Number) {
        gtk_label_set_text(GTK_LABEL(result_label), "Too low! Try again.");
    }
    else if (guess > Goal_Number) {
        gtk_label_set_text(GTK_LABEL(result_label), "Too high! Try again.");
    }
    else {
        gchar* result_text = g_strdup(gtk_label_get_text(GTK_LABEL(result_label)));
        g_snprintf(result_text, 128, "Congratulations! You found the number %d in %d attempts.", Goal_Number, user_attempts);
        gtk_label_set_text(GTK_LABEL(result_label), result_text);
        g_free(result_text);
    }

    if (guess != Goal_Number) {
        computer_guess();
    }
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("game.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    guess_entry = GTK_WIDGET(gtk_builder_get_object(builder, "guess_entry"));
    result_label = GTK_WIDGET(gtk_builder_get_object(builder, "result_label"));
    computer_label = GTK_WIDGET(gtk_builder_get_object(builder, "computer_label"));
    range_label = GTK_WIDGET(gtk_builder_get_object(builder, "range_label"));
    guess_button = GTK_WIDGET(gtk_builder_get_object(builder, "guess_button"));

    g_signal_connect(G_OBJECT(guess_button), "clicked", G_CALLBACK(on_guess_button_clicked), NULL);

    srand(time(NULL));  // Initialize random seed
    Goal_Number = rand() % (MAX_NUM - MIN_NUM + 1) + MIN_NUM;  // Random number between MIN_NUM and MAX_NUM

    gtk_widget_show(window);
    gtk_main();

    return 0;
}
*/