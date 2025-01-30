#include <iostream>
#include <string>
#include <ncurses.h>
#include <unistd.h>
#include <bitset>

using namespace std;

void slight_delay()
{
    for (int i = 0; i < 2; i++)
    {
        sleep(1);
    }
}

void record_score(int score, string challenge_name)
{
    clear();

    FILE *file = fopen("scores.txt", "a");

    if (file == NULL)
    {
        printw("Error opening file!\n");
        return;
    }

    printw("Record your score!\n");
    printw("Enter your name: ");
    refresh();

    char name_buffer[256];
    getstr(name_buffer);

    string name = string(name_buffer);

    fprintf(file, "%s  |  %d  |  %s  |  %d\n", name.c_str(), score, challenge_name.c_str(), (int)time(NULL));

    fclose(file);
}

void show_scores()
{
    clear();

    FILE *file = fopen("scores.txt", "r");

    if (file == NULL)
    {
        printw("Error opening file!\n");
        return;
    }

    printw("Scores\n");
    printw("------------------------------------\n");

    char name[256];
    int score;
    char challenge_name[256];
    int timestamp;

    while (fscanf(file, "%s  |  %d  |  %s  |  %d\n", name, &score, challenge_name, &timestamp) != EOF)
    {
        printw("%s  |  %d  |  %s  |  %d\n", name, score, challenge_name, timestamp);
    }

    fclose(file);

    refresh();

    slight_delay();
}

string create_placeholder(int length)
{
    string placeholder;
    for (int i = 0; i < length; i++)
    {
        placeholder += ".";
    }
    return placeholder;
}

void game_over(int score, string challenge_name, int random_number, int bit_count)
{
    // Clear the screen
    clear();
    printw("Game over!\n\n");

    string correct_answer;

    if (challenge_name == "binary")
    {
        // Convert to binary and remove the leading zeros
        correct_answer = bitset<32>(random_number).to_string().substr(32 - bit_count);
    }
    else
    {
        // Convert to hexadecimal
        sprintf((char *)correct_answer.c_str(), "%X", random_number);
    }

    printw("The correct answer was %s.\n", correct_answer.c_str());

    // Print final score
    printw("Your final score is %d.\n", score);

    // Ask if score needs to be recorded
    printw("Do you want to record your score? (y/n): ");
    refresh();

    int ch = getch();

    // If the user wants to record the score
    if (ch == 'y')
    {
        record_score(score, challenge_name);
    }
}

void print_game_state(int score, int bit_count, int random_number, string challenge_name, string user_input)
{
    // Clear the screen
    clear();

    // Show current score
    printw("Challenge             | %s\n", challenge_name.c_str());
    printw("Score                 | %d\n", score);
    printw("Number of bits        | %d\n\n", bit_count);

    // Show the number in decimal
    printw("Number in decimal: %d\n", random_number);

    // Ask the user to write the number
    printw("Enter the number in %s:\n", challenge_name.c_str());

    // Print current line
    printw("%s", user_input.c_str());
}

void start_game()
{
    clear();

    // Initialise variables
    unsigned int bit_count = 1;
    unsigned int score = 0;

    // Print basic information
    printw("Welcome to the binary and hexadecimal challenge!\n");
    printw("  You will be given a number in decimal.\n");
    printw("  You need to convert the number to binary or hexadecimal.\n");
    printw("  The number of bits will increase as you progress.\n\n");
    printw("Instructions:\n");
    printw("  '1'   start the binary challenge.\n");
    printw("  '2'   start the hexadecimal challenge.\n");
    printw("  's'   view the scores.\n");
    printw("  'q'   exit the game.\n");
    printw("\n");

    // Ask binary or hexadecimal challenge
    printw("Enter input: ");

    // Get the input
    int ch = getch();

    // Check if the input is "exit"
    if (ch == 'q')
    {
        clear();
        printw("Exiting the game...\n");
        refresh();
        return;
    }

    // Check if the input is "scores"
    if (ch == 's')
    {
        show_scores();
        start_game();
        return;
    }

    // Check if the input is valid
    if (ch != '1' && ch != '2')
    {
        clear();
        printw("Invalid input! Please try again.\n");
        refresh();

        slight_delay();

        start_game();
        return;
    }

    int challenge_type = ch - '0';
    string challenge_name;

    if (challenge_type == 1)
    {
        challenge_name = "binary";
    }
    else
    {
        challenge_name = "hexadecimal";
    }

    // Clear the screen
    clear();

    while (true)
    {
        // Generate random number that can be represented by the bit_count number of bits
        unsigned int random_number = (rand() % (1 << bit_count));

        // Create placeholder based on challenge type
        unsigned int placeholder_length = (challenge_type == 1) ? bit_count : (bit_count + 3) / 4;
        string user_input = create_placeholder(placeholder_length);

        // Print the game state
        print_game_state(score, bit_count, random_number, challenge_name, user_input);

        // Position cursor at start of input
        move(getcury(stdscr), 0);
        refresh();

        // Initialise variables
        int ch;
        int pos = 0;

        while (pos < placeholder_length)
        {
            // Get the character
            ch = getch();

            // Handle q
            if (ch == 'q')
            {
                game_over(score, challenge_name, random_number, bit_count);
                break;
            }

            // Handle backspace
            if (ch == 127 || ch == KEY_BACKSPACE)
            {
                // Move cursor back one
                pos = max(0, pos - 1);

                // Set the character to a dot
                user_input[pos] = '.';
            }
            else
            {
                // Validate input character
                bool valid_char = (challenge_type == 1) ? (ch == '0' || ch == '1') : (isxdigit(ch));

                // If valid character is entered add it to the input
                if (valid_char)
                {
                    // Set the character to the input
                    user_input[pos] = ch;

                    // Increment the position
                    pos++;
                }
            }

            // Print current game state
            print_game_state(score, bit_count, random_number, challenge_name, user_input);

            // Move the cursor back to the start of the input
            move(getcury(stdscr), pos);
        }

        // Double check to exit double loop
        if (ch == 'q')
        {
            break;
        }

        int answer;

        // Convert the user input to an integer
        if (challenge_type == 1)
        {

            answer = stoi(user_input, 0, 2);
        }
        else
        {

            answer = stoi(user_input, 0, 16);
        }

        // Check if the answer is correct
        if (answer != random_number)
        {
            game_over(score, challenge_name, random_number, bit_count);
            break;
        }

        // Increase the score
        score++;

        // Increase the bit count
        bit_count++;

        // Clear the screen
        clear();
    }

    // Restart the game
    start_game();
}

int main()
{
    // Initialise ncurses
    initscr();
    echo();
    cbreak();

    // Seed the random number generator
    srand(time(NULL));

    // Start the game
    start_game();

    // End ncurses
    endwin();

    return 0;
}