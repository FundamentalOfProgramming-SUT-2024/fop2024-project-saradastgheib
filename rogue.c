#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>   
#include <stdbool.h>  
//403105941
char current_username[50] = "";
char game_mode[20] = "Easy";
char character_color[20]= "";
#define FILENAME "users.txt"
#define HALLFILE "halloffame.txt"
#define MAXROW 35
#define MAXCOL 150
int max_health;
int battle_map[20][40];
int battle_room_monsters_count;
typedef enum {
    WALLH = 0, // _
    FLOOR = 1,
    DOOR = 2,
    CORRIDOR = 3,
    PILLAR = 4,
    WINDO = 5,
    EMPTY = 6,
    WALLV = 7, // |
    WALLNO = 8,
    TRAP = 9,
    STAIRCASE = 10,
    SWALLH = 11,
    SWALLV = 12,
    SWALLNO = 13,
    PASSWORDDOOR = 14,
    PASSWORD = 15, // &
    ANCIENTKEY = 16,
    REVEALEDTRAP = 17,
    HEALTH_POTION = 18,
    SPEED_POTION = 19,
    DAMAGE_POTION = 20,
    END = 21,
    GOLD = 22,
    nFOOD = 23,
    BLACKGOLD = 24,
    fFOOD = 25,
    DAGGER = 26,
    MAGIC_WAND = 27,
    NORMAL_ARROW = 28,
    SWORD = 29,
    mFOOD = 30, 
    rFOOD = 31,
    dagger_SHOT = 32,
    magic_wand_SHOT = 33,
    normal_arrow_SHOT = 34
} map_elements;
WINDOW *winb;
map_elements game_map[MAXROW][MAXCOL];
map_elements previous[MAXROW][MAXCOL];
int visible[MAXROW][MAXCOL];
int unlocked[MAXROW][MAXCOL];
int type[MAXROW][MAXCOL]; // 3 for treasure 2 for enchant 1 for nightmare
typedef struct {
    char map[MAXROW][MAXCOL];
    int player_row, player_col;
} Floor;
int current_floor = 0;
char pass[5];
typedef struct {
    int row;
    int col;
    int health;
    int ancientkeys;
    int brokenkeys;
    int hps;
    int dps;
    int sps;
    int golds;
    int normal_food;
    int fancy_food;
    int magical_food;
    int rotten_food;
    int mace; // 1
    int dagger; // 2
    int magic_wand; // 3
    int normal_arrow; // 4
    int sword; // 5
    int score;
    int weapon_in_hand;
    int power;
    int speed;
    int healthp;
    int battle_row;
    int battle_col;
} Player;
Player player;
typedef struct {
    int center_x; 
    int center_y; 
    int width;    
    int length;  
    int visited; 
} Room;
typedef struct {
    char username[50];
    char password[50];
    char email[100];
} 
User;
typedef struct{
    char name[25];
    int hits;
    int row;
    int col;
    int lives;
    int moving;
    int moves;
    int max_moves;
    int alive;
} Monsters;
Monsters monster[20];
Monsters battle_monster[20];
int monsters_count = 0;
typedef struct {
    char username[50];
    int score;
    int gold_pieces;
    int games_finished;
    time_t first_game_time;
} HallOfFameEntry;
HallOfFameEntry entry = {0};
Room rooms[10];
int room_index = 0;
int g = 1;
int moves ;
int sword_count = 0;
int last_shot = 0;
int current_message = -1;
void main_menu ();
void startplaying();
void registeruser();
void login();
int user_exists(const char *username);
int validate_email(const char *email);
void save_user(User user);
int validate_password(const char *password);
void pre_game_area();
int check_password(const char *username, const char *password);
void game_area();
void displayhalloffame();
void savehall(const HallOfFameEntry entry);
bool load_user_data(HallOfFameEntry *entry);
void settings();
void difficulty_level();
void choose_color();
void initialize_map();
void createRoom(int row, int col, int roomRows, int roomCols);
int itoverlaps(int row, int col, int rowscount, int colscount);
void placetrap(int row, int col, int roomRows, int roomCols);
void corridors(int numRooms, Room *rooms);
void generate_random_map(int staircase_row, int staircase_col, int staircase_roomRows, int staircase_roomCols);
void removeUnconnectedDoors();
void movePlayer(int newRow, int newCol);
void printMap();
void createrandompillar(int row, int col, int roomRows, int roomCols);
void battleroom();
void placestaircase(int row, int col, int roomRows, int roomCols);
void placepassword(int row, int col, int roomRows, int roomCols);
void generatedoorpass();
void removepassword(int row, int col, int roomRows, int roomCols);
void placeancientkey(int row, int col, int roomRows, int roomCols);
void placepotions(int row, int col, int roomRows, int roomCols);
void make_it_treasure(int row, int col, int roomRows, int roomCols);
void makeitnightmare(int row, int col, int roomRows, int roomCols);
void trapsfortreasure(int row, int col, int roomRows, int roomCols);
void endpoint(int row, int col, int roomRows, int roomCols);
void placenormalpotions(int row, int col, int roomRows, int roomCols);
void placegold(int row, int col, int roomRows, int roomCols);
void placefood(int row, int col, int roomRows, int roomCols);
void place_blackgold(int row, int col, int roomRows, int roomCols);
void place_weapon(int row, int col, int roomRows, int roomCols);
void place_monsters(int row, int col, int roomRows, int roomCols);
void move_monsters();
void throwweapon();
void lastshot(int last_shot);
void makeitenchant(int row, int col, int roomRows, int roomCols);
void print_message(int message);
char generaterandompass(int type);
int retrieve_password(const char *username, char *password);
void display_profile();
void monster_shot();
void place_battle_monsters();
void printbattleMap();
void battle_moveplayer(int newRow, int newCol);
void battle_movemonsters();
void battle_throwweapon();
void battle_monstersshoot();
void previous_game();
int main() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    srand(time(NULL));
    curs_set(0);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_BLACK, COLOR_WHITE);
    init_pair(8, COLOR_MAGENTA, COLOR_WHITE);
    main_menu();
    endwin();
    return 0;
}
void display_profile(){
    clear();
    attron(A_BOLD | COLOR_PAIR(6));
    mvprintw(10, 30, "===============================");
    mvprintw(11, 30, " Welcome to the Dungeon Quest! ");
    mvprintw(12, 30, "===============================");
    attroff(A_BOLD | COLOR_PAIR(6));
    attron(COLOR_PAIR(5));
    mvprintw(14, 30, "Welcome, brave adventurer! 🎮");
    mvprintw(16, 30, "You are about to embark on a grand quest %s.", current_username);
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(2));
    mvprintw(18, 30, "Your goal: Conquer the dungeon and collect loot!");
    mvprintw(20,30, "Use the keys to move:");
    mvprintw(22, 30, "h - Left, j - Up, k - Down, l - Right");
    mvprintw(24, 30, "u - Upper-left, y - Upper-right, b - Lower-left, n - Lower-right");
    attroff(COLOR_PAIR(2));
    mvprintw(26, 30, "Good luck, %s! May your quest be victorious! ✨", current_username);
    getch();
}
int retrieve_password(const char *username, char *password){
    FILE *file = fopen(FILENAME, "r");
    if(!file){
        return 0;
    }
    char file_username[50], file_password[50], file_email[100];
    while (fscanf(file, "%s %s %s", file_username, file_password, file_email)== 3){
        if(strcmp(username, file_username) == 0){
            strcpy(password, file_password);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
    
}
void main_menu() {
    int choice = 0;
    char *options[] = {
        "Start playing",
        "Profile",
        "Hall of fame",
        "Settings",
        "Exit",
    };
    int number_of_options = sizeof(options)/ sizeof(options[0]);
    while (1)
    {
        clear();
        mvprintw(1,2, "-MAIN MENU-");
        for(int i=0; i<number_of_options;i++){
            if(i == choice){
                attron(A_REVERSE);
                mvprintw(3+i, 4, "%s", options[i]);
                attroff(A_REVERSE);
            }
            else{
                mvprintw(3+i, 4, "%s", options[i]);
            }   
        }
        int key = getch();
        if(key == KEY_UP) choice = (choice -1 +number_of_options) % number_of_options;
        else if (key == KEY_DOWN) choice = (choice+1) % number_of_options;
        else if(key== '\n'){
            if(choice == 0) startplaying();
            else if(choice == 1){
                display_profile();
            } // profile
            else if(choice == 2){
                displayhalloffame();
            } 
            else if(choice ==3) {
                settings();
            }
            else if(choice == 4){ 
                endwin();
                exit(0);
        }
    }
    }
}
void startplaying(){
    int choice = 0;
    char *options[] = {
        "Register new user",
        "Login",
        "Play as guest",
        "Go back",
    };
    int number_of_options = sizeof(options) / sizeof(options[0]);
    while(1){
        clear();
        for(int i=0; i<number_of_options; i++){
            if(i==choice){
                attron(A_REVERSE);
                mvprintw(3+i, 4, "%s", options[i]);
                attroff(A_REVERSE);
            }
            else{
                mvprintw(3+i, 4, "%s", options[i]);
            }
        }
        int key =getch();
        if(key == KEY_UP){
            choice = (choice-1 + number_of_options) % number_of_options;
        }
        else if(key == KEY_DOWN){
            choice = (choice+1) % number_of_options;
        }
        else if(key =='\n') {
            if(choice==0){
                registeruser();
            }
            else if(choice == 1){
                login();
                
            }
            else if(choice==2) {
                strcpy(current_username, "Guest");
                strcpy(entry.username, current_username);
                //HallOfFameEntry entry = {0};
                pre_game_area();
            }
            else if(choice ==3){
                main_menu();
            }
        }
    }
}
void registeruser() {
    WINDOW *win = newwin(12, 60, 5, 10);
    keypad(win, TRUE);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "-REGISTER NEW USER-");
    wrefresh(win);

    char username[50] = {0}, password[50] = {0}, email[100] = {0};
    int ch, i = 0;
    while (1) {
    mvwprintw(win, 4, 2, "Username: ");
    mvwprintw(win, 4, 12, "                     ");
    wrefresh(win);
    i = 0;
    memset(username, 0, sizeof(username));
    while (1) {
        ch = wgetch(win);
        if (ch == '\n' || ch == 10 || ch == '\r' || ch == KEY_ENTER) {
            username[i] = '\0'; // Null-terminate username
            //mvwprintw(win, 12, 2, "DEBUG: Username entered = %s", username); // Debugging username
            wrefresh(win);
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (i > 0) {
                i--;
                mvwaddch(win, 4, 12 + i, ' ');
                wmove(win, 4, 12 + i);
            }
        } else if (i < 49 && isprint(ch)) {
            username[i++] = ch;
            mvwaddch(win, 4, 12 + i - 1, ch);
        }
        else if(ch == 27) {
            startplaying();
        }
        wrefresh(win);
    }

    if (user_exists(username)) {
        wattron(win, COLOR_PAIR(1));
        mvwprintw(win, 5, 2, "Username already exists!");
        wattroff(win, COLOR_PAIR(1));
        wrefresh(win);
        continue;
    } else {
        break;
    }

}
flushinp();
mvwprintw(win, 5, 2, "                                               ");
    while (1) {
        mvwprintw(win , 10, 2, "if you want a random password press the down arrow key.");
        mvwprintw(win, 6, 2, "Password: ");
        mvwprintw(win, 6,12, "                                 ");
        //mvwprintw(win, 8, 2, "                                       ");
        wrefresh(win);
        i = 0;
        memset(password, 0, sizeof(password));
        while (1) {
            ch = wgetch(win);
            if (ch == '\n') { 
                break;
            } else if (ch == KEY_BACKSPACE || ch == 127) { 
                if (i > 0) {
                    i--;
                    mvwaddch(win, 6, 12 + i, ' ');
                    wmove(win, 6, 12 + i);
                }
            } else if (i < 49 && isprint(ch)) { 
                password[i++] = ch;
                mvwaddch(win, 6, 12 + i - 1, '*');
            }
            else if (ch == KEY_DOWN)
            {
                mvwprintw(win , 7, 2, "                                                       ");
                char password[8];
                password[0] = generaterandompass(3);
                password[1] = generaterandompass(2);
                password[2] = generaterandompass(1);
                password[3] = generaterandompass(1);
                password[4] = generaterandompass(1);
                password[5] = generaterandompass(1);
                password[6] = generaterandompass(1);
                password[7] = '\0';
                mvwprintw(win , 10, 2, "Your password is %s", password);
            }
            
            else if(ch == 27) {
            startplaying();
        }
            wrefresh(win);
        }
        password[i] = '\0';

        if(validate_password(password)==1){
            mvwprintw(win, 7,2, "                                              ");
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 7,2, "Too short! must include at least 7 characters");
            wattroff(win, COLOR_PAIR(1));
        }
        else if(validate_password(password)==2){
            mvwprintw(win, 7,2, "                                              ");
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 7, 2, "Must include at least one digit!");
            wattroff(win, COLOR_PAIR(1));
        }
        else if(validate_password(password)==3){
            mvwprintw(win, 7,2, "                                              ");
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 7, 2, "Must include at least one uppercase character!");
            wattroff(win, COLOR_PAIR(1));
        }
        else if(validate_password(password)==4){
            mvwprintw(win, 7,2, "                                              ");
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 7, 2, "Must include at least one lowercase character!");
            wattroff(win, COLOR_PAIR(1));
        }
         else {
            break;
        }
        wrefresh(win);
    }
    flushinp();
    mvwprintw(win, 7, 2, "                                               ");
    while(1){
        mvwprintw(win, 8, 2, "Email: ");
        mvwprintw(win, 8, 9, "                                  ");
        wrefresh(win);
        i=0;
        memset(email, 0, sizeof(email));
        while(1){
            ch = wgetch(win);
            if(ch=='\n' ||ch ==10 || ch == '\r' || ch == KEY_ENTER){
                email[i] = '\0';
                wrefresh(win);
                break;
            }
            else if( ch == KEY_BACKSPACE || ch==127){
                if ((i>0))
                {
                    i--;
                    mvwaddch(win, 8, 9+i, ' ');
                    wmove(win, 8, 9+i);
                }
            }
            else if(i<49 && isprint(ch)) {
                email[i++] = ch;
                mvwaddch(win, 8, 9+i-1, ch);
            }
            else if(ch == 27) {
            startplaying();
        }
            wrefresh(win);
        }
        if (!validate_email(email)) {
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 9, 2, "Invalid Email!");
            wattroff(win, COLOR_PAIR(1));
            wrefresh(win);
            continue;
    }
    else{
        break;
    }
    }
    mvwprintw(win, 9, 2, "                           ");
    User user;
    strcpy(user.username, username);
    strcpy(user.password, password);
    strcpy(user.email, email);
    save_user(user);
    mvwprintw(win, 10, 2, "                                                       ");
    wattron(win, COLOR_PAIR(2));
    mvwprintw(win, 10, 2, "Registered successfully!");
    wattroff(win, COLOR_PAIR(2));
    wrefresh(win);
    getch();
    delwin(win);
}
int validate_email(const char *email){
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');
    if(!at || !dot || at>dot) return 0;
    if(at == email || *(dot+1)=='\0') return 0;
    return 1;
}
int user_exists(const char *username) {
    FILE *file = fopen(FILENAME, "r");
    if(!file) return 0;
    User user;
    while(fscanf(file, "%s %s %s", user.username, user.password, user.email)==3){
        if(strcmp(user.username, username)==0){
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}
void save_user(User user){
    FILE *file = fopen(FILENAME, "a");
    if(!file){
        return;
    }
    fprintf(file, "%s %s %s\n", user.username, user.password, user.email);
    fclose(file);
}
int validate_password(const char *password){
    int Digit=0, Upper =0, Lower =0;
    if(strlen(password)<7){
        return 1;
    }
    for (int i=0; password[i]!='\0'; i++) {
        if(isdigit(password[i])) Digit ++;
        else if(isupper(password[i])) Upper++;
        else if(islower(password[i])) Lower++;
    }
    if(Digit==0) return 2;
    if(Upper==0) return 3;
    if(Lower==0) return 4;
}
void login(){
    WINDOW *win = newwin(12, 50, 5, 10);
    keypad(win, TRUE);
    box(win, 0 ,0);
    mvwprintw(win, 1, 2, "-LOGIN-");
    wrefresh(win);
    char username[50] = {0}, password[50];
    int ch, i=0;
    while(1) {
        mvwprintw(win, 4, 2, "Username: ");
        mvwprintw(win, 4, 12, "                      ");
        wrefresh(win);
        i=0;
        while(1){
            ch = wgetch(win);
            if(ch == '\n' || ch == KEY_ENTER){
                username[i] = '\0';
                wrefresh(win);
                break;
            }
            else if(ch == KEY_BACKSPACE || ch==127){
                if(i>0){
                    i--;
                    mvwaddch(win, 4, 12+i, ' ');
                    wmove(win, 4, 12+i);
                }
            }
            else if(i<49 && isprint(ch)){
                username[i++] = ch;
                mvwaddch(win, 4, 12+i-1, ch);
            }
            else if(ch == 27) {
            startplaying();
        }
            wrefresh(win);
        }
        if(!user_exists(username)){
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 5, 2, "Username doesn't exist");
            wattroff(win, COLOR_PAIR(1));
            wrefresh(win);
            
            continue;
        }
        else {
            mvwprintw(win, 5, 2, "                        ");
            break;
        }
    }
    flushinp();
    mvwprintw(win, 5, 2, "                                               ");
    while(1){
        mvwprintw(win, 6, 2, "Password: ");
        mvwprintw(win, 6, 12, "                           ");
        wrefresh(win);
        i=0;
        memset(password, 0, sizeof(password));
        while(1){
            mvwprintw(win, 7, 2, "Press down arrow if you forgot your password.");
            ch = wgetch(win);
            if(ch == '\n' || ch == KEY_ENTER) {
                password[i]= '\0';
                break;
            }
            else if(ch == KEY_DOWN){
                char stored_password[50];
                if(retrieve_password(username, stored_password)){
                    mvwprintw(win, 8, 2, "                                                       ");
                    mvwprintw(win, 8, 2, "Your password: %s", stored_password);
                }
                else{
                    mvwprintw(win, 8, 2, "Password retrieval failed!");
                }
                wrefresh(win);
                continue;
            }
            else if(ch== KEY_BACKSPACE || ch ==127) {
                if(i>0) {
                    i--;
                    mvwaddch(win, 6, 12+i, ' ');
                    wmove(win, 6, 12+i);
                }
            }
            else if(i<49 && isprint(ch)) {
                password[i++] = ch;
                mvwaddch(win, 6,12+i-1, '*');
            }
            else if(ch == 27) {
            startplaying();
        }
            wrefresh(win);
        }
        password[i] = '\0';
        if(check_password(username, password)){
            strcpy(current_username, username);
            current_username[sizeof(current_username) - 1] = '\0';
            wattron(win, COLOR_PAIR(2));
            mvwprintw(win, 8, 2, "                                             ");
            mvwprintw(win, 8, 2, "Login successful");
            wattroff(win, COLOR_PAIR(2));
            wrefresh(win);
            //HallOfFameEntry entry = {0};
            strcpy(entry.username, current_username);
        
            getch();
            pre_game_area();
        }
        else {
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 8, 2, "Wrong password! try again");
            wattroff(win, COLOR_PAIR(1));
            wrefresh(win);
        }
    }
}
int check_password(const char *username, const char *password){
    FILE *file = fopen(FILENAME, "r");
    if(!file) return 0;
    User user;
    while(fscanf(file, "%s %s %s", user.username, user.password, user.email)==3){
        if(strcmp(user.username, username)==0){
            fclose(file);
            return strcmp(user.password, password) == 0;
        }
    }
    fclose(file);
    return 0;
}
void save_map(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for saving");
        return;
    }

    if (fwrite(game_map, sizeof(map_elements), MAXROW * MAXCOL, file) != MAXROW * MAXCOL) {
        perror("Failed to write game_map");
    }
    if (fwrite(&player, sizeof(Player), 1, file) != 1) {
        perror("Failed to write player data");
    }
    if (fwrite(monster, sizeof(Monsters), 20, file) != 20) {
        perror("Failed to write monster data");
    }
    if (fwrite(visible, sizeof(int), MAXROW * MAXCOL, file) != MAXROW * MAXCOL) {
        perror("Failed to write visibility array");
    }

    fclose(file);
}
void load_map(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file for loading");
        return;
    }
    if (fread(game_map, sizeof(map_elements), MAXROW * MAXCOL, file) != MAXROW * MAXCOL) {
        perror("Failed to read game_map");
    }
    if (fread(&player, sizeof(Player), 1, file) != 1) {
        perror("Failed to read player data");
    }
    if (fread(monster, sizeof(Monsters), 20, file) != 20) {
        perror("Failed to read monster data");
    }
    if (fread(visible, sizeof(int), MAXROW * MAXCOL, file) != MAXROW * MAXCOL) {
        perror("Failed to read visibility array");
    }
    
    

    fclose(file);
}

void pre_game_area(){
    int choice =0;
    char *options[] = {
        "New game",
        "Continue last game",
        "Go back"
    };
    int number_of_options = sizeof(options)/ sizeof(options[0]) ;
    while(1) {
        clear();
        mvprintw(1, 2, "-CHOOSE ONE TO START THE GAME-");
        for(int i=0; i<number_of_options; i++){
            if(i==choice){
                attron(A_REVERSE);
                mvprintw(3+i, 4, "%s", options[i]);
                attroff(A_REVERSE);
            }
            else {
                mvprintw(3+i, 4, "%s", options[i]);
            }
        }
        int key = getch();
        if(key == KEY_UP) choice = (choice -1 + number_of_options) % number_of_options;
        else if(key == KEY_DOWN) choice = (choice+1) % number_of_options;
        else if(key == '\n'){
            if(choice==0) game_area();
            else if(choice ==1) {
                load_map("saved.dat");
                previous_game();
            }
            else if(choice ==2){
                startplaying();
            }
        }
    }
}
void savehall(const HallOfFameEntry delta_entry) {
    HallOfFameEntry entries[100];
    int count = 0;
    int updated = 0;

    // Read existing entries from the file
    FILE *file = fopen(HALLFILE, "r");
    if (file) {
        while (fscanf(file, "%s %d %d %d %ld",
                      entries[count].username,
                      &entries[count].score,
                      &entries[count].gold_pieces,
                      &entries[count].games_finished,
                      &entries[count].first_game_time) == 5) {
            // Check if this entry matches the current user
            if (strcmp(entries[count].username, delta_entry.username) == 0) {
                // Add deltas to the existing entry
                entries[count].score += delta_entry.score;
                entries[count].gold_pieces += delta_entry.gold_pieces;
                entries[count].games_finished += delta_entry.games_finished;
                updated = 1;
            }
            count++;
        }
        fclose(file);
    }
    if (!updated) {
        if (count < 100) {
            entries[count] = delta_entry; // Includes first_game_time for new users
            count++;
        } else {
            fprintf(stderr, "Error: Hall of fame is full.\n");
            return;
        }
    }
    file = fopen(HALLFILE, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not save data.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %d %d %d %ld\n",
                entries[i].username,
                entries[i].score,
                entries[i].gold_pieces,
                entries[i].games_finished,
                entries[i].first_game_time);
    }
    fclose(file);
}
void displayhalloffame(){
    clear();
    mvprintw(1, 2, "-HALL OF FAME-");
    FILE *file = fopen(HALLFILE, "r");
    if(!file){
        mvprintw(3,2, "No one has played yet!Be the first");
        getch();
        return;
    }
    HallOfFameEntry entries[100];
    int entry_count=0;
    while(fscanf(file, "%s %d %d %d %ld", 
                  entries[entry_count].username, 
                  &entries[entry_count].score, 
                  &entries[entry_count].gold_pieces, 
                  &entries[entry_count].games_finished, 
                  &entries[entry_count].first_game_time) == 5) {
        entry_count++;
    }
    fclose(file);
    for(int i=0; i<entry_count-1; i++){
        for(int j=i+1; j<entry_count; j++){
            if(entries[i].score<entries[j].score){
            HallOfFameEntry temp = entries[i];
            entries[i] = entries[j];
            entries[j] = temp;
            }
        }
    }
    int current_page = 0;
    int entriesinpage = 10;
    int total_pages = (entry_count + entriesinpage-1) / entriesinpage;
    int key;
    do {
        clear();
        mvprintw(1, 2, "-HALL OF FAME- page %d/%d", current_page+1, total_pages);
        int start = current_page*entriesinpage;
        int end = start + entriesinpage;
        if(end> entry_count) end = entry_count;
        for(int i=start; i<end; i++){
            int row= 3 +i- start;
            if(strcmp(entries[i].username, current_username)==0){
                attron(A_ITALIC);
            }
            if(i==0){
                wattron(stdscr, COLOR_PAIR(3) | A_BOLD);
                mvprintw(row, 2, "\U0001F947 legend %s - %d - %d gold - %d games - %ld days",
                         entries[i].username,
                         entries[i].score,
                         entries[i].gold_pieces,
                         entries[i].games_finished,
                         ((time(NULL) - entries[i].first_game_time) / (60 * 60 * 24))%50);
                wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
            }
            else if(i==1){
                wattron(stdscr, COLOR_PAIR(3) | A_BOLD);
                mvprintw(row, 2, "\U0001F948 legend %s - %d - %d gold - %d games - %ld days",
                         entries[i].username,
                         entries[i].score,
                         entries[i].gold_pieces,
                         entries[i].games_finished,
                         ((time(NULL) - entries[i].first_game_time) / (60 * 60 * 24))%50);
                wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
            }
            else if(i==2){
                wattron(stdscr, COLOR_PAIR(3) | A_BOLD);
                mvprintw(row, 2, "\U0001F949 legend %s - %d - %d gold - %d games - %ld days",
                         entries[i].username,
                         entries[i].score,
                         entries[i].gold_pieces,
                         entries[i].games_finished,
                         ((time(NULL) - entries[i].first_game_time) / (60 * 60 * 24))%50);
                wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
            }
            else {
                mvprintw(row, 2, "%d. %s - %d - %d gold - %d games - %ld days",
                     i,
                     entries[i].username,
                     entries[i].score,
                     entries[i].gold_pieces,
                     entries[i].games_finished,
                     ((time(NULL) - entries[i].first_game_time) / (60 * 60 * 24))%50);
        }
                     if(strcmp(entries[i].username, current_username)==0){
                        attroff(A_ITALIC);
                     }
        }
        mvprintw(LINES -2, 2, "Use UP/DOWN to switch pages, ESC to exit.");
        key = getch();
        if(key == KEY_DOWN && current_page<total_pages-1){
            current_page++;
        }
        else if(key == KEY_UP && current_page>0) current_page --;
    }
    while(key != 27);
}
void printall() {
    clear(); 

    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            
                switch (game_map[i][j]) {
                case WALLV: mvaddch(i, j, '|'); break;
                case FLOOR: mvaddch(i, j, '.'); break;
                case DOOR: mvaddch(i, j, '+'); break;
                case CORRIDOR: mvaddch(i, j, '#'); break;
                case PILLAR: mvaddch(i, j, 'O'); break;
                case WINDO: mvaddch(i, j, '='); break;
                case EMPTY: mvaddch(i, j, ' '); break;
                case WALLH: mvaddch(i, j, '_'); break;
                case WALLNO: mvaddch(i, j, ' '); break;
                case TRAP: mvaddch(i, j, 'T'); break;
                case STAIRCASE: mvaddch(i, j, '<'); break;
                case SWALLH: mvaddch(i, j, 'D'); break;
                case SWALLV: mvaddch(i, j, 'D'); break;
                case SWALLNO: mvaddch(i, j, 'D'); break;
                case GOLD:
                attron(COLOR_PAIR(5));
                mvaddch(i, j, '*');
                attroff(COLOR_PAIR(5));
                break;
                case ANCIENTKEY:  
                    mvaddch(i, j, '^');  
                     break;
                case PASSWORD:
                    attron(COLOR_PAIR(5));
                    mvaddch(i, j, '&');
                    attroff(COLOR_PAIR(5));
                    break;
                case PASSWORDDOOR:
                    if(unlocked[i][j]) {
                        attron(COLOR_PAIR(2));
                        mvaddch(i, j, '@');
                        attroff(COLOR_PAIR(2));
                    }
                    else {
                        attron(COLOR_PAIR(1));
                        mvaddch(i, j, '@');
                        attroff(COLOR_PAIR(1));
                    }
                    break;
                default: mvaddch(i, j, ' '); break;
            }
            
        }
    }
    
    refresh();
    getch();
    clear();
}
void game_area(){
    player.score = 0;
    int gold = 0;
    moves = 0;
    last_shot = 0;
    clear();
    mvprintw(1, 2, "This is where messages should appear");
    refresh();
    if(strcmp(game_mode, "Legendary") == 0) max_health = 10;
    else if(strcmp(game_mode, "Insane") == 0) max_health = 20;
    else if(strcmp(game_mode, "Hard") == 0) max_health = 30;
    else if(strcmp(game_mode, "Normal") == 0) max_health = 40;
    else if(strcmp(game_mode , "Easy") == 0) max_health = 50;
    player.health = max_health;
    player.ancientkeys = 0;
    player.brokenkeys = 0;
    player.mace = 1;
    player.dagger = 0;
    player.magic_wand = 0;
    player.normal_arrow = 0;
    player.normal_food = 0;
    player.sword = 0;
    player.golds =0;
    player.score = 0;
    player.weapon_in_hand = 1;
    player.speed = 0; 
    player.power = 0;
    player.healthp = 0;
    current_message  = 4;
    strcpy(entry.username, current_username);
    current_floor = 0;
    if(!load_user_data(&entry)){
         entry.first_game_time = time(NULL);
    }
    generate_random_map(-1, -1, 0, 0);
    removeUnconnectedDoors();
    while(player.health > 0){
        printMap();
        print_message(current_message);
        mvprintw(35, 10, "Health: %d/%d", player.health, max_health);
        mvprintw(35, 40, "Ancient keys: %d", player.ancientkeys);
        mvprintw(35, 70, "Broken keys: %d", player.brokenkeys);
        mvprintw(35, 100, "Total score: %d", player.score);
        mvprintw(35, 130, "Gold: %d", player.golds);
        
        refresh();

        int ch = getch();
        switch (ch)
        {
        case 'o':
            WINDOW *winc = newwin(20, 60, 5, 10);
            keypad(winc, TRUE);
            box(winc, 0, 0);
            for(int i =0; i< monsters_count; i++){
                mvwprintw(winc, 2 + i, 12, "%d)%s %d %d", i, monster[i].name, monster[i].row, monster[i].col);
            }
            wrefresh(winc);
            getch();
            delwin(winc);
            break;
        case 'y':
            movePlayer(player.row -1, player.col -1);
            if(player.speed) movePlayer(player.row -1, player.col -1);
            move_monsters();
            break;
        case 'u':
            movePlayer(player.row-1, player.col+1);
            if(player.speed) movePlayer(player.row-1, player.col+1);
            move_monsters();
            break;
        case 'h':
            movePlayer(player.row, player.col-1);
            if(player.speed) movePlayer(player.row, player.col-1);
            move_monsters();
            break;
        case 'j':
            movePlayer(player.row-1, player.col);
            if(player.speed) movePlayer(player.row-1, player.col);
            move_monsters();
            break;
        case 'k' :
            movePlayer(player.row+1, player.col);
            if(player.speed) movePlayer(player.row+1, player.col);
            move_monsters();
            break;
        case 'l' :
            movePlayer(player.row, player.col+1);
            if(player.speed) movePlayer(player.row, player.col+1);
            move_monsters();
            break;
        case 'b':
            movePlayer(player.row+1, player.col-1);
            if(player.speed) movePlayer(player.row+1, player.col-1);
            move_monsters();
            break;
        case 'n':
            movePlayer(player.row+1, player.col+1);
            if(player.speed) movePlayer(player.row+1, player.col+1);
            move_monsters();
            break;
        case 'r':
            if(player.brokenkeys >= 2){
                player.ancientkeys ++;
                player.brokenkeys = player.brokenkeys -2;
            }
            break;
        case 'q':
            monsters_count = 0;
            save_map("saved.dat");
            return;
        case 'm':
            printall();
            break;
        case 'p':
            {WINDOW *winp = newwin(14, 76, 5, 10);
            keypad(winp, TRUE);
            box(winp, 0, 0);
            mvwprintw(winp, 1, 2, "-LIST OF POTIONS-");
            mvwprintw(winp, 3, 2, "You have %d health potion(s).", player.hps);
            mvwprintw(winp, 5,2, "You have %d speed potion(s).", player.sps);
            mvwprintw(winp, 7, 2, "You have %d damage potion(s).", player.dps);
            mvwprintw(winp, 9, 2, "Enter the first letter of the potion you want to consume!");
            wrefresh(winp);
            while(1){
                ch = getch();
                if(ch == 'h'){
                    int random = rand() % 4;
                    if(random == 0)mvwprintw(winp, 11, 2, "You chug it down. It tastes awful, but at least you're not dying... yet.");
                    else if(random == 1) mvwprintw(winp, 11, 2, "You feel reinvigorated! The dungeon is slightly less terrifying now.");
                    else if(random == 2) mvwprintw(winp, 11, 2, "Your wounds close up. Science? Magic? Who cares- it works!");
                    else if(random == 3) mvwprintw(winp, 11, 2, "You drink it. Your body appreciates it. Your taste buds do not.");
                    player.healthp = 1;
                    player.speed = 0;
                    player.power = 0;
                    wrefresh(winp);
                    getch();
                    break;
                }
                if(ch == 's'){
                    int random = rand() % 5;
                    if(random == 0) mvwprintw(winp, 11, 2, "Your legs start moving before your brain catches up. Hold on tight!");
                    if(random == 1) mvwprintw(winp, 11, 2, "You suddenly feel the need to run everywhere. Even when standing still.");
                    if(random == 2) mvwprintw(winp, 11, 2, "Your heartbeat accelerates. Either you're faster, or you're having a panic attack.");
                    if(random == 3) mvwprintw(winp, 11, 2, "You blink, and now you're five feet ahead. That was not normal.");
                    if(random == 4) mvwprintw(winp, 11, 2, "Your reflexes sharpen. Your feet demand adventure");
                    player.speed = 1;
                    player.power = 0;
                    player.healthp = 0;
                    wrefresh(winp);
                    getch();
                    break;
                }
                if(ch == 'd'){
                    int random = rand() % 3;
                    if(random == 0) mvwprintw(winp, 11, 2, "The weapon in your hand feels lighter. The enemies look more nervous.");
                    if(random == 1) mvwprintw(winp, 11, 2, "A rush of strength fills you! Your enemies have no idea what's coming");
                    if(random == 2) mvwprintw(winp, 11, 2, "You feel unstoppable. Maybe even too powerful. This should be fun.");
                    player.power  =1;
                    wrefresh(winp);
                    getch();
                    break;
                }
                if(ch == 27 || ch == 'q'){
                    break;
                }
            }
            delwin(winp);
                break;}
        case 'i':{
            WINDOW *winn = newwin(20, 60, 5, 10);
            keypad(winn, TRUE);
            box(winn, 0, 0);
            mvwprintw(winn, 1, 2, "-LIST OF WEAPONS-");
            wattron(winn, COLOR_PAIR(2));
            if(player.weapon_in_hand == 1) mvwprintw(winn, 3, 2, "You have a mace in your hand!");
            else if(player.weapon_in_hand == 2) mvwprintw (winn, 3, 2, "You have daggers in your hand!");
            else if(player.weapon_in_hand == 3) mvwprintw(winn, 3, 2, "You have magic wands in your hand!");
            else if(player.weapon_in_hand == 4) mvwprintw(winn, 3, 2, "You have normal arrows in your hand!");
            else if(player.weapon_in_hand == 5) mvwprintw(winn, 3, 2, "You have a sword in your hand!");
            wattroff(winn, COLOR_PAIR(2));
            if(player.mace != 0)mvwprintw(winn, 5, 2, "(m)%d mace(s)", player.mace);
            if(player.mace == 0)mvwprintw(winn, 5, 2, "You don't have any maces");
            if(player.dagger != 0) mvwprintw(winn, 7, 2, "(d)%d dagger(s) ", player.dagger);
            if(player.dagger == 0) mvwprintw(winn, 7, 2, "You don't have any daggers");
            if(player.magic_wand != 0) mvwprintw(winn, 9, 2, "(g)%d magic wand(s)", player.magic_wand);
            if(player.magic_wand == 0) mvwprintw(winn, 9, 2, "You don't have any magic wands");
            if(player.normal_arrow != 0) mvwprintw(winn, 11, 2, "(n)%d normal arrow(s)", player.normal_arrow);
            if(player.normal_arrow == 0) mvwprintw(winn, 11, 2, "You don't have any normal arrows");
            if(player.sword != 0) mvwprintw(winn, 13, 2, "(s)%d sword(s)", player.sword);
            if(player.sword == 0) mvwprintw(winn, 13, 2, "You don't have any swords");
            wrefresh(winn);
            while(1){
                ch = getch();
                if( ch == 'w'){
                    mvwprintw(winn, 15, 2, "                                                     ");
                    if(player.weapon_in_hand == 1) mvwprintw(winn, 15, 2, "You put your mace in your backpack!");
                    else if(player.weapon_in_hand == 2) mvwprintw (winn, 15, 2, "You put your daggers in your backpack!");
                    else if(player.weapon_in_hand == 3) mvwprintw(winn, 15, 2, "You put your magic_wands in your backpack!");
                    else if(player.weapon_in_hand == 4) mvwprintw(winn, 15, 2, "You put your normal arrows in your backpack!");
                    else if(player.weapon_in_hand == 5) mvwprintw(winn, 15, 2, "You put your sword in your backpack!");
                    else mvwprintw(winn, 13, 2, "You have nothing in your hand");
                    player.weapon_in_hand = 0;
                }
                else if(ch == 'm'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                       ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have a mace in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 1;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(1)); 
                        mvwprintw(winn, 15, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 'd'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have daggers in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 2;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(1));
                        mvwprintw(winn, 15, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 'g'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have magic wands in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 3;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(1));
                        mvwprintw(winn, 15, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 'n'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have normal arrows in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 4;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(1));
                        mvwprintw(winn, 15, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 's'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have swords in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 5;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                            ");
                        attron(COLOR_PAIR(1));
                        mvwprintw(winn, 13, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 'q' || ch == 27) break;
                else {
                    attron(COLOR_PAIR(1));
                    mvwprintw(winn, 15, 2, "                                                      ");
                    mvwprintw(winn, 15, 2, "You don't have any of these.");
                    attroff(COLOR_PAIR(1));
                }
                wrefresh(winn);
            }
            wrefresh(winn);
            delwin(winn);
            break;}
        case 'a':
            lastshot(last_shot);
            break;
        case 'e':
            {WINDOW *win = newwin(14, 50, 5, 10);
            keypad(win, TRUE);
            box(win, 0, 0);
            mvwprintw(win, 1, 2, "-LIST OF FOODS-");
            mvwprintw(win, 3, 2, "hunger : %d", max_health-player.health);
            mvwprintw(win,5, 2, "you have %d foods with you.", player.normal_food + player.fancy_food + player.magical_food + player.rotten_food);
            mvwprintw(win, 7, 2, "do you want to consume food? (y/n)");
            wrefresh(win);
            ch = getch();
            if(ch == 'y') {
                if(player.normal_food + player.fancy_food + player.magical_food +player.rotten_food == 0){
                    wattron(win, COLOR_PAIR(1));
                    mvwprintw(win, 9, 2, "You don't have any food with you");
                    wattroff(win, COLOR_PAIR(1));
                    wrefresh(win);
                }
                else{
                    int not_consumed  = 1;
                    while(not_consumed){
                        int random = rand() % 4;
                        if(random == 0 && player.normal_food > 0){
                            player.normal_food --;
                            not_consumed = 0;
                            if(player.health < max_health) player.health ++;
                            if(player.health < max_health && player.healthp) player.health ++;
                            mvwprintw(win, 9, 2, "You consumed a normal food!");
                            wrefresh(win);
                        }
                        else if(random == 1 && player.fancy_food > 0){
                            player.fancy_food --;
                            not_consumed = 0;
                            if(player.health < max_health) player.health ++;
                            if(player.health < max_health && player.healthp) player.health ++;
                            player.power = 1;
                            wattron(win, COLOR_PAIR(2));
                            mvwprintw(win , 9, 2, "You consumed a fancy food!");
                            wattroff(win, COLOR_PAIR(2));
                            wrefresh(win);
                        }
                        else if(random == 2 && player.magical_food > 0){
                            player.magical_food --;
                            not_consumed = 0;
                            if(player.health < max_health) player.health ++;
                            if(player.health < max_health && player.healthp) player.health ++;
                            player.speed = 1;
                            wattron(win, COLOR_PAIR(2));
                            mvwprintw(win, 9, 2, "You consumed a magical food!");
                            wattroff(win, COLOR_PAIR(2));
                            wrefresh(win);
                        }
                        else if(random == 3 && player.rotten_food > 0){
                            player.rotten_food --;
                            not_consumed = 0;
                            player.health  --;
                            if(player.health < max_health && player.healthp) player.health ++;
                            player.speed = 1;
                            wattron(win, COLOR_PAIR(1));
                            mvwprintw(win, 9, 2, "You consumed a rotten food!");
                            wattroff(win, COLOR_PAIR(1));
                            wrefresh(win);
                        }

                    }
                }
                getch();
                delwin(win);
            }
            else {
                delwin(win);
            }
            break;}
        case 'f':
            ch = getch();
            if(ch == 'y'){
                while(game_map[player.row-1][player.col-1] != WALLH && game_map[player.row-1][player.col-1] != WALLV && game_map[player.row-1][player.col-1] != WALLNO && game_map[player.row-1][player.col-1] != PILLAR && game_map[player.row-1][player.col-1] != END &&
                game_map[player.row-1][player.col-1] != ANCIENTKEY &&game_map[player.row-1][player.col-1] != STAIRCASE && game_map[player.row-1][player.col-1] != REVEALEDTRAP && game_map[player.row-1][player.col-1] != HEALTH_POTION && 
                game_map[player.row-1][player.col-1] != PASSWORDDOOR && game_map[player.row-1][player.col-1] != PASSWORD && game_map[player.row-1][player.col-1] != TRAP && game_map[player.row-1][player.col-1] != SPEED_POTION && game_map[player.row-1][player.col-1] != DAMAGE_POTION ){
                    movePlayer(player.row-1, player.col-1);
                    move_monsters();
                }
            }
            if(ch == 'u'){
                while(game_map[player.row-1][player.col+1] != WALLH && game_map[player.row-1][player.col+1] != WALLV && game_map[player.row-1][player.col+1] != WALLNO && game_map[player.row-1][player.col+1] != PILLAR && game_map[player.row-1][player.col+1] != END &&
                game_map[player.row-1][player.col+1] != ANCIENTKEY &&game_map[player.row-1][player.col+1] != STAIRCASE && game_map[player.row-1][player.col+1] != REVEALEDTRAP && game_map[player.row-1][player.col+1] != HEALTH_POTION && 
                game_map[player.row-1][player.col+1] != PASSWORDDOOR && game_map[player.row-1][player.col+1] != PASSWORD && game_map[player.row-1][player.col+1] != TRAP && game_map[player.row-1][player.col+1] != SPEED_POTION && game_map[player.row-1][player.col+1] != DAMAGE_POTION ){
                    movePlayer(player.row-1, player.col+1);
                    move_monsters();
                }
            }
            if(ch == 'l'){
                while(game_map[player.row][player.col+1] != WALLH && game_map[player.row][player.col+1] != WALLV && game_map[player.row][player.col+1] != WALLNO && game_map[player.row][player.col+1] != PILLAR && game_map[player.row][player.col+1] != END &&
                game_map[player.row][player.col+1] != ANCIENTKEY &&game_map[player.row][player.col+1] != STAIRCASE && game_map[player.row][player.col+1] != REVEALEDTRAP && game_map[player.row][player.col+1] != HEALTH_POTION && 
                game_map[player.row][player.col+1] != PASSWORDDOOR && game_map[player.row][player.col+1] != PASSWORD && game_map[player.row][player.col+1] != TRAP && game_map[player.row][player.col+1] != SPEED_POTION && game_map[player.row][player.col+1] != DAMAGE_POTION ){
                    movePlayer(player.row, player.col+1);
                    move_monsters();
                }
            }
            if(ch == 'j'){
                while(game_map[player.row-1][player.col] != WALLH && game_map[player.row-1][player.col] != WALLV && game_map[player.row-1][player.col] != WALLNO && game_map[player.row-1][player.col] != PILLAR && game_map[player.row-1][player.col] != END &&
                game_map[player.row-1][player.col] != ANCIENTKEY &&game_map[player.row-1][player.col] != STAIRCASE && game_map[player.row-1][player.col] != REVEALEDTRAP && game_map[player.row-1][player.col] != HEALTH_POTION && 
                game_map[player.row-1][player.col] != PASSWORDDOOR && game_map[player.row-1][player.col] != PASSWORD && game_map[player.row-1][player.col] != TRAP && game_map[player.row-1][player.col] != SPEED_POTION && game_map[player.row-1][player.col] != DAMAGE_POTION ){
                    movePlayer(player.row-1, player.col);
                    move_monsters();
                }
            }
            if(ch == 'k'){
                while(game_map[player.row+1][player.col] != WALLH && game_map[player.row+1][player.col] != WALLV && game_map[player.row+1][player.col] != WALLNO && game_map[player.row+1][player.col] != PILLAR && game_map[player.row+1][player.col] != END &&
                game_map[player.row+1][player.col] != ANCIENTKEY &&game_map[player.row+1][player.col] != STAIRCASE && game_map[player.row+1][player.col] != REVEALEDTRAP && game_map[player.row+1][player.col] != HEALTH_POTION && 
                game_map[player.row+1][player.col] != PASSWORDDOOR && game_map[player.row+1][player.col] != PASSWORD && game_map[player.row+1][player.col] != TRAP && game_map[player.row+1][player.col] != SPEED_POTION && game_map[player.row+1][player.col] != DAMAGE_POTION ){
                    movePlayer(player.row+1, player.col);
                    move_monsters();
                }
            }
            if(ch == 'h'){
                while(game_map[player.row][player.col-1] != WALLH && game_map[player.row][player.col-1] != WALLV && game_map[player.row][player.col-1] != WALLNO && game_map[player.row][player.col-1] != PILLAR && game_map[player.row][player.col-1] != END &&
                game_map[player.row][player.col-1] != ANCIENTKEY &&game_map[player.row][player.col-1] != STAIRCASE && game_map[player.row][player.col-1] != REVEALEDTRAP && game_map[player.row][player.col-1] != HEALTH_POTION && 
                game_map[player.row][player.col-1] != PASSWORDDOOR && game_map[player.row][player.col-1] != PASSWORD && game_map[player.row][player.col-1] != TRAP && game_map[player.row][player.col-1] != SPEED_POTION && game_map[player.row][player.col-1] != DAMAGE_POTION ){
                    movePlayer(player.row, player.col-1);
                    move_monsters();
                }
            }
            if(ch == 'b'){
                while(game_map[player.row+1][player.col-1] != WALLH && game_map[player.row+1][player.col-1] != WALLV && game_map[player.row+1][player.col-1] != WALLNO && game_map[player.row+1][player.col-1] != PILLAR && game_map[player.row+1][player.col-1] != END &&
                game_map[player.row+1][player.col-1] != ANCIENTKEY &&game_map[player.row+1][player.col-1] != STAIRCASE && game_map[player.row+1][player.col-1] != REVEALEDTRAP && game_map[player.row+1][player.col-1] != HEALTH_POTION && 
                game_map[player.row+1][player.col-1] != PASSWORDDOOR && game_map[player.row+1][player.col-1] != PASSWORD && game_map[player.row+1][player.col-1] != TRAP && game_map[player.row+1][player.col-1] != SPEED_POTION && game_map[player.row+1][player.col-1] != DAMAGE_POTION ){
                    movePlayer(player.row+1, player.col-1);
                    move_monsters();
                }
            }
            if(ch == 'n'){
                while(game_map[player.row+1][player.col+1] != WALLH && game_map[player.row+1][player.col+1] != WALLV && game_map[player.row+1][player.col+1] != WALLNO && game_map[player.row+1][player.col+1] != PILLAR && game_map[player.row+1][player.col+1] != END &&
                game_map[player.row+1][player.col+1] != ANCIENTKEY &&game_map[player.row+1][player.col+1] != STAIRCASE && game_map[player.row+1][player.col+1] != REVEALEDTRAP && game_map[player.row+1][player.col+1] != HEALTH_POTION && 
                game_map[player.row+1][player.col+1] != PASSWORDDOOR && game_map[player.row+1][player.col+1] != PASSWORD && game_map[player.row+1][player.col+1] != TRAP && game_map[player.row+1][player.col+1] != SPEED_POTION && game_map[player.row+1][player.col+1] != DAMAGE_POTION ){
                    movePlayer(player.row+1, player.col+1);
                    move_monsters();
                }
            }
            break;
        
        case 'g':
        g = 0;
        break;
        case ' ':
         throwweapon();
         break;
        default:
            break;
        }
    }
    clear();
    attron(COLOR_PAIR(1));
    print_message(3);
    attroff(COLOR_PAIR(1));
    refresh();
    napms(5000);
    getch();
    entry.score = 0;
    entry.gold_pieces = 0;
    entry.games_finished = 0;
    monsters_count = 0;
    sword_count = 0;
    pre_game_area();
}
bool load_user_data(HallOfFameEntry *entry) {
    FILE *file = fopen(HALLFILE, "r");
    if (!file) return false;

    bool found = false;
    HallOfFameEntry temp;
    while (fscanf(file, "%s %d %d %d %ld",
                  temp.username,
                  &temp.score,
                  &temp.gold_pieces,
                  &temp.games_finished,
                  &temp.first_game_time) == 5) {
        if (strcmp(temp.username, entry->username) == 0) {
            *entry = temp; // Load existing data into the struct
            found = true;
            break;
        }
    }
    fclose(file);
    return found;
}
void settings(){
    int choice =0; 
    char *options[] = {
        "Difficulty level",
        "Character color",
        "Choose a song",
        "Back"
    };
    int number_of_options = sizeof(options) / sizeof(options[0]);
    while(1){
        clear();
        for(int i=0; i<number_of_options; i++){
            if(i==choice){
                attron(A_REVERSE);
                mvprintw(3+i, 4, "%s", options[i]);
                attroff(A_REVERSE);
            }
            else{
                mvprintw(3+i, 4, "%s", options[i]);
            }
        }
        int key = getch();
        if(key == KEY_UP){
            choice = (choice - 1 +number_of_options) % number_of_options;
        }
        else if(key == KEY_DOWN){
            choice = (choice+1) % number_of_options;
        }
        else if(key == '\n'){
            if(choice == 0){
                difficulty_level();
            }
            if(choice==1){
                choose_color();
            }
            if(choice == 3){
                main_menu();
            }
        }
    }
}
void difficulty_level(){
    int choice =0;
    char *options[] = {
        "Barely an Adventurer",
        "Seasoned Explorer",
        "Dungeon Delver",
        "Master of Shadows",
        "Rogue of Rogues"
    };
    int number_of_options = sizeof(options) / sizeof(options[0]);
    while (1)
    {
        clear();
        mvprintw(1, 2, "-DIFFICULTY LEVEL-");
        for(int i=0; i<number_of_options; i++){
            if(i==choice){
                attron(A_REVERSE);
                mvprintw(3+i, 4, "%s", options[i]);
                attroff(A_REVERSE);
            }
            else{
                mvprintw(3+i, 4, "%s", options[i]);
            }
        }
        int key = getch();
        if(key == KEY_UP) choice = (choice-1 + number_of_options) % number_of_options;
        else if(key ==KEY_DOWN) choice = (choice +1) % number_of_options;
        else if(key =='\n'){
            if(choice ==0) {
                strcpy(game_mode, "Easy");
                return;
            }
            else if(choice == 1) {
                strcpy(game_mode, "Normal");
                return;
            }
            else if(choice == 2){
                strcpy(game_mode, "Hard");
                return;
            }
            else if(choice == 3){
                strcpy(game_mode, "Insane");
                return;
            }
            else if(choice == 4){
                strcpy(game_mode, "Legendary");
                return;
            }
        }
        else if(key == 27) return;
    }
    
}
void choose_color(){
    int choice =0;
    char *options[] = {
        "Blue",
        "Magenta",
        "Green",
        "White"
    };
    int number_of_options = sizeof(options) / sizeof(options[0]);
    while(1){
        clear();
        mvprintw(1, 2, "-CHOOSE A COLOR-");
        for(int i=0; i<number_of_options; i++){
            if(i==choice){
                attron(A_REVERSE);
                mvprintw(3+i, 4, "%s", options[i]);
                attroff(A_REVERSE);
            }
            else {
                mvprintw(3+i, 4, "%s", options[i]);
            }
        }
        int key = getch();
        if(key == KEY_UP) choice = (choice -1 +number_of_options) %number_of_options;
        if(key==KEY_DOWN) choice = (choice +1 ) % number_of_options;
        else if(key=='\n'){
            if(choice == 0) strcpy(character_color, "Blue");
            else if(choice==1) strcpy(character_color, "Magenta");
            else if(choice==2) strcpy(character_color, "Green");
            else if(choice == 3) strcpy(character_color, "White");
            return;
        }
        else if(key == 27) return;
    }
}
void initialize_map(){
    for(int i=0; i<MAXROW; i++){
        for(int j=0; j<MAXCOL; j++){
            visible[i][j]= 0;
            unlocked[i][j] = 0;
            type[i][j] = 0;
        }
    }
    for(int i=0; i<MAXROW; i++){
        for(int j=0; j<MAXCOL; j++){
            game_map[i][j] = EMPTY;
        }
    }
}
void createRoom(int row, int col, int roomRows, int roomCols){
    for(int i=row+1; i<roomRows+row; i++){
        for (int j= col+1; j<roomCols+col; j++){
            game_map[i][j] = FLOOR;
        }
    }
    for(int i= row+1; i<=roomRows+row; i++){
        game_map[i][col] = WALLV;
        game_map[i][col+roomCols] = WALLV;
        previous[i][col] = WALLV;
        previous[i][col+roomCols] = WALLV;
    }
    for(int j=col+1; j<roomCols+col; j++){
        game_map[row][j] = WALLH;
        game_map[row+roomRows][j] = WALLH;
        previous[row][j] = WALLH;
        previous[row+roomRows][j] = WALLH;
    }
    game_map[row][col] = WALLNO;
    game_map[row][col+roomCols] = WALLNO;
    previous[row][col] = WALLNO;
    previous[row][col+roomCols] = WALLNO;
}
int itoverlaps(int row, int col, int rowscount, int colscount){
    if (row + rowscount >= MAXROW-1 || col + colscount >= MAXCOL -1 ) return 1;
    for(int i= row-2; i<=rowscount+ row+2; i++){
        for(int j= col-2; j<=colscount+col+2; j++){
            if(game_map[i][j]!=EMPTY) return 1;
        }
    }
    return 0;
}
void place_door(int row, int col) {
    int random = rand();
    if (random % 20 == 0) {
        game_map[row][col] = (game_map[row][col] == WALLH) ? SWALLH :
                             (game_map[row][col] == WALLV) ? SWALLV :
                             SWALLNO;
    } else if (rand() % 16 == 0) { 
        game_map[row][col] = PASSWORDDOOR;
        int start_row = row, end_row = row, start_col = col, end_col = col;
        while (start_row > 0 && game_map[start_row - 1][col] != EMPTY && game_map[start_row - 1][col] != CORRIDOR ) start_row--;
        while (end_row < MAXROW - 1 && game_map[end_row + 1][col] != EMPTY && game_map[end_row + 1][col] != CORRIDOR ) end_row++;
        while (start_col > 0 && game_map[row][start_col - 1] != EMPTY && game_map[row][start_col - 1] != CORRIDOR) start_col--;
        while (end_col < MAXCOL - 1 && game_map[row][end_col + 1] != EMPTY && game_map[row][end_col + 1] != CORRIDOR) end_col++;
        int room_rows = end_row - start_row + 1;
        int room_cols = end_col - start_col + 1;
        placepassword(start_row, start_col, room_rows, room_cols);
    } else { 
        game_map[row][col] = DOOR;
    }
}

void corridors(int numRooms, Room *rooms) {
    for (int i = 0; i < numRooms - 1; i++) {
        int x1_pos = rooms[i].center_x;
        int y1_pos = rooms[i].center_y;
        int x2_pos = rooms[i + 1].center_x;
        int y2_pos = rooms[i + 1].center_y;
        if (game_map[y1_pos][x1_pos] == WALLH || game_map[y1_pos][x1_pos] == WALLV || game_map[y1_pos][x1_pos] == WALLNO) {
            place_door(y1_pos, x1_pos);
        }
        if (game_map[y2_pos][x2_pos] == WALLH || game_map[y2_pos][x2_pos] == WALLV || game_map[y2_pos][x2_pos] == WALLNO) {
            place_door(y2_pos, x2_pos);
        }
        int currentx = x1_pos;
        int currenty = y1_pos;
        while (currentx != x2_pos) {
            currentx += (currentx < x2_pos) ? 1 : -1;
            if (game_map[currenty][currentx] == EMPTY) {
                game_map[currenty][currentx] = CORRIDOR;
            } else if (game_map[currenty][currentx] == WALLH || game_map[currenty][currentx] == WALLV || game_map[currenty][currentx] == WALLNO) {
                place_door(currenty, currentx);
            }
        }
        while (currenty != y2_pos) {
            currenty += (currenty < y2_pos) ? 1 : -1;
            if (game_map[currenty][currentx] == EMPTY) {
                game_map[currenty][currentx] = CORRIDOR;
            } else if (game_map[currenty][currentx] == WALLH || game_map[currenty][currentx] == WALLV || game_map[currenty][currentx] == WALLNO) {
                place_door(currenty, currentx);
            }
        }
    }
}

void makeitvisible(int start_row,int start_col,int rowscount,int colscount){
    for(int i=start_row; i<= rowscount+start_row; i++){
        for(int j= start_col; j<=colscount + start_col; j++){
            visible[i][j] = 1;
        }
    }
    player.row = start_row  + 1;
    player.col = start_col + 1;
}
void generate_random_map(int staircase_row, int staircase_col, int staircase_roomRows, int staircase_roomCols) {
    initialize_map();
    int roomcount = rand() % 5 + 6;
    int traproom = rand() % roomcount;
    int stairroom = rand() % (roomcount-1) + 1;
    int ancientkeyroom = rand() % roomcount;
    int retry_count = 0;
    int extra_room = 0;
    int nightmare_room = rand() % (roomcount-1) + 1;
    room_index = 0;
    if (staircase_row >= 0 && staircase_col >= 0) {
        createRoom(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        placepotions(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        placegold(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        place_blackgold(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        createrandompillar(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        makeitvisible(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        makeitenchant(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        place_monsters(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        rooms[room_index].center_x = staircase_col + staircase_roomCols / 2;
        rooms[room_index].center_y = staircase_row + staircase_roomRows / 2;
        rooms[room_index].width = staircase_roomCols;
        rooms[room_index].length = staircase_roomRows;
        room_index++;
        extra_room =1;
    }
    for (int i = 0; i < roomcount; i++) {
        if(extra_room){
            extra_room=0; 
            roomcount --;
        }
        int start_row = rand() % (MAXROW - 9);
        int start_col = rand() % (MAXCOL - 9);
        int rowscount = rand() % 5 + 5;
        int colscount = rand() % 5 + 5;

        if (!itoverlaps(start_row, start_col, rowscount, colscount)) {
            createRoom(start_row, start_col, rowscount, colscount);
            placegold(start_row, start_col, rowscount, colscount);
            createrandompillar(start_row, start_col, rowscount, colscount);
            placenormalpotions(start_row, start_col, rowscount, colscount);
            placefood(start_row, start_col, rowscount, colscount);
            place_weapon(start_row, start_col, rowscount, colscount);
            place_monsters(start_row, start_col, rowscount, colscount);
            place_blackgold(start_row, start_col, rowscount, colscount);
            rooms[room_index].center_x = start_col + colscount / 2;
            rooms[room_index].center_y = start_row + rowscount / 2;
            rooms[room_index].width = colscount;
            rooms[room_index].length = rowscount;
            if(room_index == nightmare_room) makeitnightmare(start_row, start_col,   rowscount, colscount);
            if(room_index == ancientkeyroom) placeancientkey(start_row, start_col, rowscount, colscount);
            if(room_index == 0 && current_floor == 0) {
                player.row = start_row + 1;
                player.col = start_col + 1;
                makeitvisible(start_row, start_col, rowscount, colscount);
            }
            if (room_index == traproom) placetrap(start_row, start_col, rowscount, colscount);
            if (room_index == stairroom && current_floor!=3 ) placestaircase(start_row, start_col, rowscount, colscount);
            if(room_index == stairroom && current_floor == 3) make_it_treasure(start_row, start_col, rowscount, colscount);
            room_index++;
        } else {
            i--;
            continue;
        }
    }
    corridors(room_index, rooms);
}

void removeUnconnectedDoors() {
    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            if (game_map[i][j] == DOOR || game_map[i][j] == PASSWORDDOOR || game_map[i][j]== SWALLH || game_map[i][j] == SWALLV || game_map[i][j] == SWALLNO) {
                int connected = 0;
                if (i > 0 && game_map[i - 1][j] == CORRIDOR) connected = 1; // Up
                if (i < MAXROW - 1 && game_map[i + 1][j] == CORRIDOR) connected = 1; // Down
                if (j > 0 && game_map[i][j - 1] == CORRIDOR) connected = 1; // Left
                if (j < MAXCOL - 1 && game_map[i][j + 1] == CORRIDOR) connected = 1; // Right

                if (!connected) {
                    if(game_map[i][j] == PASSWORDDOOR){
                        int start_row = i, end_row = i, start_col = j, end_col = j;
                        while (start_row > 0 && game_map[start_row - 1][j] != EMPTY && game_map[start_row - 1][j] != CORRIDOR ) start_row--;
                        while (end_row < MAXROW - 1 && game_map[end_row + 1][j] != EMPTY && game_map[end_row + 1][j] != CORRIDOR ) end_row++;
                        while (start_col > 0 && game_map[i][start_col - 1] != EMPTY && game_map[i][start_col - 1] != CORRIDOR) start_col--;
                        while (end_col < MAXCOL - 1 && game_map[i][end_col + 1] != EMPTY && game_map[i][end_col + 1] != CORRIDOR) end_col++;
                        int room_rows = end_row - start_row + 1;
                        int room_cols = end_col - start_col + 1;
                        removepassword(start_row, start_col, room_rows, room_cols);
                             }
                    game_map[i][j] = previous[i][j]; 
                    
                }
            }
        }
    }
}
void movePlayer(int newRow, int newCol) {
    if(moves % 75 == 74){
        player.health--;
    }
    if(moves % 20 == 19){
        if(player.speed) {
            player.speed = 0;
            current_message = 0;
        }
        if(player.power){
            player.power = 0;
            current_message = 1;
        }
        if(player.healthp){
            player.healthp = 0;
            current_message = 2;
        }
    }
    if(moves % 70 == 69){
        int random = rand() % 3;
        if(random == 0 && player.fancy_food != 0){
            player.fancy_food --;
            player.normal_food ++;
        }
        else if(random == 1 && player.magical_food != 0){
            player.magical_food --;
            player.normal_food ++;
        }
        else if(random == 2 && player.normal_food != 0){
            player.normal_food --;
            player.rotten_food ++;
        }
    }
    if (newRow >= 0 && newRow < MAXROW && newCol >= 0 && newCol < MAXCOL &&
        (game_map[newRow][newCol] == FLOOR || game_map[newRow][newCol] == DOOR || game_map[newRow][newCol] == CORRIDOR || game_map[newRow][newCol]==SWALLH || game_map[newRow][newCol] == SWALLV || game_map[newRow][newCol] == SWALLNO || game_map[newRow][newCol] == STAIRCASE)) {
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == SWALLH  || game_map[newRow][newCol] == SWALLV || game_map[newRow][newCol] == SWALLNO) visible[newRow][newCol]=1;
    for(int i = -1; i<=1; i++){
        for(int j= -1; j<=1; j++){
            int r= newRow+i, c = newCol+j;
            if(r>=0 && r<MAXROW && c>= 0 && c <MAXCOL && game_map[newRow][newCol]==CORRIDOR){
                if(game_map[r][c]== CORRIDOR) visible[r][c]=1;
            }
        }
    }
    if(game_map[newRow][newCol] == ANCIENTKEY ) {
        if(g){player.ancientkeys ++;
        game_map[newRow][newCol] = FLOOR;
        player.row = newRow;
        player.col = newCol;}
        else{
        player.row = newRow;
        player.col = newCol;
        }
    }
    if(game_map[newRow][newCol] == dagger_SHOT){
        if(g){
            game_map[newRow][newCol] = FLOOR;
            player.dagger ++;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == DAGGER){
        if(g){
            game_map[newRow][newCol] = FLOOR;
            player.dagger += 12;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == MAGIC_WAND){
        if(g){
            game_map[newRow][newCol] = FLOOR;
            player.magic_wand += 8;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == magic_wand_SHOT){
        if(g){
            game_map[newRow][newCol] = FLOOR;
            player.magic_wand ++;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == NORMAL_ARROW){
        if(g){
            game_map[newRow][newCol] = FLOOR;
            player.normal_arrow += 20;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == normal_arrow_SHOT){
        if(g){
            game_map[newRow][newCol] = FLOOR;
            player.normal_arrow ++;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == SWORD){
        if(g){
            game_map[newRow][newCol] = FLOOR;
            player.sword ++;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if (game_map[newRow][newCol] == PASSWORD) {
    generatedoorpass();
    mvprintw(1, 75, "%s", pass);
    refresh();
    napms(10000);
}
    if(game_map[newRow][newCol] == nFOOD){
        if(g && (player.normal_food + player.fancy_food + player.magical_food + player.rotten_food )< 10){
            game_map[newRow][newCol] = FLOOR;
            player.normal_food ++;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == fFOOD){
        if(g && (player.normal_food + player.fancy_food + player.magical_food + player.rotten_food )< 10){
            game_map[newRow][newCol] = FLOOR;
            player.fancy_food ++;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == mFOOD){
        if(g && (player.normal_food + player.fancy_food + player.magical_food + player.rotten_food )< 10){
            game_map[newRow][newCol] = FLOOR;
            player.magical_food ++;
        }
        player.row = newRow;
        player.col = newCol;
    }
    if(game_map[newRow][newCol] == rFOOD){
        if(g && (player.normal_food + player.fancy_food + player.magical_food + player.rotten_food )< 10){
            game_map[newRow][newCol] = FLOOR;
            player.rotten_food ++;
        }
        player.row = newRow;
        player.col = newCol;
    }
if(game_map[newRow][newCol] == GOLD){
    if(g){game_map[newRow][newCol] = FLOOR;
    player.row = newRow;
    player.col = newCol;
    current_message =7;
    player.golds ++;}
    else{
    player.row = newRow;
    player.col = newCol;
    }
}
if(game_map[newRow][newCol] == BLACKGOLD){
    if(g){game_map[newRow][newCol] = FLOOR;
    player.row = newRow;
    player.col = newCol;
    current_message = 7;
    player.golds += 5;}
    else{
    player.row = newRow;
    player.col = newCol;
    }
}

if (game_map[newRow][newCol] == PASSWORDDOOR) {
    if (unlocked[newRow][newCol]) {
        player.row = newRow;
        player.col = newCol;
    } 
    
    
    if(!unlocked[newRow][newCol]){
        int random =rand()%10 ;
        if(player.ancientkeys == 1 && random%10 !=0) {
                attron(COLOR_PAIR(2));
                mvprintw(1, 2, "                                                                                  ");
                mvprintw(1, 2, "You unlocked the door!");
                attroff(COLOR_PAIR(2));
                refresh();
                unlocked[newRow][newCol] = 1;
                player.row = newRow;
                player.col = newCol;
                getch();
        }
        if(player.ancientkeys== 1 && random%10 == 0 ){
            attron(COLOR_PAIR(2));
            mvprintw(1, 2, "                                                                                  ");
            mvprintw(1, 2, "You broke your ancient key:(  if you have two broken ancient keys press r");    
            attroff(COLOR_PAIR(2));
            refresh();
            player.ancientkeys --;
            player.brokenkeys ++;
            getch();
        }
        if(!unlocked[newRow][newCol]){
        attron(COLOR_PAIR(1));
        mvprintw(1, 2, "                                                                                  ");
        mvprintw(1, 2, "This door is locked! If you dont have the password press q!");
        attroff(COLOR_PAIR(1));
        refresh();
        
        char input[5] = {0};
        int retries = 3;
        
        while (retries--) {
            int correct = 1;
            for (int i = 0; i < 4; i++) {
                int ch = getch();
                if (ch == 'q') {
                    return; 
                }
                
                input[i] = ch; 
                
            }
            input[4] = '\0';
            if (strcmp(input, pass)!= 0) {
                    correct = 0;
                }
            if (correct) {
                attron(COLOR_PAIR(2));
                mvprintw(1, 2, "                                                                                  ");
                int random = rand() % 4;
                if(random == 0)mvprintw(1, 2, "With a satisfying click, the door swings open. You feel unstoppable.");
                else if(random == 1) mvprintw(1, 2, "The lock gives in. The door creaks open, revealing what lies beyond...");
                else if(random == 2) mvprintw(1, 2, "You hear a faint click- success! You try to act like you knew what you were doing.");
                else if(random == 3) mvprintw(1, 2, "Victory! The door swings open. Time to see if it was worth the trouble.");
                attroff(COLOR_PAIR(2));
                refresh();
                unlocked[newRow][newCol] = 1;
                getch();
                break;
            } else {
                if (retries == 2) {
                    attron(COLOR_PAIR(5));
                    mvprintw(1, 2, "                                                                                  ");
                    mvprintw(1, 2, "Wrong password! Try again!");
                    attroff(COLOR_PAIR(5));
                    refresh();
                }
                if (retries == 1) {
                    mvprintw(1, 2, "                                                                                  ");
                    attron(COLOR_PAIR(1));
                    mvprintw(1, 2, "Wrong password! Try again!");
                    attroff(COLOR_PAIR(1));
                    refresh();
                }
                if (retries == 0) {
                    break;
                }
            }
        }
        }
    }
}
if(game_map[newRow][newCol] == HEALTH_POTION){
    if(g){game_map[newRow][newCol] = FLOOR;
    player.hps ++; }
    player.row = newRow;
    player.col = newCol;
}
if(game_map[newRow][newCol] == SPEED_POTION){
    if(g){game_map[newRow][newCol] = FLOOR;
    player.sps ++;}
    player.row = newRow;
    player.col = newCol;
    
}
if(game_map[newRow][newCol] == DAMAGE_POTION ){
    if(g){game_map[newRow][newCol] = FLOOR;
    player.dps ++;}
    player.row = newRow;
    player.col = newCol;
}
if(game_map[newRow][newCol] == TRAP) {
    battleroom();
    game_map[newRow][newCol] = REVEALEDTRAP;
}
if (game_map[newRow][newCol] == STAIRCASE && current_floor< 3) {
        
        int start_row = newRow, end_row = newRow, start_col = newCol, end_col = newCol;
        while (start_row > 0 && game_map[start_row - 1][newCol] != EMPTY && game_map[start_row - 1][newCol] !=CORRIDOR) start_row--;
        while (end_row < MAXROW - 1 && game_map[end_row + 1][newCol] != EMPTY && game_map[end_row + 1][newCol] != CORRIDOR) end_row++;
        while (start_col > 0 && game_map[newRow][start_col - 1] != EMPTY && game_map[newRow][start_col - 1] != CORRIDOR) start_col--;
        while (end_col < MAXCOL - 1 && game_map[newRow][end_col + 1] != EMPTY && game_map[newRow][end_col + 1] != CORRIDOR) end_col++;
        int room_rows = end_row - start_row + 1;
        int room_cols = end_col - start_col + 1;
        monsters_count = 0;
        current_floor++;
        player.score += player.golds * (current_floor + 1) / 3;
        generate_random_map(start_row, start_col, room_rows-1, room_cols-1);
        makeitvisible(start_row, start_col, room_rows, room_cols);
        player.row = start_row + room_rows / 2;
        player.col = start_col + room_cols / 2;
        current_message =5;

    }


if((game_map[newRow][newCol] == DOOR || game_map[newRow][newCol]==SWALLH
    || game_map[newRow][newCol]==SWALLV || game_map[newRow][newCol]==SWALLNO
    || (game_map[newRow][newCol]==PASSWORDDOOR && unlocked[newRow][newCol])) && type[newRow][newCol] != 1) {
        int k=0;
        for(int i=0; game_map[newRow + i][newCol]!= EMPTY && game_map[newRow + i][newCol]!= CORRIDOR; i++){
            k++;
        }
        int end_row = newRow + k ;
        k =0;
        for(int i=0; game_map[newRow + i][newCol]!= EMPTY && game_map[newRow+i][newCol]!= CORRIDOR; i--){
            k++;
        }
        int start_row = newRow - k;
        k= 0;
        for(int i=0; game_map[newRow][newCol+i]!= EMPTY && game_map[newRow][newCol+i]!= CORRIDOR; i++){
            k++;
        }
        int end_column = newCol + k ;
        k=0;
        for(int i=0; game_map[newRow][newCol+i]!= EMPTY && game_map[newRow][newCol+i]!= CORRIDOR; i--){
            k++;
        }
        int start_column = newCol - k;
        for(int i=start_row; i<end_row; i++){
            for(int j= start_column; j< end_column; j++){
                visible[i][j] = 1;
            }
        }
        current_message = 6;
    }
    for(int i =0; i<monsters_count; i++){
        if(player.row >= monster[i].row - 1 && player.row <= monster[i].row + 1
        && player.col >= monster[i].col - 1 && player.col <= monster[i].col + 1 
        && monster[i].alive ){
        if(monster[i].moves < monster[i].max_moves) monster[i].moving = 1;
        if(strcmp(monster[i].name, "Deamon") == 0) current_message = 19;
        else if(strcmp(monster[i].name, "Giant") == 0) current_message = 20;
        else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
        else if(strcmp(monster[i].name, "Snake") == 0) current_message = 22;
        else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 23;
        player.health --;
        }
    }
moves ++;
if(game_map[newRow][newCol] == END){
    clear();
    player.score += player.golds * (current_floor + 1) / 3;
    print_message(8);
    refresh();
    napms(5000);
    getch();
    HallOfFameEntry existing_entry = {0};
    strcpy(existing_entry.username, current_username);
    bool isExistingUser = load_user_data(&existing_entry);
    int score_delta = player.score;
    int gold_delta = player.golds;
    int games_delta = 1;
    HallOfFameEntry delta_entry = {0};
    strcpy(delta_entry.username, current_username);
    delta_entry.score = score_delta;
    delta_entry.gold_pieces = gold_delta;
    delta_entry.games_finished = games_delta;
    if (!isExistingUser) {
        delta_entry.first_game_time = time(NULL);
    }
    savehall(delta_entry);
    monsters_count = 0;
    sword_count = 0;
    main_menu();
}
}
void printMap() {
    clear(); 

    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            
            if(visible[i][j]){
                
                switch (game_map[i][j]) {
                case WALLV: 
                if(type[i][j] == 2) attron(COLOR_PAIR(3));
                if(type[i][j] == 3) attron(COLOR_PAIR(5));
                mvaddch(i, j, '|'); 
                if(type[i][j] == 2) attroff(COLOR_PAIR(3));
                if(type[i][j] == 3) attroff(COLOR_PAIR(5));
                break;
                case FLOOR: mvaddch(i, j, '.'); break;
                case dagger_SHOT: mvaddch(i, j, ','); break;
                case magic_wand_SHOT: mvaddch(i, j, ','); break;
                case normal_arrow_SHOT: mvaddch(i, j, ','); break;
                case DOOR: mvaddch(i, j, '+'); break;
                case CORRIDOR: mvaddch(i, j, '#'); break;
                case PILLAR: mvaddch(i, j, 'O'); break;
                case WINDO: mvaddch(i, j, '='); break;
                case EMPTY: mvaddch(i, j, ' '); break;
                case WALLH: 
                if(type[i][j] == 3) attron(COLOR_PAIR(5));
                if(type[i][j] == 2) attron(COLOR_PAIR(3));
                mvaddch(i, j, '_');
                if(type[i][j] == 2) attroff(COLOR_PAIR(3));
                if(type[i][j] == 3) attroff(COLOR_PAIR(5));
                 break;
                case WALLNO: mvaddch(i, j, ' '); break;
                case TRAP: mvaddch(i, j, '.'); break;
                case STAIRCASE: mvaddch(i, j, '<'); break;
                case SWALLH: 
                if(type[i][j] == 3) attron(COLOR_PAIR(5));
                if(type[i][j] == 2) attron(COLOR_PAIR(3));
                mvaddch(i, j, '_');
                if(type[i][j] == 2) attroff(COLOR_PAIR(3));
                if(type[i][j] == 3) attroff(COLOR_PAIR(5));
                 break;
                case SWALLV:
                if(type[i][j] == 3) attron(COLOR_PAIR(5));
                if(type[i][j] == 2) attron(COLOR_PAIR(3)); 
                mvaddch(i, j, '|'); 
                if(type[i][j] == 2) attroff(COLOR_PAIR(3));
                if(type[i][j] == 3) attroff(COLOR_PAIR(5));
                break;
                case SWALLNO: mvaddch(i, j, ' '); break;
                case ANCIENTKEY: 
                    attron(COLOR_PAIR(5));
                    mvaddch(i, j, '^');
                    attroff(COLOR_PAIR(4));
                     break;
                case PASSWORD:
                    attron(COLOR_PAIR(5));
                    mvaddch(i, j, '&');
                    attroff(COLOR_PAIR(5));
                    break;
                case PASSWORDDOOR:
                    if(unlocked[i][j]) {
                        attron(COLOR_PAIR(2));
                        mvaddch(i, j, '?');
                        attroff(COLOR_PAIR(2));
                    }
                    else {
                        attron(COLOR_PAIR(1));
                        mvaddch(i, j, '@');
                        attroff(COLOR_PAIR(1));
                    }
                    break;
                case HEALTH_POTION: 
                attron(COLOR_PAIR(3));
                mvaddch(i, j, 'h'); 
                attroff(COLOR_PAIR(3));
                break;
                case DAMAGE_POTION: 
                attron(COLOR_PAIR(3));
                mvaddch(i, j, 'd'); 
                attroff(COLOR_PAIR(3));
                break;
                case SPEED_POTION: 
                attron(COLOR_PAIR(3));
                mvaddch(i, j, 's'); 
                attroff(COLOR_PAIR(3));
                break;
                case REVEALEDTRAP: mvaddch(i, j, '*'); break;
                case GOLD:
                attron(COLOR_PAIR(5));
                mvaddch(i, j, 'G');
                attroff(COLOR_PAIR(5));
                break;
                case BLACKGOLD:
                attron(COLOR_PAIR(7));
                mvaddch(i, j, 'G');
                attroff(COLOR_PAIR(7));
                break;
                case rFOOD:
                mvaddch(i, j,  'n');
                break;
                case nFOOD:
                mvaddch(i, j,  'n');
                break;
                case fFOOD:
                attron(COLOR_PAIR(2));
                mvaddch(i, j, 'f');
                attroff(COLOR_PAIR(2));
                break;
                case mFOOD:
                mvaddch(i, j, 'm');
                break;
                case DAGGER:
                attron(COLOR_PAIR(8));
                mvaddch(i, j, 'D');
                attroff(COLOR_PAIR(8));
                break;
                case MAGIC_WAND:
                attron(COLOR_PAIR(8));
                mvaddch(i, j, 'W');
                attroff(COLOR_PAIR(8));
                break;
                case NORMAL_ARROW:
                attron(COLOR_PAIR(8));
                mvaddch(i, j, 'A');
                attroff(COLOR_PAIR(8));
                break;
                case SWORD:
                attron(COLOR_PAIR(8));
                mvaddch(i, j, 'S');
                attroff(COLOR_PAIR(8));
                break;
                case END: 
                attron(COLOR_PAIR(6));
                mvaddch(i, j, '~');
                attroff(COLOR_PAIR(6));
                break;
                default: mvaddch(i, j, ' '); break;
                
            }
            }
        }
    }
    if(type[player.row][player.col] == 1){for(int i = -2; i <=2; i++){
        for(int j = -2; j<=2; j++){
             switch (game_map[player.row+i][player.col+ j]) {
                case WALLV: 
                mvaddch(player.row+i, player.col+ j, '|'); 
                break;
                case FLOOR: mvaddch(player.row+i, player.col+ j, '.'); break;
                case dagger_SHOT: mvaddch(player.row+i, player.col+ j, ','); break;
                case magic_wand_SHOT: mvaddch(player.row+i, player.col+ j, ','); break;
                case normal_arrow_SHOT: mvaddch(player.row+i, player.col+ j, ','); break;
                case DOOR: mvaddch(player.row+i, player.col+ j, '+'); break;
                case CORRIDOR: mvaddch(player.row+i, player.col+ j, '#'); break;
                case PILLAR: mvaddch(player.row+i, player.col+ j, 'O'); break;
                case WINDO: mvaddch(player.row+i, player.col+ j, '='); break;
                case EMPTY: mvaddch(player.row+i, player.col+ j, ' '); break;
                case WALLH:
                mvaddch(player.row+i, player.col+ j, '_');
                 break;
                case WALLNO: mvaddch(player.row+i, player.col+ j, ' '); break;
                case TRAP: mvaddch(player.row+i, player.col+ j, '.'); break;
                case STAIRCASE: mvaddch(player.row+i, player.col+ j, '<'); break;
                case SWALLH:
                mvaddch(player.row+i, player.col+ j, '_');
                 break;
                case SWALLV: 
                mvaddch(player.row+i, player.col+ j, '|');
                break;
                case SWALLNO: mvaddch(player.row+i, player.col+ j, ' '); break;
                case ANCIENTKEY:
                    mvaddch(player.row+i, player.col+ j, '^');
                     break;
                case PASSWORD:
                    mvaddch(player.row+i, player.col+ j, '&');
                    break;
                case PASSWORDDOOR:
                    if(unlocked[i][j]) {
                        mvaddch(player.row+i, player.col+ j, '?');
                    }
                    else {
                        mvaddch(player.row+i, player.col+ j, '@');
                    }
                    break;
                case HEALTH_POTION:
                mvaddch(player.row+i, player.col+ j, 'h');
                break;
                case DAMAGE_POTION:
                mvaddch(player.row+i, player.col+ j, 'd'); 
                break;
                case SPEED_POTION:
                mvaddch(player.row+i, player.col+ j, 's'); 
                break;
                case REVEALEDTRAP: mvaddch(player.row+i, player.col+ j, '*'); break;
                case GOLD:
                mvaddch(player.row+i, player.col+ j, 'G');
                break;
                case BLACKGOLD:
                mvaddch(player.row+i, player.col+ j, 'G');
                break;
                case rFOOD:
                mvaddch(player.row+i, player.col+ j,  'n');
                break;
                case nFOOD:
                mvaddch(player.row+i, player.col+ j,  'n');
                break;
                case fFOOD:
                mvaddch(player.row+i, player.col+ j, 'f');
                break;
                case mFOOD:
                mvaddch(player.row+i, player.col+ j, 'm');
                break;
                case DAGGER:
                mvaddch(player.row+i, player.col+ j, 'D');
                break;
                case MAGIC_WAND:
                mvaddch(player.row+i, player.col+ j, 'W');
                break;
                case NORMAL_ARROW:
                mvaddch(player.row+i, player.col+ j, 'A');
                break;
                case SWORD:
                mvaddch(player.row+i, player.col+ j, 'S');
                break;
                case END:
                mvaddch(player.row+i, player.col+ j, '~');
                break;
                default: mvaddch(player.row+i, player.col+ j, ' '); break;
        }
    }
    }}
    if(strcmp(character_color, "Blue")==0) {
        attron(COLOR_PAIR(4));
        mvaddch(player.row, player.col, 'P');
        attroff(COLOR_PAIR(4));
    }
    else if(strcmp(character_color, "Magenta")==0){
        attron(COLOR_PAIR(3));
        mvaddch(player.row, player.col, 'P');
        attroff(COLOR_PAIR(3));
    }
    else if(strcmp(character_color, "Green")==0){
        attron(COLOR_PAIR(2));
        mvaddch(player.row, player.col, 'P');
        attroff(COLOR_PAIR(2));
    }
    else {
    mvaddch(player.row, player.col, 'P'); 
    }
    attron(COLOR_PAIR(1));
    for(int i = 0 ; i < monsters_count; i++){
            if(strcmp(monster[i].name, "Deamon") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'D');
            else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'F');
            else if(strcmp(monster[i].name, "Giant") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'G');
            else if(strcmp(monster[i].name, "Snake") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'S');
            else if(strcmp(monster[i].name, "Undeed") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'U');
    }
    attroff(COLOR_PAIR(1));
    refresh();
}

void createrandompillar(int row, int col, int roomRows, int roomCols){
    int number_of_pillars = rand() % 2 ;
    while(number_of_pillars --){
        int rown = ( rand() %(roomRows-2) ) +row+1;
        int coln = (rand() % (roomCols-2)) +col +1;
        if(game_map[rown][coln] == FLOOR )game_map[rown][coln] = PILLAR;
        else {
            number_of_pillars ++;
        }
    }
}
void placetrap(int row, int col, int roomRows, int roomCols){
    int rown = (rand() %(roomRows-2)) + row +1;
    int coln = (rand() % (roomCols-2)) + col +1;
    if(game_map[rown][coln] == FLOOR) game_map[rown][coln] = TRAP;
    else{
        placetrap(row, col, roomRows, roomCols);
    }
}
void battleroom(){
    winb = newwin(30, 60, 4,60);
    keypad(winb, TRUE);
    box(winb, 0, 0);
    battle_map[0][0] = WALLNO;
    battle_map[0][39]= WALLNO;
    battle_map[19][0]= WALLV;
    battle_map[19][39] = WALLV;
    for(int i = 1; i < 39; i++){
        battle_map[0][i] = WALLH;
        battle_map[19][i] = WALLH;
    }
    for(int i = 1; i< 19; i++){
        battle_map[i][0] = WALLV;
        battle_map[i][39] = WALLV;
    }
    for(int i =1; i< 39; i++){
        for(int j = 1; j< 19; j++){
            battle_map[j][i] = FLOOR;
        }
    }
    player.battle_row = 1;
    player.battle_col = 1;
    place_battle_monsters();
    while(battle_room_monsters_count > 0 && player.health > 0){
        printbattleMap();
        mvwprintw(winb, 1, 2, "Health: %d/%d", player.health,max_health);
        wrefresh(winb);
        print_message(current_message);
        int ch = getch();
        switch(ch){
            case 'y':
            battle_moveplayer(player.battle_row - 1, player.battle_col - 1);
            battle_movemonsters();
            break;
            case 'u':
            battle_moveplayer(player.battle_row - 1, player.battle_col +1);
            battle_movemonsters();
            break;
            case 'h':
            battle_moveplayer(player.battle_row , player.battle_col -1);
            battle_movemonsters();
            break;
            case 'j':
            battle_moveplayer(player.battle_row -1, player.battle_col);
            battle_movemonsters();
            break;
            case 'k':
            battle_moveplayer(player.battle_row + 1, player.battle_col);
            battle_movemonsters();
            break;
            case 'l':
            battle_moveplayer(player.battle_row, player.battle_col+1);
            battle_movemonsters();
            break;
            case 'b':
            battle_moveplayer(player.battle_row + 1, player.battle_col - 1);
            battle_movemonsters();
            break;
            case 'n':
            battle_moveplayer(player.battle_row + 1, player.battle_col + 1);
            battle_movemonsters();
            break;
            case ' ':
            battle_throwweapon();
            break;
            case 'q':
            delwin(winb);
            return;
            default: 
            break;
        }
    }
    delwin(winb);
    current_message = 24;
}
void place_battle_monsters(){
    int count = rand() % 11 + 10;
    battle_room_monsters_count = 0;
    while (count --)
    {
        int row = rand() % 18 + 1;
        int col = rand() % 38 + 1;
        int which = rand() % 5;
        if(which == 0){
            battle_monster[battle_room_monsters_count].row = row;
            battle_monster[battle_room_monsters_count].col = col;
            battle_monster[battle_room_monsters_count].hits = 0;
            battle_monster[battle_room_monsters_count].lives = 5;
            battle_monster[battle_room_monsters_count].max_moves = 0;
            battle_monster[battle_room_monsters_count].alive = 1;
            battle_monster[battle_room_monsters_count].moves = 0;
            strcpy(battle_monster[battle_room_monsters_count].name, "Deamon");
            battle_monster[battle_room_monsters_count].moving = 0;
            battle_room_monsters_count++;
        }
        else if(which == 1){
            battle_monster[battle_room_monsters_count].row = row;
            battle_monster[battle_room_monsters_count].col = col;
            battle_monster[battle_room_monsters_count].hits = 0;
            battle_monster[battle_room_monsters_count].lives = 10;
            battle_monster[battle_room_monsters_count].max_moves = 0;
            battle_monster[battle_room_monsters_count].alive = 1;
            battle_monster[battle_room_monsters_count].moves = 0;
            strcpy(battle_monster[battle_room_monsters_count].name, "Fire Breathing Monster");
            battle_monster[battle_room_monsters_count].moving = 0;
            battle_room_monsters_count++;
        }
        else if(which == 2){
            battle_monster[battle_room_monsters_count].row = row;
            battle_monster[battle_room_monsters_count].col = col;
            battle_monster[battle_room_monsters_count].hits = 0;
            battle_monster[battle_room_monsters_count].lives = 15;
            battle_monster[battle_room_monsters_count].max_moves = 5;
            battle_monster[battle_room_monsters_count].moves = 0;
            battle_monster[battle_room_monsters_count].alive = 1;
            strcpy(battle_monster[battle_room_monsters_count].name, "Giant");
            battle_monster[battle_room_monsters_count].moving = 0;
            battle_room_monsters_count ++;
        }
        else if(which == 3){
            battle_monster[battle_room_monsters_count].row = row;
            battle_monster[battle_room_monsters_count].col = col;
            battle_monster[battle_room_monsters_count].hits = 0;
            battle_monster[battle_room_monsters_count].lives = 20;
            battle_monster[battle_room_monsters_count].max_moves = 2000;
            battle_monster[battle_room_monsters_count].moves = 0;
            battle_monster[battle_room_monsters_count].alive = 1;
            strcpy(battle_monster[battle_room_monsters_count].name, "Snake");
            battle_monster[battle_room_monsters_count].moving = 0;
            battle_room_monsters_count ++;
        }
        else if(which == 4){
            battle_monster[battle_room_monsters_count].row = row;
            battle_monster[battle_room_monsters_count].col = col;
            battle_monster[battle_room_monsters_count].hits = 0;
            battle_monster[battle_room_monsters_count].lives = 30;
            battle_monster[battle_room_monsters_count].max_moves = 5;
            battle_monster[battle_room_monsters_count].moves = 0;
            battle_monster[battle_room_monsters_count].alive = 1;
            strcpy(battle_monster[battle_room_monsters_count].name, "Undeed");
            battle_monster[battle_room_monsters_count].moving = 0;
            battle_room_monsters_count ++;
        }
    }
}
void battle_throwweapon(){
    if(rand() % 5 != 0){
        for (int i =0 ; i< battle_room_monsters_count; i++){
            if(!battle_monster[i].alive) continue;
            if(abs(battle_monster[i].row - player.battle_row) <=1 &&
            abs(battle_monster[i].col - player.battle_col) <=1){
                battle_monster[i].hits += 5;
                if(strcmp(battle_monster[i].name, "Deamon") == 0) current_message = 9;
                else if(strcmp(battle_monster[i].name, "Fire Breathing Monster") == 0) current_message = 10;
                else if(strcmp(battle_monster[i].name, "Giant") == 0) current_message = 11;
                else if(strcmp(battle_monster[i].name, "Snake") == 0) current_message = 12;
                else if(strcmp(battle_monster[i].name, "Undeed") == 0) current_message = 13;
                if(battle_monster[i].hits >= battle_monster[i].lives){
                    battle_monster[i].alive = 0;
                    battle_room_monsters_count --;
                    battle_monster[i].moves = battle_monster[i].max_moves;
                    if(strcmp(battle_monster[i].name, "Deamon") == 0) current_message = 14;
                    else if(strcmp(battle_monster[i].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(battle_monster[i].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(battle_monster[i].name, "Snake")==0) current_message = 17;
                    else if(strcmp(battle_monster[i].name, "Undeed") == 0) current_message = 18;
                }
            }
        }
    }
    else {
        battle_monstersshoot();
    }
}
void battle_monstersshoot(){
    for(int i =0 ; i< battle_room_monsters_count; i++){
        if(player.battle_row>= battle_monster[i].row - 1 &&player.battle_row <= battle_monster[i].row + 1
        && player.battle_col >= battle_monster[i].col - 1 && player.battle_col <= battle_monster[i].col
        && battle_monster[i].alive){
            if(strcmp(battle_monster[i].name, "Deamon") == 0) current_message = 19;
            else if(strcmp(battle_monster[i].name, "Giant") == 0) current_message = 20;
            else if(strcmp(battle_monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
            else if(strcmp(battle_monster[i].name, "Snake") == 0) current_message = 22;
            else if(strcmp(battle_monster[i].name, "Undeed") == 0) current_message = 23;
            player.health --;
        }
    }
}
void battle_moveplayer(int newRow, int newCol){
    if(newRow >= 0 && newRow<MAXROW && newCol >= 0 && newCol < MAXCOL &&
    battle_map[newRow][newCol] == FLOOR){
        player.battle_row = newRow;
        player.battle_col = newCol;
    }
    for(int i = 0; i <battle_room_monsters_count; i++){
        if(player.battle_row >= battle_monster[i].row - 1&&  player.battle_row <= battle_monster[i].row + 1
        && player.battle_col >= battle_monster[i].col - 1 && player.battle_col <= battle_monster[i].col + 1
        && battle_monster[i].alive){
        if(battle_monster[i].moves < battle_monster[i].max_moves) battle_monster[i].moving = 1;
        if(strcmp(battle_monster[i].name, "Deamon")==0) current_message =19;
        else if(strcmp(battle_monster[i].name, "Giant") == 0) current_message = 20;
        else if(strcmp(battle_monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
        else if(strcmp(battle_monster[i].name, "Snake") == 0) current_message = 22;
        else if(strcmp(battle_monster[i].name, "Undeed") == 0) current_message = 23;
        player.health -- ;
        }
    }
}
void battle_movemonsters(){
    for(int i =0 ; i< battle_room_monsters_count; i++){
        if(battle_monster[i].moving && battle_monster[i].alive){
            if(player.battle_row >= battle_monster[i].row && battle_map[battle_monster[i].row + 1][battle_monster[i].col] == FLOOR){
                if(player.battle_row != battle_monster[i].row + 1 && player.battle_col != battle_monster[i].col) battle_monster[i].row++;
                else {
                    if(strcmp(battle_monster[i].name, "Deamon") == 0) current_message = 19;
                    else if(strcmp(battle_monster[i].name, "Giant") == 0) current_message = 20;
                    else if(strcmp(battle_monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
                    else if(strcmp(battle_monster[i].name, "Snake") == 0) current_message = 22;
                    else if(strcmp(battle_monster[i].name, "Undeed") == 0) current_message = 23;
                    player.health --;
                }
                battle_monster[i].moves ++;
            }
            else if(player.battle_row<=battle_monster[i].row && battle_map[battle_monster[i].row - 1][battle_monster[i].col] == FLOOR){
                if(player.battle_row != battle_monster[i].row - 1 && player.battle_col!= battle_monster[i].col) battle_monster[i].row --;
                else{
                    if(strcmp(battle_monster[i].name, "Deamon") == 0) current_message = 19;
                    else if(strcmp(battle_monster[i].name, "Giant") == 0) current_message = 20;
                    else if(strcmp(battle_monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
                    else if(strcmp(battle_monster[i].name, "Snake") == 0) current_message = 22;
                    else if(strcmp(battle_monster[i].name, "Undeed") == 0) current_message = 23;
                    player.health --;
                }
                battle_monster[i].moves ++;
            }
            else if(player.battle_col <= battle_monster[i].col && battle_map[battle_monster[i].row][battle_monster[i].col -1] == FLOOR){
                if(player.battle_row != battle_monster[i].row && player.battle_col != battle_monster[i].col -1) battle_monster[i].col --;
                else{
                    if(strcmp(battle_monster[i].name, "Deamon") == 0) current_message = 19;
                    else if(strcmp(battle_monster[i].name, "Giant") == 0) current_message = 20;
                    else if(strcmp(battle_monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
                    else if(strcmp(battle_monster[i].name, "Snake") == 0) current_message = 22;
                    else if(strcmp(battle_monster[i].name, "Undeed") == 0) current_message = 23;
                    player.health --;
                }
                battle_monster[i].moves ++;
            }
            else if(player.battle_col >= battle_monster[i].col && battle_map[battle_monster[i].row][battle_monster[i].col + 1] == FLOOR){
                if(player.battle_row != battle_monster[i].row && player.battle_col != battle_monster[i].col+1) battle_monster[i].col++;
                else{
                    if(strcmp(battle_monster[i].name, "Deamon") == 0) current_message = 19;
                    else if(strcmp(battle_monster[i].name, "Giant") == 0) current_message = 20;
                    else if(strcmp(battle_monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
                    else if(strcmp(battle_monster[i].name, "Snake") == 0) current_message = 22;
                    else if(strcmp(battle_monster[i].name, "Undeed") == 0) current_message = 23;
                    player.health --;
                }
                battle_monster[i].moves ++;
            }
            if(battle_monster[i].moves >= battle_monster[i].max_moves){
                battle_monster[i].moving= 0;
            }
        }
    }
}
void printbattleMap(){
    wclear(winb);
    box(winb, 0, 0);
    for(int i =0; i<20; i++){
        for(int j = 0; j< 40; j++){
            switch(battle_map[i][j]){
                case WALLV:
                mvwaddch(winb, i+5, j+ 10, '|');
                break;
                case WALLH:
                mvwaddch(winb, i+5, j+10, '_');
                break;
                case WALLNO:
                mvwaddch(winb, i+5, j+10, ' ');
                break;
                case FLOOR:
                mvwaddch(winb, i+5, j+10, '.');
                break;
                default: 
                mvwaddch(winb, i+5, j+10, ' ');
                break;
            }
        }
    }
    if(strcmp(character_color, "Blue")==0) {
        wattron(winb, COLOR_PAIR(4));
        mvwaddch(winb, player.battle_row + 5, player.battle_col + 10, 'P');
        wattroff(winb, COLOR_PAIR(4));
    }
    else if(strcmp(character_color, "Magenta")==0){
        wattron(winb, COLOR_PAIR(3));
        mvwaddch(winb, player.battle_row + 5, player.battle_col+ 10, 'P');
        wattroff(winb, COLOR_PAIR(3));
    }
    else if(strcmp(character_color, "Green")==0){
        wattron(winb, COLOR_PAIR(2));
        mvwaddch(winb, player.battle_row + 5, player.battle_col + 10, 'P');
        wattroff(winb, COLOR_PAIR(2));
    }
    else {
    mvwaddch(winb, player.battle_row + 5, player.battle_col + 10, 'P'); 
    }
    wattron(winb, COLOR_PAIR(1));
    for(int i = 0; i < battle_room_monsters_count; i++){
        if(strcmp(battle_monster[i].name, "Deamon") == 0 && battle_monster[i].alive) mvwaddch(winb, battle_monster[i].row+5, battle_monster[i].col +10, 'D');
        else if(strcmp(battle_monster[i].name, "Fire Breathing Monster") == 0 && battle_monster[i].alive) mvwaddch(winb, battle_monster[i].row + 5, battle_monster[i].col + 10, 'F');
        else if(strcmp(battle_monster[i].name, "Giant") == 0 && battle_monster[i].alive) mvwaddch(winb, battle_monster[i].row +5, battle_monster[i].col + 10, 'G');
        else if(strcmp(battle_monster[i].name, "Snake") == 0 && battle_monster[i].alive) mvwaddch(winb, battle_monster[i].row + 5, battle_monster[i].col+ 10, 'S');
        else if(strcmp(battle_monster[i].name, "Undeed") ==0 && battle_monster[i].alive) mvwaddch(winb, battle_monster[i].row + 5, battle_monster[i].col+ 10, 'U');
    }
    wattroff(winb, COLOR_PAIR(1));
    wrefresh(winb);
}
void placestaircase(int row, int col, int roomRows, int roomCols){
    int rown = (rand() % (roomRows - 2)) + row + 1;
    int coln = (rand() % (roomCols - 2)) + col + 1;
    if(game_map[rown][coln] == FLOOR) {
    game_map[rown][coln] = STAIRCASE;
    }
    else{
        placestaircase(row, col, roomRows, roomCols);
    }
}
void placepassword(int row, int col, int roomRows, int roomCols){
    int rown = (rand() % (roomRows -2)) + row +1;
    int coln = rand() % (roomCols-2) + col +1;
    if(game_map[rown][coln] == FLOOR){
        game_map[rown][coln] = PASSWORD;
    }
    else placepassword(row, col, roomRows, roomCols);
}
void generatedoorpass() {
    pass[0] = (rand() % 9) + '0'; 
    pass[1] = (rand() % 9) + '0';
    pass[2] = (rand() % 9) + '0';
    pass[3] = (rand() % 9) + '0';
    pass[4] = '\0'; 
}
void removepassword(int row, int col, int roomRows, int roomCols){
    int found = 0;
for (int i = row; i <= row + roomRows && !found; i++) {
    for (int j = col; j <= col + roomCols; j++) {
        if (game_map[i][j] == PASSWORD) {
            game_map[i][j] = FLOOR;
            found = 1;
            break; 
        }
    }
}

}
void placeancientkey(int row, int col, int roomRows, int roomCols){
    int rown = (rand() %(roomRows-2)) + row +1;
    int coln = (rand() % (roomCols-2)) + col +1;
    if(game_map[rown][coln] == FLOOR) game_map[rown][coln] = ANCIENTKEY;
    else{
        placeancientkey(row, col, roomRows, roomCols);
    }
}
void placepotions(int row, int col, int roomRows, int roomCols){
 int numberofpotions = rand() %10 ;
 while (numberofpotions--)
 {
    int rown = (rand()% (roomRows-2)) + row + 1;
    int coln = (rand()% (roomCols-2)) + col + 1;
    if(game_map[rown][coln] == FLOOR){
        int random = rand() %3 ;
        if(random== 0){
            game_map[rown][coln] = SPEED_POTION;
        }
        if(random == 1){
            game_map[rown][coln] = DAMAGE_POTION;
        }
        if(random ==2){
            game_map[rown][coln] = HEALTH_POTION;
        }
    }
 }
 for(int i = row; i<= row + roomRows; i++){
    for(int j = col; j<= col + roomCols; j++){
        type[i][j] = 2;
    }
 }
}
void make_it_treasure(int row, int col, int roomRows, int roomCols){
    for (int i=row; i<=row+roomRows; i++){
        for(int j= col; j<=col+roomCols; j++){
            type[i][j] = 3;
        }
    }
    trapsfortreasure(row, col, roomRows, roomCols);
    endpoint(row, col, roomRows, roomCols);
}
void makeitnightmare(int row, int col, int roomRows, int roomCols){
    for(int i =row; i<= row+roomRows; i++){
        for(int j = col; j<=col+roomCols; j++){
            type[i][j] = 1;
        }
    }
}
void trapsfortreasure(int row, int col, int roomRows, int roomCols){
    int retries = 5;
    while (retries--)
    {
        int rown = (rand()% (roomRows-2)) + row + 1;
        int coln = (rand()% (roomCols-2)) + col + 1;
        if(game_map[rown][coln] == FLOOR) game_map[rown][coln] = TRAP;
    }
    
}
void endpoint(int row, int col, int roomRows, int roomCols){
    int rown = (rand()%(roomRows-2)) + row + 1;
    int coln = (rand() % (roomCols-2)) + col + 1;
    if(game_map[rown][coln] == FLOOR || game_map[rown][coln] == TRAP ) game_map[rown][coln] = END;
    else endpoint(row, col, roomRows, roomCols);
}
void placenormalpotions(int row, int col, int roomRows, int roomCols){
    int count = rand() % 5;
    while (count --)
    {
    int rown = (rand()% (roomRows-2)) + row + 1;
    int coln = (rand()% (roomCols-2)) + col +1;
    int random = rand()%3 +1 ;
    if(game_map[rown][coln]== FLOOR && random == 1) game_map[rown][coln] = HEALTH_POTION;
    else if(game_map[rown][coln]== FLOOR && random == 2) game_map[rown][coln] = DAMAGE_POTION;
    else if(game_map[rown][coln]== FLOOR && random ==3) game_map[rown][coln] = SPEED_POTION;
    }
    

}
void placegold(int row, int col, int roomRows, int roomCols){
    int count = rand() % 10;
    if(type[row][col] == 3) count += rand()%10;
    while(count -- ){
        int rown = (rand() % (roomRows-2)) + row + 1;
        int coln = (rand()%(roomCols-2)) + col + 1;
        if(game_map[rown][coln] == FLOOR) game_map[rown][coln] = GOLD;
    }
}
void place_blackgold(int row, int col, int roomRows, int roomCols){
    int count = rand() % 3;
    while(count --){
        int rown = (rand() % (roomRows - 2)) + row + 1;
        int coln = (rand() % (roomCols - 2)) + col + 1;
        if(game_map[rown][coln] == FLOOR) game_map[rown][coln] = BLACKGOLD;
    }
}
void placefood(int row, int col, int roomRows, int roomCols){
    int count = rand() % 3 + 1;
    while(count --){
        int rown = (rand() %(roomRows - 2)) + row + 1;
        int coln = (rand() % (roomCols - 2)) + col + 1;
        int random = rand() % 4;
        if(game_map[rown][coln] == FLOOR && random == 0) game_map[rown][coln] = nFOOD;
        else if(game_map[rown][coln] == FLOOR && random == 1) game_map[rown][coln] = fFOOD;
        else if(game_map[rown][coln] == FLOOR && random == 2) game_map[rown][coln] = mFOOD;
        else if(game_map[rown][coln] == FLOOR && random == 3) game_map[rown][coln] = rFOOD;    }
}
void place_weapon(int row, int col, int roomRows, int roomCols){
    int count = rand() % 4;
    while(count --){
        int rown = (rand() % (roomRows - 2)) + row + 1;
        int coln = (rand() % (roomCols - 2)) + col + 1;
        if(game_map[rown][coln] == FLOOR){
            int random = rand() % 4 + 2;
            if(random == 2) game_map[rown][coln] = DAGGER;
            if(random == 3) game_map[rown][coln] = MAGIC_WAND;
            if(random == 4) game_map[rown][coln] = NORMAL_ARROW;
            if(random == 5 && sword_count == 0){
                game_map[rown][coln] = SWORD;
                sword_count = 1;
            }
        }
    }
}
void place_monsters(int row, int col, int roomRows, int roomCols){
    int count = rand() % 3;
    while(count --){
        int which = rand() % 5;
        int rown = (rand() %(roomRows - 2)) + row + 1;
        int coln = (rand()%(roomCols - 2) )+ col + 1;
        if(rown < 0 || rown > MAXROW || coln < 0 || coln > MAXCOL){
            count ++;
            continue;
        }
        if(which == 0){
            if(game_map[rown][coln]!= WALLH &&game_map[rown][coln]!= WALLV && game_map[rown][coln]!= WALLNO && 
            game_map[rown][coln]!= SWALLH && game_map[rown][coln]!= SWALLV &&game_map[rown][coln]!= SWALLNO &&
            game_map[rown][coln]!= DOOR && game_map[rown][coln]!= PASSWORDDOOR){
                monster[monsters_count].row = rown;
                monster[monsters_count].col = coln;
                monster[monsters_count].hits = 0;
                monster[monsters_count].lives = 5;
                monster[monsters_count].max_moves = 0;
                monster[monsters_count].moves = 0;
                monster[monsters_count].alive = 1;
                strcpy(monster[monsters_count].name, "Deamon");
                monster[monsters_count].moving = 0;
                monsters_count ++;
            }
        }
        else if(which == 1){
            if(game_map[rown][coln]!= WALLH &&game_map[rown][coln]!= WALLV && game_map[rown][coln]!= WALLNO && 
            game_map[rown][coln]!= SWALLH && game_map[rown][coln]!= SWALLV &&game_map[rown][coln]!= SWALLNO &&
            game_map[rown][coln]!= DOOR && game_map[rown][coln]!= PASSWORDDOOR){
                monster[monsters_count].row = rown;
                monster[monsters_count].col = coln;
                monster[monsters_count].hits = 0;
                monster[monsters_count].lives = 10;
                monster[monsters_count].max_moves = 0;
                monster[monsters_count].moves = 0;
                monster[monsters_count].alive = 1;
                strcpy(monster[monsters_count].name, "Fire Breathing Monster");
                monster[monsters_count].moving = 0;
                monsters_count ++;
            }
        }
        else if(which == 2){
            if(game_map[rown][coln]!= WALLH &&game_map[rown][coln]!= WALLV && game_map[rown][coln]!= WALLNO && 
            game_map[rown][coln]!= SWALLH && game_map[rown][coln]!= SWALLV &&game_map[rown][coln]!= SWALLNO &&
            game_map[rown][coln]!= DOOR && game_map[rown][coln]!= PASSWORDDOOR){
                monster[monsters_count].row = rown;
                monster[monsters_count].col = coln;
                monster[monsters_count].hits = 0;
                monster[monsters_count].moves = 0;
                monster[monsters_count].max_moves = 5;
                monster[monsters_count].alive = 1;
                strcpy(monster[monsters_count].name, "Giant");
                monster[monsters_count].lives = 15;
                monster[monsters_count].moving = 0;
                monsters_count ++;
            }
        }
        else if(which == 3){
            if(game_map[rown][coln]!= WALLH &&game_map[rown][coln]!= WALLV && game_map[rown][coln]!= WALLNO && 
            game_map[rown][coln]!= SWALLH && game_map[rown][coln]!= SWALLV &&game_map[rown][coln]!= SWALLNO &&
            game_map[rown][coln]!= DOOR && game_map[rown][coln]!= PASSWORDDOOR){
                monster[monsters_count].row = rown;
                monster[monsters_count].col = coln;
                monster[monsters_count].hits = 0;
                monster[monsters_count].alive = 1;
                monster[monsters_count].max_moves = 200;
                monster[monsters_count].moves = 0;
                strcpy(monster[monsters_count].name, "Snake");
                monster[monsters_count].lives = 20;
                monster[monsters_count].moving = 0;
                monsters_count ++;
            }
        }
        else if(which == 4){
            if(game_map[rown][coln]!= WALLH &&game_map[rown][coln]!= WALLV && game_map[rown][coln]!= WALLNO && 
            game_map[rown][coln]!= SWALLH && game_map[rown][coln]!= SWALLV &&game_map[rown][coln]!= SWALLNO &&
            game_map[rown][coln]!= DOOR && game_map[rown][coln]!= PASSWORDDOOR){
                monster[monsters_count].row = rown;
                monster[monsters_count].col = coln;
                monster[monsters_count].hits = 0;
                monster[monsters_count].max_moves = 5;
                monster[monsters_count].moves = 0;
                monster[monsters_count].alive = 1;
                strcpy(monster[monsters_count].name, "Undeed");
                monster[monsters_count].lives = 30;
                monster[monsters_count].moving = 0;
                monsters_count ++;
            }
        }
    }
}
void move_monsters(){
    for(int i =0 ; i<monsters_count; i++){
        if(monster[i].moving && monster[i].alive){
            int start_row = monster[i].row, end_row = monster[i].row, start_col = monster[i].col, end_col = monster[i].col;
            while (start_row > 0 && game_map[start_row - 1][monster[i].col] != EMPTY && game_map[start_row - 1][monster[i].col] != CORRIDOR ) start_row--;
            while (end_row < MAXROW - 1 && game_map[end_row + 1][monster[i].col] != EMPTY && game_map[end_row + 1][monster[i].col] != CORRIDOR ) end_row++;
            while (start_col > 0 && game_map[monster[i].row][start_col - 1] != EMPTY && game_map[monster[i].row][start_col - 1] != CORRIDOR) start_col--;
            while (end_col < MAXCOL - 1 && game_map[monster[i].row][end_col + 1] != EMPTY && game_map[monster[i].row][end_col + 1] != CORRIDOR) end_col++;
            int room_rows = end_row - start_row + 1;
            int room_cols = end_col - start_col + 1;
            if(start_row <= player.row && player.row <= end_row
            && start_col <=player.col && player.col <= end_col){
                    if(player.row >= monster[i].row && game_map[monster[i].row + 1][monster[i].col] != WALLV
                    && game_map[monster[i].row + 1][monster[i].col] != WALLH && game_map[monster[i].row + 1][monster[i].col] != WALLNO &&
                    game_map[monster[i].row + 1][monster[i].col] != DOOR && game_map[monster[i].row + 1][monster[i].col] != PASSWORDDOOR &&
                    game_map[monster[i].row + 1][monster[i].col] != SWALLH && game_map[monster[i].row + 1][monster[i].col] != SWALLV && game_map[monster[i].row + 1][monster[i].col] != SWALLNO
                    && game_map[monster[i].row + 1][monster[i].col] != PILLAR){
                       if(player.row != monster[i].row  + 1 && player.col != monster[i].col) monster[i].row ++;
                       else {
                            if(strcmp(monster[i].name, "Deamon") == 0) current_message = 19;
                            else if(strcmp(monster[i].name, "Giant") == 0) current_message = 20;
                            else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
                            else if(strcmp(monster[i].name, "Snake") == 0) current_message = 22;
                            else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 23;
                            player.health --;
                        }
                        monster[i].moves ++;
                    }
                    else if(player.row <= monster[i].row && game_map[monster[i].row - 1][monster[i].col] != WALLV
                    && game_map[monster[i].row - 1][monster[i].col] != WALLH && game_map[monster[i].row - 1][monster[i].col] != WALLNO &&
                    game_map[monster[i].row - 1][monster[i].col] != DOOR && game_map[monster[i].row - 1][monster[i].col] != PASSWORDDOOR &&
                    game_map[monster[i].row - 1][monster[i].col] != SWALLH && game_map[monster[i].row - 1][monster[i].col] != SWALLV && game_map[monster[i].row - 1][monster[i].col] != SWALLNO
                    && game_map[monster[i].row - 1][monster[i].col] != PILLAR){
                        if(player.row != monster[i].row -1 && player.col != monster[i].col)monster[i].row --;
                        else {
                            if(strcmp(monster[i].name, "Deamon") == 0) current_message = 19;
                            else if(strcmp(monster[i].name, "Giant") == 0) current_message = 20;
                            else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
                            else if(strcmp(monster[i].name, "Snake") == 0) current_message = 22;
                            else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 23;
                            player.health --;
                        }
                        monster[i].moves ++;
                    }
                    
                    else if(player.col >= monster[i].col && game_map[monster[i].row ][monster[i].col + 1] != WALLV
                    && game_map[monster[i].row ][monster[i].col + 1] != WALLH && game_map[monster[i].row][monster[i].col + 1] != WALLNO &&
                    game_map[monster[i].row][monster[i].col + 1] != DOOR && game_map[monster[i].row][monster[i].col + 1] != PASSWORDDOOR &&
                    game_map[monster[i].row][monster[i].col + 1] != SWALLH && game_map[monster[i].row][monster[i].col + 1] != SWALLV && game_map[monster[i].row][monster[i].col+1] != SWALLNO
                    && game_map[monster[i].row][monster[i].col + 1] != PILLAR){
                        if(player.row != monster[i].row && player.col != monster[i].col + 1)monster[i].col ++;
                        else {
                            if(strcmp(monster[i].name, "Deamon") == 0) current_message = 19;
                            else if(strcmp(monster[i].name, "Giant") == 0) current_message = 20;
                            else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
                            else if(strcmp(monster[i].name, "Snake") == 0) current_message = 22;
                            else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 23;
                            player.health --;;
                        }
                        monster[i].moves ++;
                    }
                    else if(player.col <= monster[i].col && game_map[monster[i].row][monster[i].col-1] != WALLV
                    && game_map[monster[i].row][monster[i].col-1] != WALLH && game_map[monster[i].row][monster[i].col-1] != WALLNO &&
                    game_map[monster[i].row][monster[i].col-1] != DOOR && game_map[monster[i].row][monster[i].col-1] != PASSWORDDOOR &&
                    game_map[monster[i].row][monster[i].col-1] != SWALLH && game_map[monster[i].row][monster[i].col-1] != SWALLV && game_map[monster[i].row][monster[i].col-1] != SWALLNO
                    && game_map[monster[i].row][monster[i].col-1] != PILLAR){
                        if(player.row != monster[i].row && player.col != monster[i].col - 1)monster[i].col --;
                        else {
                            if(strcmp(monster[i].name, "Deamon") == 0) current_message = 19;
                            else if(strcmp(monster[i].name, "Giant") == 0) current_message = 20;
                            else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
                            else if(strcmp(monster[i].name, "Snake") == 0) current_message = 22;
                            else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 23;
                            player.health --;
                        }
                        monster[i].moves ++;
                    }
                
            }
            if(monster[i].moves >= monster[i].max_moves){
                monster[i].moving = 0;
            }
        }
    }
}
void throwweapon(){
    if (player.weapon_in_hand == 1) {
    if(rand()%3 != 0){
        for (int i = 0; i < monsters_count && i < 20; i++) {
        if (!monster[i].alive) continue; 
        if (abs(monster[i].row - player.row) <= 1 &&
            abs(monster[i].col - player.col) <= 1) {
            monster[i].hits += 5;
            if(player.power) monster[i].hits += 5;
            if(strcmp(monster[i].name, "Deamon") == 0)current_message =9;
            else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 10;
            else if(strcmp(monster[i].name, "Giant") == 0) current_message = 11;
            else if(strcmp(monster[i].name, "Snake") == 0) current_message = 12;
            else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 13;
            if (monster[i].hits >= monster[i].lives) {
                monster[i].alive = 0;
                monster[i].moves = monster[i].max_moves;
                if(strcmp(monster[i].name, "Deamon") == 0)current_message =14;
                else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 15;
                else if(strcmp(monster[i].name, "Giant") == 0) current_message = 16;
                else if(strcmp(monster[i].name, "Snake") == 0) current_message = 17;
                else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 18;
            }
            refresh();
        }
    }}
    else{
        monster_shot();
    }
    last_shot = 1;
}
    else if(player.weapon_in_hand == 2 && player.dagger > 0){
        int ch = getch();
        if (ch == 'l') {
    int hit = 0;
    for (int i = player.col + 1; i <= player.col + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.dagger --; 
                    monster[j].hits += 12;
                    if(player.power) monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i - 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
    if(!hit){
        game_map[player.row][player.col +5] = dagger_SHOT;
        hit =1;
        player.dagger--;
    }
    last_shot = 21;
}
        if(ch == 'k'){
            int hit = 0;
    for (int i = player.row + 1; i <= player.row + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(player.power) monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLH || game_map[player.row][i] == SWALLH)) {
                game_map[i-1][player.col] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
            if(!hit){
                game_map[player.row + 5][player.col] = dagger_SHOT;
                hit = 1;
                player.dagger --; 
            }
            last_shot = 22;
        }
        if(ch == 'j'){
        
            int hit = 0;
    for (int i = player.row - 1; i >= player.row - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(player.power) monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                     ;
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLH || game_map[player.row][i] == SWALLH)) {
                game_map[i + 1][player.col] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
            if(!hit){
                game_map[player.row - 5][player.col] = dagger_SHOT;
                hit = 1;
                player.dagger -- ;
            }
            last_shot = 23;
        }
        if(ch == 'h'){
            int hit = 0;
    for (int i = player.col - 1; i >= player.col - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    if(player.power) monster[j].hits += 12;
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i + 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
        if(!hit){
            game_map[player.row][player.col - 5] = dagger_SHOT;
            hit =1;
            player.dagger --;
        }
            last_shot = 24;
        }
        if(ch == 'u'){
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col ==  player.col + i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    if(player.power) monster[j].hits += 12;
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;;
                }
                   
                    break; 
                }
            }
            if(game_map[player.row - i][player.col + i] == WALLV || game_map[player.row - i][player.col + i] == WALLH || game_map[player.row - i][player.col + i] == WALLNO
                || game_map[player.row - i][player.col + i] == SWALLV ||game_map[player.row - i][player.col + i] == SWALLH || game_map[player.row - i][player.col + i] == SWALLNO  ){
                game_map[player.row - i + 1][player.col + i - 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
            if(!hit){
                game_map[player.row - 5][player.col - 5] = dagger_SHOT;
                player.dagger --;
            }
            last_shot = 25;
        }
        if(ch == 'y'){
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(player.power) monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if(game_map[player.row - i][player.col - i] == WALLV || game_map[player.row - i][player.col - i] == WALLH || game_map[player.row - i][player.col - i] == WALLNO
                || game_map[player.row - i][player.col - i] == SWALLV ||game_map[player.row - i][player.col - i] == SWALLH || game_map[player.row - i][player.col - i] == SWALLNO  ){
                game_map[player.row - i + 1][player.col - i + 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
            if(!hit){
                game_map[player.row - 5][player.col - 5] = dagger_SHOT;
                player.dagger --;
            }
            last_shot = 26;
        }
        if(ch == 'n'){
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col + i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(player.power) monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if(game_map[player.row + i][player.col + i] == WALLV || game_map[player.row + i][player.col + i] == WALLH || game_map[player.row + i][player.col + i] == WALLNO
                || game_map[player.row + i][player.col + i] == SWALLV ||game_map[player.row + i][player.col + i] == SWALLH || game_map[player.row + i][player.col + i] == SWALLNO  ){
                game_map[player.row + i - 1][player.col + i - 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
        if(!hit){
            game_map[player.row+5][player.col +5] = dagger_SHOT;
            player.dagger -- ;
        }
            last_shot = 27;
        }
        if(ch == 'b'){
            
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    if(player.power) monster[j].hits += 12;
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                     ;
                    break; 
                }
            }
            if(game_map[player.row + i][player.col - i] == WALLV || game_map[player.row + i][player.col - i] == WALLH || game_map[player.row + i][player.col - i] == WALLNO
                || game_map[player.row + i][player.col - i] == SWALLV ||game_map[player.row + i][player.col - i] == SWALLH || game_map[player.row + i][player.col - i] == SWALLNO  ){
                game_map[player.row + i - 1][player.col - i + 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
    if(!hit){
        game_map[player.row+5][player.col - 5] = dagger_SHOT;
        player.dagger --;
    }
    last_shot = 28;
        }
    
        }
    else if(player.weapon_in_hand == 2 && player.dagger <= 0){
        attron(COLOR_PAIR(1));
        mvprintw(1, 2, "You don't have enough daggers.");
        attroff(COLOR_PAIR(1));
        refresh();
        getch();
    }
    else if(player.weapon_in_hand == 3 && player.magic_wand > 0){
        int ch = getch();
        if(ch == 'l'){
            int hit = 0;
        for (int i = player.col + 1; i <= player.col + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                     ;
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i - 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    if(!hit){
        game_map[player.row][player.col + 5] = magic_wand_SHOT;
        player.magic_wand --;
    }
    last_shot = 31;
        }
        if(ch == 'k'){
            int hit = 0;
            for (int i = player.row + 1; i <= player.row + 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                     ;
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[i][player.col] == SWALLV)) {
                game_map[i-1][player.col] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    if(!hit){
        game_map[player.row + 10][player.col] = magic_wand_SHOT;
        player.magic_wand --;
    }
            last_shot = 32;
        }
        if(ch == 'j'){
            int hit = 0;
            for (int i = player.row - 1; i >= player.row - 10; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    monster[j].moving = 0;
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[i][player.col] == SWALLV)) {
                game_map[i+1][player.col] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    if(!hit){
        game_map[player.row -10][player.col] = magic_wand_SHOT;
        player.magic_wand --;
    }
            last_shot = 33;
        }
        if(ch == 'h'){
            int hit = 0;
            for (int i = player.row + 1; i <= player.row + 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                     ;
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[i][player.col] == SWALLV)) {
                game_map[i-1][player.col] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    if(!hit){
        game_map[player.row + 10][player.col] = magic_wand_SHOT;
        player.magic_wand --;
    }
            last_shot = 34;
        }
        if(ch == 'u'){
            int hit = 0;
            for (int i = 1; i <= 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col == player.col + i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[player.row - i][player.col + i] == WALLV || game_map[player.row - i][player.col + i] == WALLH || game_map[player.row - i][player.col + i] == WALLNO
                  || game_map[player.row - i][player.col + i] == SWALLV ||game_map[player.row - i][player.col + i] == SWALLH || game_map[player.row - i][player.col + i] == SWALLNO)) {
                game_map[player.row-i+1][player.col + i - 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
            if(!hit){
                game_map[player.row - 10][player.col + 10] = magic_wand_SHOT;
                player.magic_wand -- ;
            }
            last_shot = 35;
        }
        if(ch == 'y'){
            int hit = 0;
            for (int i = 1; i <= 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col == player.col - i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                     ;
                    break; 
                }
            }
            if (!hit && (game_map[player.row - i][player.col - i] == WALLV || game_map[player.row - i][player.col - i] == WALLH || game_map[player.row - i][player.col + i] == WALLNO
                  || game_map[player.row - i][player.col - i] == SWALLV ||game_map[player.row - i][player.col - i] == SWALLH || game_map[player.row - i][player.col - i] == SWALLNO)) {
                game_map[player.row-i+1][player.col - i + 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
        if(!hit){
            game_map[player.row - 10][player.col - 10] = magic_wand_SHOT;
            player.magic_wand --;
        }
            last_shot = 36;
        }
        if(ch == 'n'){
            int hit = 0;
            for (int i = 1; i <= 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col == player.col + i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                     
                    break; 
                }
            }
            if (!hit && (game_map[player.row + i][player.col + i] == WALLV || game_map[player.row + i][player.col + i] == WALLH || game_map[player.row + i][player.col + i] == WALLNO
                  || game_map[player.row + i][player.col + i] == SWALLV ||game_map[player.row + i][player.col + i] == SWALLH || game_map[player.row + i][player.col + i] == SWALLNO)) {
                game_map[player.row + i - 1][player.col + i - 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
        if(!hit){
            game_map[player.row+10][player.col + 10] = magic_wand_SHOT;
            player.magic_wand --;
        }
            last_shot = 37;
        }
        if(ch == 'b'){
            int hit = 0;
            for (int i = 1; i <= 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col == player.col - i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[player.row + i][player.col - i] == WALLV || game_map[player.row + i][player.col - i] == WALLH || game_map[player.row + i][player.col - i] == WALLNO
                  || game_map[player.row + i][player.col - i] == SWALLV ||game_map[player.row + i][player.col - i] == SWALLH || game_map[player.row + i][player.col - i] == SWALLNO)) {
                game_map[player.row + i - 1][player.col - i + 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    if(!hit){
        game_map[player.row +  10][player.col - 10] = magic_wand_SHOT;
        player.magic_wand --;
    }
            last_shot = 38;
        }
    } 
    else if(player.weapon_in_hand == 3 && player.magic_wand <= 0){
        attron(COLOR_PAIR(1));
        mvprintw(1, 2, "You don't have enough magic wands!");
        attroff(COLOR_PAIR(1));
        refresh();
        getch();
    }
    else if(player.weapon_in_hand == 4 && player.normal_arrow > 0){
        int ch = getch();
        if (ch == 'l') {
    int hit = 0;
    for (int i = player.col + 1; i <= player.col + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i - 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
    if(!hit){
        game_map[player.row][player.col + 5] = magic_wand_SHOT;
        player.magic_wand --;
    }
    last_shot = 41;
}
        if(ch == 'k'){
            int hit = 0;
    for (int i = player.row + 1; i <= player.row + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLH || game_map[player.row][i] == SWALLH)) {
                game_map[i-1][player.col] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
    if(!hit){
        game_map[player.row + 5][player.col] = normal_arrow_SHOT;
        player.normal_arrow --;
    }
            last_shot = 42;
        }
        if(ch == 'j'){
            int hit = 0;
    for (int i = player.row - 1; i >= player.row - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                   if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLH || game_map[i][player.col] == SWALLH)) {
                game_map[i + 1][player.col] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
    if(!hit){
        game_map[player.row - 5][player.col] = normal_arrow_SHOT;
        player.normal_arrow --;
    }
            last_shot = 43;
        }
        if(ch == 'h'){
            int hit = 0;
    for (int i = player.col - 1; i >= player.col - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i + 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
        if(!hit){
            game_map[player.row][player.col - 5] = normal_arrow_SHOT;
            player.normal_arrow--;
            
        }
            last_shot = 44;
        }
        if(ch == 'u'){
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col ==  player.col + i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                   
                    break; 
                }
            }
            if(game_map[player.row - i][player.col + i] == WALLV || game_map[player.row - i][player.col + i] == WALLH || game_map[player.row - i][player.col + i] == WALLNO
                || game_map[player.row - i][player.col + i] == SWALLV ||game_map[player.row - i][player.col + i] == SWALLH || game_map[player.row - i][player.col + i] == SWALLNO  ){
                game_map[player.row - i + 1][player.col + i - 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
    if(!hit){
        game_map[player.row - 5][player.col +5] = normal_arrow_SHOT;
        player.normal_arrow--;
    }
            last_shot = 45;
        }
        if(ch == 'y'){
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if(game_map[player.row - i][player.col - i] == WALLV || game_map[player.row - i][player.col - i] == WALLH || game_map[player.row - i][player.col - i] == WALLNO
                || game_map[player.row - i][player.col - i] == SWALLV ||game_map[player.row - i][player.col - i] == SWALLH || game_map[player.row - i][player.col - i] == SWALLNO  ){
                game_map[player.row - i + 1][player.col - i + 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
        if(!hit){
            game_map[player.row - 5][player.col -5] = normal_arrow_SHOT;
            player.normal_arrow --;
        }
            last_shot = 46;
        }
        if(ch == 'n'){
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col + i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if(game_map[player.row + i][player.col + i] == WALLV || game_map[player.row + i][player.col + i] == WALLH || game_map[player.row + i][player.col + i] == WALLNO
                || game_map[player.row + i][player.col + i] == SWALLV ||game_map[player.row + i][player.col + i] == SWALLH || game_map[player.row + i][player.col + i] == SWALLNO  ){
                game_map[player.row + i - 1][player.col + i - 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
        if(!hit){
            game_map[player.row +5][player.col + 5] = normal_arrow_SHOT;
            player.normal_arrow --;
        }
            last_shot = 47;
        }
        if(ch == 'b'){
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if(game_map[player.row + i][player.col - i] == WALLV || game_map[player.row + i][player.col - i] == WALLH || game_map[player.row + i][player.col - i] == WALLNO
                || game_map[player.row + i][player.col - i] == SWALLV ||game_map[player.row + i][player.col - i] == SWALLH || game_map[player.row + i][player.col - i] == SWALLNO  ){
                game_map[player.row + i - 1][player.col - i + 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
    if(!hit){
        game_map[player.row + 5][player.col - 5] = normal_arrow_SHOT;
        player.normal_arrow --;
    }
    last_shot = 48;
        }
    }
    else if(player.weapon_in_hand == 4 && player.normal_arrow <= 0){
        attron(COLOR_PAIR(1));
        mvprintw(1, 2, "You don't have enough normal arrows!");
        attroff(COLOR_PAIR(1));
        refresh();
        getch();
    }
    if(player.weapon_in_hand == 5){
        if(rand()% 3 != 0){for(int i =0; i< monsters_count; i++){
            if(monster[i].row <= player.row + 1 && monster[i].row >= player.row - 1
            && monster[i].col <= player.col + 1 && monster[i].col >= player.col -1
            && monster[i].alive){
                monster[i].hits += 10;
                if(player.power) monster[i].hits += 10;
                if(strcmp(monster[i].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[i].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[i].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 13;
            }
            if(monster[i].hits >= monster[i].lives && monster[i].alive){
                monster[i].moving = 0;
                monster[i].alive = 0;
                monster[i].moves = monster[i].max_moves;
                if(strcmp(monster[i].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[i].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[i].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 18;
                refresh();
            }
        }}
        else{
            monster_shot();
        }
    }
}
void lastshot(int last_shot){
    if(last_shot == 1){
        for (int i = 0; i < monsters_count && i < 20; i++) {
        if (!monster[i].alive) continue; 
        if (abs(monster[i].row - player.row) <= 1 &&
            abs(monster[i].col - player.col) <= 1) {
            monster[i].hits += 5;
            if(strcmp(monster[i].name, "Deamon") == 0)current_message =9;
            else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 10;
            else if(strcmp(monster[i].name, "Giant") == 0) current_message = 11;
            else if(strcmp(monster[i].name, "Snake") == 0) current_message = 12;
            else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 13;
            if (monster[i].hits >= monster[i].lives) {
                monster[i].alive = 0;
                monster[i].moves = monster[i].max_moves;
                if(strcmp(monster[i].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[i].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[i].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 18;
            }
        }
    }
    }
    else if(last_shot == 21 && player.dagger > 0){
        int hit = 0;
    for (int i = player.col + 1; i <= player.col + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.dagger --; 
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i - 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
    last_shot = 21;
    }
    else if(last_shot == 22 && player.dagger > 0){
        int hit = 0;
    for (int i = player.row + 1; i <= player.row + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(player.power) monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLH || game_map[player.row][i] == SWALLH)) {
                game_map[i-1][player.col] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
    }
    else if(last_shot == 23 && player.dagger > 0){
        int hit = 0;
    for (int i = player.row - 1; i >= player.row - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLH || game_map[player.row][i] == SWALLH)) {
                game_map[i + 1][player.col] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
            last_shot = 23;    
        }
    else if(last_shot == 24 && player.dagger > 0){
        int hit = 0;
    for (int i = player.col - 1; i >= player.row - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i + 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
            last_shot = 24;
    }
    else if(last_shot == 25 && player.dagger > 0){
        int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col ==  player.col + i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if(game_map[player.row - i][player.col + i] == WALLV || game_map[player.row - i][player.col + i] == WALLH || game_map[player.row - i][player.col + i] == WALLNO
                || game_map[player.row - i][player.col + i] == SWALLV ||game_map[player.row - i][player.col + i] == SWALLH || game_map[player.row - i][player.col + i] == SWALLNO  ){
                game_map[player.row - i + 1][player.col + i - 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
            last_shot = 25;
    }
    else if(last_shot == 26 && player.dagger > 0){
        int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if(game_map[player.row - i][player.col - i] == WALLV || game_map[player.row - i][player.col - i] == WALLH || game_map[player.row - i][player.col - i] == WALLNO
                || game_map[player.row - i][player.col - i] == SWALLV ||game_map[player.row - i][player.col - i] == SWALLH || game_map[player.row - i][player.col - i] == SWALLNO  ){
                game_map[player.row - i + 1][player.col - i + 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
            last_shot = 26;
    }
    else if(last_shot == 27 && player.dagger > 0){
        int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col + i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if(game_map[player.row + i][player.col + i] == WALLV || game_map[player.row + i][player.col + i] == WALLH || game_map[player.row + i][player.col + i] == WALLNO
                || game_map[player.row + i][player.col + i] == SWALLV ||game_map[player.row + i][player.col + i] == SWALLH || game_map[player.row + i][player.col + i] == SWALLNO  ){
                game_map[player.row + i - 1][player.col + i - 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
            last_shot = 27;
    }
    else if(last_shot == 28 && player.dagger > 0){
        int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if(game_map[player.row + i][player.col - i] == WALLV || game_map[player.row + i][player.col - i] == WALLH || game_map[player.row + i][player.col - i] == WALLNO
                || game_map[player.row + i][player.col - i] == SWALLV ||game_map[player.row + i][player.col - i] == SWALLH || game_map[player.row + i][player.col - i] == SWALLNO  ){
                game_map[player.row + i - 1][player.col - i + 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
    }
    last_shot = 28;
    }
    else if(last_shot >= 21 && last_shot <=28 && player.dagger <=0){
        attron(COLOR_PAIR(1));
        mvprintw(1, 2, "You don't have enough daggers.");
        attroff(COLOR_PAIR(1));
        refresh();
        getch();
    }
    else if(last_shot == 31 && player.magic_wand > 0){
        int hit = 0;
        for (int i = player.col + 1; i <= player.col + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i - 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    last_shot = 31;
    }
    else if(last_shot == 32 && player.magic_wand > 0){
        int hit = 0;
            for (int i = player.row + 1; i <= player.row + 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[i][player.col] == SWALLV)) {
                game_map[i-1][player.col] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    }
    else if(last_shot == 33 && player.magic_wand > 0){
        int hit = 0;
            for (int i = player.row - 1; i >= player.row - 10; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[i][player.col] == SWALLV)) {
                game_map[i+1][player.col] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    }
    else if(last_shot == 34 && player.magic_wand > 0){
        int hit = 0;
            for (int i = player.row + 1; i <= player.row + 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[i][player.col] == SWALLV)) {
                game_map[i-1][player.col] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    }
    else if(last_shot == 35 && player.magic_wand > 0){
        int hit = 0;
            for (int i = 1; i <= 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col == player.col + i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[player.row - i][player.col + i] == WALLV || game_map[player.row - i][player.col + i] == WALLH || game_map[player.row - i][player.col + i] == WALLNO
                  || game_map[player.row - i][player.col + i] == SWALLV ||game_map[player.row - i][player.col + i] == SWALLH || game_map[player.row - i][player.col + i] == SWALLNO)) {
                game_map[player.row-i+1][player.col + i - 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    }
    else if(last_shot == 36 && player.magic_wand > 0){
        int hit = 0;
            for (int i = 1; i <= 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col == player.col - i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[player.row - i][player.col - i] == WALLV || game_map[player.row - i][player.col - i] == WALLH || game_map[player.row - i][player.col + i] == WALLNO
                  || game_map[player.row - i][player.col - i] == SWALLV ||game_map[player.row - i][player.col - i] == SWALLH || game_map[player.row - i][player.col - i] == SWALLNO)) {
                game_map[player.row-i+1][player.col - i + 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    }
    else if(last_shot == 37 && player.magic_wand > 0){
        int hit = 0;
            for (int i = 1; i <= 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col == player.col + i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[player.row + i][player.col + i] == WALLV || game_map[player.row + i][player.col + i] == WALLH || game_map[player.row + i][player.col + i] == WALLNO
                  || game_map[player.row + i][player.col + i] == SWALLV ||game_map[player.row + i][player.col + i] == SWALLH || game_map[player.row + i][player.col + i] == SWALLNO)) {
                game_map[player.row + i - 1][player.col + i - 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    }
    else if(last_shot == 38 && player.magic_wand > 0){
        int hit = 0;
            for (int i = 1; i <= 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col == player.col - i && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    monster[j].moving = 0;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[player.row + i][player.col - i] == WALLV || game_map[player.row + i][player.col - i] == WALLH || game_map[player.row + i][player.col - i] == WALLNO
                  || game_map[player.row + i][player.col - i] == SWALLV ||game_map[player.row + i][player.col - i] == SWALLH || game_map[player.row + i][player.col - i] == SWALLNO)) {
                game_map[player.row + i - 1][player.col - i + 1] = magic_wand_SHOT;
                hit = 1;
                player.magic_wand--; 
            }
        }
    }
    }
    else if(last_shot >= 31 && last_shot <= 28 && player.magic_wand > 0){
        attron(COLOR_PAIR(1));
        mvprintw(1, 2, "You don't have enough magic wands!");
        attroff(COLOR_PAIR(1));
        refresh();
        getch();
    }
    else if(last_shot == 41 && player.normal_arrow > 0){
         int hit = 0;
    for (int i = player.col + 1; i <= player.col + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i - 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }

    }
    else if(last_shot == 42 && player.normal_arrow > 0){
        int hit = 0;
    for (int i = player.row + 1; i <= player.row + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLH || game_map[player.row][i] == SWALLH)) {
                game_map[i-1][player.col] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
            last_shot = 42;
    }
    else if(last_shot == 43 && player.normal_arrow > 0){
        int hit = 0;
    for (int i = player.row - 1; i >= player.row - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLH || game_map[player.row][i] == SWALLH)) {
                game_map[i + 1][player.col] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
            last_shot = 43;
    }
    else if(last_shot == 44 && player.normal_arrow > 0){
        int hit = 0;
    for (int i = player.col - 1; i >= player.col - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if (!hit && (game_map[player.row][i] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i - 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
            last_shot = 44;
    }
    else if(last_shot == 45 && player.normal_arrow > 0){
        int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col ==  player.col + i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if(game_map[player.row - i][player.col + i] == WALLV || game_map[player.row - i][player.col + i] == WALLH || game_map[player.row - i][player.col + i] == WALLNO
                || game_map[player.row - i][player.col + i] == SWALLV ||game_map[player.row - i][player.col + i] == SWALLH || game_map[player.row - i][player.col + i] == SWALLNO  ){
                game_map[player.row - i + 1][player.col + i - 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
        if(!hit){
            game_map[player.row - 5][player.col +5] = normal_arrow_SHOT;
            hit= 1;
            player.normal_arrow--;
        }
            last_shot = 45;
    }
    else if(last_shot == 46 && player.normal_arrow > 0){
        int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row - i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if(game_map[player.row - i][player.col - i] == WALLV || game_map[player.row - i][player.col - i] == WALLH || game_map[player.row - i][player.col - i] == WALLNO
                || game_map[player.row - i][player.col - i] == SWALLV ||game_map[player.row - i][player.col - i] == SWALLH || game_map[player.row - i][player.col - i] == SWALLNO  ){
                game_map[player.row - i + 1][player.col - i + 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
            last_shot = 46;
    }
    else if(last_shot == 47 && player.normal_arrow > 0){
        int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col + i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if(game_map[player.row + i][player.col + i] == WALLV || game_map[player.row + i][player.col + i] == WALLH || game_map[player.row + i][player.col + i] == WALLNO
                || game_map[player.row + i][player.col + i] == SWALLV ||game_map[player.row + i][player.col + i] == SWALLH || game_map[player.row + i][player.col + i] == SWALLNO  ){
                game_map[player.row + i - 1][player.col + i - 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
            last_shot = 47;
    }
    else if(last_shot == 48 && player.normal_arrow > 0){
        int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 13;
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    if(strcmp(monster[j].name, "Deamon") == 0)current_message =14;
                    else if(strcmp(monster[j].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[j].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[j].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[j].name, "Undeed") == 0) current_message = 18;
                }
                    break; 
                }
            }
            if(game_map[player.row + i][player.col - i] == WALLV || game_map[player.row + i][player.col - i] == WALLH || game_map[player.row + i][player.col - i] == WALLNO
                || game_map[player.row + i][player.col - i] == SWALLV ||game_map[player.row + i][player.col - i] == SWALLH || game_map[player.row + i][player.col - i] == SWALLNO  ){
                game_map[player.row + i - 1][player.col - i + 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
    }
    last_shot = 48;
    }
    else if(last_shot >= 41 && last_shot <= 48 && player.normal_arrow <= 0){
        attron(COLOR_PAIR(1));
        mvprintw(1, 2, "                                                                                                     ");
        mvprintw(1, 2, "You don't have enough normal arrows!");
        attroff(COLOR_PAIR(1));
        refresh();
        getch();
    }
    if(last_shot == 5){
        for(int i =0; i< monsters_count; i++){
            if(monster[i].row <= player.row + 1 && monster[i].row >= player.row - 1
            && monster[i].col <= player.col + 1 && monster[i].col >= player.col -1
            && monster[i].alive){
                monster[i].hits += 5;
                if(strcmp(monster[i].name, "Deamon") == 0)current_message =9;
                    else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 10;
                    else if(strcmp(monster[i].name, "Giant") == 0) current_message = 11;
                    else if(strcmp(monster[i].name, "Snake") == 0) current_message = 12;
                    else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 13;
                refresh(); 
            }
            if(monster[i].hits >= monster[i].lives && monster[i].alive){
                monster[i].moving = 0;
                monster[i].alive = 0;
                monster[i].moves = monster[i].max_moves;
                if(strcmp(monster[i].name, "deamon") == 0)current_message =14;
                    else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 15;
                    else if(strcmp(monster[i].name, "Giant") == 0) current_message = 16;
                    else if(strcmp(monster[i].name, "Snake") == 0) current_message = 17;
                    else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 18;
            }
        }
    }
}
void makeitenchant(int row, int col, int roomRows, int roomCols){
    for (int i=row; i<=row+roomRows; i++){
        for(int j= col; j<=col+roomCols; j++){
            type[i][j] = 2;
        }
    }
}
void print_message(int message){
        mvprintw(1, 2, "                                                                                                                                  ");
        if(message == 0){
            int random = rand() % 5;
            if(random == 0) mvprintw(1,2, "Your legs feel heavy again. The speed... was that just a dream?");
            else if(random == 1) mvprintw(1, 2, "You feel like you're walking through mud again. Guess the speed fairy went on vacation.");
            else if(random == 2) mvprintw(1, 2, "The world slows back to its normal pace. You weren't really built for running anyway.");
            else if(random == 3) mvprintw(1, 2, "Your legs stop moving at hyper-speed. Now you're just a regular adventurer again.");
            else if(random == 4) mvprintw(1, 2, "The boost is gone. You briefly think you can outrun a snake. Spoiler: you can't.");
        }
        else if(message == 1){
            int random = rand() % 5;
            if(random == 0) mvprintw(1, 2, "Your muscles are back to their regular, unimpressive size. Guess you're just a mortal after all.");
            else if(random == 1) mvprintw(1, 2, "The surge of power fades. You're no longer a walking wrecking ball.");
            else if(random == 2) mvprintw(1, 2, "The potion's magic wears off, and suddenly, you can't even break a chair with your bare hands");
            else if(random == 3) mvprintw(1, 2, "The adrenaline rush stops. Now you're just trying to not trip over your own feet.");
            else if(random == 4) mvprintw(1, 2, "You feel a little weaker now. Maybe next time, try hitting like you actually mean it.");
        }
        else if( message == 2){
            int random = rand() % 5;
            if(random == 0) mvprintw(1, 2, "Your body is starting to feel... too healthy. it was nice while it lasted");
            else if(random == 1) mvprintw(1, 2, "That glow fades. Now you're just back to being mediocre again.");
            else if(random == 2) mvprintw(1, 2, "The healing fizzles out. You can almost feel you injuries reappearing. Oops");
            else if(random == 3) mvprintw(1, 2, "The potion's effects wear off. You're not worse, just not better anymore.");
            else if(random == 4) mvprintw(1, 2, "The potions effects fade. it's like your body forgot how to be invincible.");
        }
        else if(message == 3){
            int random = rand() % 9;
            mvprintw(3, 58, "     (×_×)☠️  ");
            mvprintw(4, 59, "     /|\\   💀 GAME OVER 💀 ");
            mvprintw(5, 59, "     / \\  ");
            if(random ==0) mvprintw(17, 46, "You died. The enemies take a group photo with your corpse.");
            if(random==1) mvprintw(17,49, "Game Over! The enemies loot your pockets for snacks.");
            if(random == 2) mvprintw(17, 49, "Your adventure ends... but at least you looked cool.");
            if(random == 3) mvprintw(17,46, "You fade into darkness. Somewhere, a deamon laughs at you.");
            if(random == 4) mvprintw(17, 50, "Game over! Your last words? \"This was a bad idea\"");
            if(random == 5) mvprintw(17, 46, "Your adventure ends. The undeed is now wearing your boots.");
            if(random == 6) mvprintw(17, 48, "You collapse. The snake slithers away with your wallet.");
            if(random == 7) mvprintw(17, 34, "You have been deleted from existence. Press any key to pretend that didn't happen.");
            if(random == 8) mvprintw(17, 48, "Congratulations! You're now part of the dungeon decor.");
            mvprintw(23, 41, "                                .");
            mvprintw(24, 41, "                               -|-");
            mvprintw(25, 41, "                                |");
            mvprintw(26, 41, "                            .-'~~~`-.");
            mvprintw(27, 41, "                          .'         `.");
            mvprintw(28, 41, "                          |  R  I  P  |");
            mvprintw(29, 41, "                          |           |");
            mvprintw(30, 41, "                          |           |");
            mvprintw(31, 41, "                        \\\\|           |//");
            mvprintw(32, 41, "   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        }
        else if(message == 4) {
            int random = rand() % 14;
            if(random == 0) mvprintw(1, 2, "Welcome, brave soul!\U0001F3F9The dungeon awaits... and so does your fate.");
            if(random==1) mvprintw(1, 2, "You step into the shadows...\U0001F526Let's hope you step out alive.");
            if(random == 2) mvprintw(1, 2, "A sword in your hand, a dungeon before you.\u2694Try not to die too fast.");
            if(random == 3) mvprintw(1, 2, "The monsters are ready. Are you?\U0001F40D\U0001F525");
            if(random == 4) mvprintw(1, 2, "You awaken in a dark place... and something is watching you\U0001F440");
            if(random == 5) mvprintw(1, 2, "Oh great, another hero.\U0001F644 Let's see how long you last.");
            if(random == 6) mvprintw(1, 2, "Welcome to the dungeon!\U0001F3F0 Don't worry the monsters are mostly friendly.");
            if(random == 7) mvprintw(1, 2, "You enter... and immediately regret your life choices.\U0001F480");
            if(random == 8) mvprintw(1, 2, "Congratulation! You've made the worst desicion of your life.\U0001F389");
            if(random == 9) mvprintw(1, 2, "You hear wihspers in the dark...\U0001F56FToo late to turn back now");
            if(random == 10) mvprintw(1, 2, "The dungeon calls your name. Who told it?\U0001F56F");
            if(random == 11) mvprintw(1, 2, "Something unseen stirs in the shadows... Welcome to your nightmare.\U0001F441");
            if(random == 12) mvprintw(1, 2, "The air is thick with danger. Or maybe it's just the smell of rotten food.");
            if(random == 13) mvprintw(1, 2, "A deep voice echoes: \"You should not have come here.\"\U0001F480");
        }
        else if(message == 5){
            int random = rand() % 12;
            if(random == 0) mvprintw(1, 2, "You step forward... deeper into the abyss.\U0001F526");
            if(random == 1) mvprintw(1, 2, "The air grows colder. Something ancient stirs below... \U0001F3F0");
            if(random == 2) mvprintw(1, 2, "With each  step, the shadows creep closer. Be ready.\u2694");
            if(random == 3) mvprintw(1, 2, "The dungeon whispers your name. It knows you're here. \U0001F441");
            if(random == 4) mvprintw(1, 2, "You descend further... and the real nightmare begins.");
            if(random == 5) mvprintw(1, 2, "Down you go! Hope you brought a will.\U0001F4DC");
            if(random == 6) mvprintw(1, 2, "The monsters below heard you coming. They're placing bets.\U0001F4B0");
            if(random == 7) mvprintw(1, 2, "You descend... regretting every life choice that led you here.");
            if(random == 8) mvprintw(1, 2, "The walls pulse as if alive... are they breathing?\U0001F52E");
            if(random == 9) mvprintw(1, 2, "Something watches from the shadows. It does not blink.\U0001F440");
            if(random == 10) mvprintw(1, 2, "The torchlight flickers. For a second, you swear you're not alone.\U0001F56F");
            if(random == 11) mvprintw(1, 2, "A chill runs down your spine. Whatever's waiting below... it's hungry.\U0001F9B4");
        }
        else if(message == 6) {
            if(type[player.row][player.col] == 3){
                int random = rand() % 5;
                if(random == 0) mvprintw(1, 2, "A pile of gold gleams in the dim light... but why is it just sitting there?\U0001F928");
                else if(random == 1) mvprintw(1, 2, "Treasure! or is it a cleverly disguised death trap? only one way to find out.\U0001F6A8\U0001F4B0");
                else if(random == 2) mvprintw(1, 2, "You see loot. You also see ominous spikes on the walls. Hmm.");
                else if(random == 3) mvprintw(1, 2, "Glorious loot glows ahead. You're about to feel very lucky-or very dead.\u2620");
                else if(random == 4) mvprintw(1, 2, "A golden idol sits atop a pedestal. You know how this ends.\U0002F3C6\U0001F525");
            }
            else {
                int random = rand() % 6;
                if(random == 0) mvprintw(1, 2, "You step into the room and immediately notice the clutter. There's a little bit of everything here.\U0001F52E\U0001F5E1");
                else if(random == 1) mvprintw(1, 2, "The room is filled with old crates, scattered papers, and signs of past travelers. What will you find today?\U0001F4DC\U0001F6E1");
                else if(random == 2) mvprintw(1, 2,"A table with strange tools, a half-empty potion bottle, and the scent of something burning.\u2697\U0001F525");
                else if(random == 3) mvprintw(1, 2, "A pile of discarded armor, a glowing gem on the floor, and an oddly placed book.\U0001F4DA");
                else if(random == 4) mvprintw(1, 2 ,"You step through the door and are greeted with a mix of ancient relics and rusted weapon. Something feels off.\U0001F5E1\U0001F56F");
                else if(random == 5) mvprintw(1, 2, "There's a strange energy in the air, mixed with the smell of old food and damp stone.\u26A1\U0001F356");
            }
        }
        else if(message == 7){
            int random = rand() % 4;
            if(random == 0) mvprintw(1, 2, "You scoop up the shimmering gold, feeling richer already.\U0001F4B0");
            if(random == 1) mvprintw(1, 2, "Gold glints in your hands, gleaming like a treasure of the gods.\U0001F4B0");
            if(random == 2) mvprintw(1, 2, "A heavy pouch of gold coins is yours! Time to splurge... or save\U0001F4B0");
            if(random == 3) mvprintw(1, 2, "You pick up the gold and feel the weight of fortune.\U0001F4B0");
        }
        else if(message == 8){
            attron(COLOR_PAIR(2));
            int random = rand() %14;
            if(random == 0) {
                mvprintw(10, 50, "You have conquered the dungeon! Even the undead tip their hats to you.");
                mvprintw(12, 74, "Your Score: %d", player.score);
            }
            if(random == 1){
                mvprintw(10, 58, "You walked in as a warrior... and walked out a legend.");
                mvprintw(12, 74, "Your Score: %d", player.score);
            } 
            if(random == 2){
                mvprintw(10, 48, "You did it! The enemies all leave you a nice \"thanks for the fun!\" note.");
                mvprintw(12, 74, "Your Score: %d", player.score);
            } 
            if(random == 3){
                mvprintw(10, 40, "Victory! The enemies bow down in defeat... but probably still talk trash about you later.");
                mvprintw(12, 74, "Your Score: %d", player.score);
            } 
            if(random == 4){
                mvprintw(10, 65, "You emerge victorious, treasure in hand!");
                mvprintw(12, 74, "Your final score : %d\U0001F3C6", player.score);
            }
            if(random == 5){
                mvprintw(10, 66, "Legends will be told of your triumph!");
                mvprintw(12, 74, "Your final score: %d\U0001F4dC\u2728", player.score);
            }
            if(random == 6){
                mvprintw(10, 64, "The dungeon trembles as you claim victory!");
                mvprintw(12, 74,"Your final score: %d\u2694\U0001F525", player.score);
            }
            if(random == 7){
                mvprintw(10, 68, "You made it out alive... and rich!");
                mvprintw(12, 74, "Your final score :%d\U0001F4B0", player.score);
            }
            if(random == 8){
                mvprintw(10, 63, "Even the monsters whisper your name in fear.");
                mvprintw(12, 74, "Your final score :%d\U0001F451", player.score);
            }
            if(random == 9){
                mvprintw(10, 52, "You crushed the dungeon, left no loot behind, and became a legend.");
                mvprintw(12, 74, "Your final score :%d\U0001F3C5", player.score);
            }
            if(random == 10){
                mvprintw(10, 51, "After all that chaos, was it worth it? Your final score says yes:%d\U0001F3AD", player.score);
            }
            if(random == 11){
                mvprintw(10, 58, "You walk away from the dungeon battered but victorious!");
                mvprintw(12, 74, "Your final score :%d\U0001F6E1", player.score);
            }
            if(random == 12){
                mvprintw(10, 62, "Gold, glory, and victory!");
                mvprintw(12, 64, "Your final score:%d\U0001F37E\U0001F38A", player.score);
            }
            if(random == 13){
                mvprintw(10, 60, "You have conquered the dungeon... but at what cost? oh right, a %d-point victory!\U0001F3F4", player.score);
            }
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(5));
            mvprintw(17, 46, "                                    o");
            mvprintw(18, 46, "                                   $\"\"$o");
            mvprintw(19, 46, "                                  $\"  $$");
            mvprintw(20, 46, "                                   $$$$");
            mvprintw(21, 46, "                                   o \"$o");
            mvprintw(22, 46, "                                  o\"  \"$");
            mvprintw(23, 46, "             oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o");
            mvprintw(24, 46, "o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  \"oo   o o o o");
            mvprintw(25, 46, "\"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $   \"o$$\"    o$$");
            mvprintw(26, 46, "  \"\"o       o  $          $\"       $$$$$       o          $  ooo     o\"\"");
            mvprintw(27, 46, "     \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$   o\"");
            mvprintw(28, 46, "      \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  \"$$$  $");
            mvprintw(29, 46, "        \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \"\"\"      \"\"\" \"");
            mvprintw(30, 46, "         \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$");
            mvprintw(31, 46, "          \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\"\"$$$$");
            mvprintw(32, 46, "           $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"");
            mvprintw(33, 46, "           $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$");
            mvprintw(34, 46, "           $\"                                                  o");
            mvprintw(35, 46, "           $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$$");
            attroff(COLOR_PAIR(5));
        }
        else if(message == 9){
            int random = rand() % 8;
            if(random == 0) mvprintw(1, 2, "Your blade cuts into the deamon, but dark energy pulses from the wound!");
            else if(random == 1) mvprintw(1, 2, "The deamon snarls, its fiery eyes flickering with rage.");
            else if(random == 2) mvprintw(1, 2, "You strike, but the deamon only laughs. 'Is that all?'");
            else if(random == 3) mvprintw(1, 2, "Black blood drips from the deamon’s wound, sizzling on the ground.");
            else if(random == 4) mvprintw(1, 2, "Your attack lands, but the deamon seems to enjoy the pain!");
            else if(random == 5) mvprintw(1, 2, "You stab the deamon! It gasps dramatically. 'How dare you?!'");
            else if(random == 6) mvprintw(1, 2, "You slash at the deamon! It winks at you. Weird.");
            else if(random == 7) mvprintw(1, 2, "Your attack lands! The deamon rolls its eyes. 'Wow. So original.'");
            
        }
        else if(message == 10){
            int random = rand() % 7;
            if(random == 0) mvprintw(1, 2, "Your weapon sizzles as it strikes the monster’s molten skin!");
            else if(random == 2) mvprintw(1, 2, "Flames burst from the wound, nearly burning you in the process!");
            else if(random == 3) mvprintw(1, 2, "You slash through the fire, feeling the heat singe your arms!");
            else if(random == 4) mvprintw(1, 2, "The monster roars, smoke pouring from its burning wounds!");
            else if(random == 5) mvprintw(1, 2, "Your attack lands, but the flames only seem to grow fiercer!");
            else if(random == 6) mvprintw(1, 2, "You slash through flames! Your eyebrows are now… optional.");
            
        }
        else if(message == 11){
            int random = rand() % 5;
            if(random == 0) mvprintw(1,2, "You stab at the giant’s thick hide! It roars in pain.");
            else if(random == 1) mvprintw(1, 2, "Your attack barely makes a dent in the giant’s massive frame!");
            else if(random == 2) mvprintw(1, 2, "The giant grunts as your blade cuts through its flesh.");
            else if(random == 3) mvprintw(1, 2,"You slice at the giant’s leg—it stumbles but doesn’t fall!");
            else if(random == 4) mvprintw(1, 2, "The giant swipes at you angrily, knocking dust into the air.");
            
        }
        else if(message == 12) {
            int random = rand() % 5;
            if(random == 0) mvprintw(1, 2, "The snake hisses in pain as your blade slices through its scales!");
            else if(random == 1) mvprintw(1, 2, "You strike, but the snake slithers back, ready to counter!");
            else if(random == 2) mvprintw(1, 2, "Your attack lands, venom dripping from the wound!");
            else if(random == 3) mvprintw(1, 2, "The snake coils in agony, its tail lashing violently!");
            else if(random == 4) mvprintw(1, 2, "You pierce the snake’s hide—it writhes in pain!");
        
        }
        else if(message == 13){
            int random = rand() % 5;
            if(random == 0) mvprintw(1, 2, "Your weapon cuts through rotting flesh, but the undead keeps moving!");
            else if(random == 1) mvprintw(1, 2, "Bones crack as your attack lands, but the creature fights on.");
            else if(random == 2) mvprintw(1, 2, "The undead lets out a guttural groan, unfazed by the wound!");
            else if(random == 3) mvprintw(1, 2, "You swing, and a piece of decayed flesh falls to the ground!");
            else if(random == 4) mvprintw(1, 2, "Your strike breaks a rib, but the undead doesn’t seem to care.");
            
        }
        else if(message == 14){
            int random = rand() % 5;
            attron(COLOR_PAIR(2));
            if(random == 0) mvprintw(1, 2, "The deamon lets out an unholy scream as its body dissolves into shadows.");
            else if(random == 1) mvprintw(1, 2, "Its laughter fades into silence... but you feel it watching from the void.");
            else if(random == 2) mvprintw(1, 2, "The deamon collapses, whispering, 'I'll see you... again...");
            else if(random == 3) mvprintw(1, 2, "Dark flames consume its form, leaving behind only a scorched mark on the ground.");
            else if(random == 4) mvprintw(1, 2, "It crumbles into ash. Somewhere, another deamon takes notes on your technique.");
            attroff(COLOR_PAIR(2));
        }
        else if(message == 15){
            int random = rand() % 4;
            attron(COLOR_PAIR(2));
            if(random == 0) mvprintw(1, 2, "The monster’s flames flicker and die out, leaving only a pile of smoldering ash.");
            else if(random == 1) mvprintw(1, 2, "A final explosion of fire erupts before the monster collapses into embers.");
            else if(random == 2) mvprintw(1, 2, "Its body cools, hardening into black stone before crumbling to dust.");
            else if(random == 3) mvprintw(1, 2, "The heat in the air fades as the fire-breathing monster lets out a final roar.");
            attroff(COLOR_PAIR(2));
        }
        else if(message == 16){
            int random = rand() % 4;
            attron(COLOR_PAIR(2));
            if(random == 0) mvprintw(1, 2, "The giant crashes to the ground, shaking the dungeon like an earthquake.");
            else if(random == 1) mvprintw(1, 2, "With a final groan, the giant slumps over, defeated.");
            else if(random == 2) mvprintw(1, 2, "Its massive body collapses, kicking up a cloud of dust.");
            else if(random == 3) mvprintw(1, 2, "The dungeon rumbles as the giant’s last breath fades into silence.");
            attroff(COLOR_PAIR(2));
        }
        else if(message == 17){
            int random = rand() % 4;
            attron(COLOR_PAIR(2));
            if(random == 0) mvprintw(1, 2, "The snake’s body twitches one last time before going still.");
            else if(random == 1) mvprintw(1, 2, "It lets out a final hiss before collapsing into a lifeless coil.");
            else if(random == 2) mvprintw(1, 2, "The serpent writhes, then stops moving. Victory!");
            else if(random == 3) mvprintw(1, 2, "Its forked tongue flickers one last time before falling limp.");
            attroff(COLOR_PAIR(2));
        }
        else if(message == 18){
            int random = rand() % 4;
            attron(COLOR_PAIR(2));
            if(random == 0) mvprintw(1, 2, "The undead collapses into a heap of bones and dust.");
            else if(random == 1) mvprintw(1, 2, "Its eyes fade into darkness as it crumbles to the ground.");
            else if(random == 2) mvprintw(1, 2, "A final groan escapes its lips before silence takes over.");
            else if(random == 3) mvprintw(1, 2, "The undead falls apart, finally finding rest… or so you hope.");
            attroff(COLOR_PAIR(2));
        }
        else if(message == 19){
            int random = rand() % 3;
            attron(COLOR_PAIR(1));
            if(random == 0) mvprintw(1, 2, "The daemon grins as it sears your soul!");
            else if(random == 1) mvprintw(1, 2, "You smell something burning… oh wait, it's you.");
            else if(random == 2) mvprintw(1, 2, "The daemon chuckles, ‘You call that dodging?’");
            attroff(COLOR_PAIR(1));
        }
        else if(message == 20){
            int random = rand() % 3;
            attron(COLOR_PAIR(1));
            if(random == 0) mvprintw(1, 2, "The giant’s club slams you like a fly on a window!");
            else if(random == 1) mvprintw(1, 2, "Your bones rattle like dice in the giant’s fist!");
            else if(random == 2) mvprintw(1, 2, "You briefly experience flight before hitting the ground.");
            attroff(COLOR_PAIR(1));
        }
        else if(message == 21){
            int random = rand()% 3;
            attron(COLOR_PAIR(1));
            if(random == 0) mvprintw(1, 2, "You get a free dragon roast! Too bad you’re the main course.");
            else if(random == 1) mvprintw(1, 2, "The monster exhales, and so does your will to live.");
            else if(random == 2) mvprintw(1, 2, "You now understand how toast feels.");
            attroff(COLOR_PAIR(1));
        }
        else if(message == 22){
            int random = rand() % 4;
            attron(COLOR_PAIR(1));
            if(random == 0) mvprintw(1, 2, "You feel a sharp bite—congrats, you’re venomous now!");
            else if(random == 1) mvprintw(1, 2, "The snake hisses, ‘I was aiming for your ego.’");
            else if(random == 2) mvprintw(1, 2, "You just got slapped by something with no arms.");
            else if(random == 3) mvprintw(1, 2, "Poison seeps in… your body hates you now.");
            attroff(COLOR_PAIR(1));
        }
        else if(message == 23){
            int random = rand() % 3;
            attron(COLOR_PAIR(1));
            if(random == 0) mvprintw(1, 2, "A skeletal hand reaches into your soul… and pockets your dignity.");
            else if(random == 1) mvprintw(1, 2, "You get hit with the stench of death… and a sword!");
            else if(random == 2) mvprintw(1, 2, "A bony fist collides with your face. The skeleton cackles.");
            attroff(COLOR_PAIR(2));
        }
        else if(message == 24){
            int random = rand() % 9;
            attron(COLOR_PAIR(2));
            if(random ==0 ) mvprintw(1, 2, "You stagger out, still breathing. A win’s a win! 😮‍💨🩹");
            else if(random == 1) mvprintw(1, 2, "That could’ve gone worse… or better. Let’s not dwell on it. 🤕");
            else if(random == 2) mvprintw(1, 2, "You wipe the sweat off your brow. That was too close! 😓");
            else if(random == 3) mvprintw(1, 2, "You strut out like a legend. Enemies? More like practice dummies. 😎✨");
            else if(random == 4) mvprintw(1, 2, "Well, that was easy. Maybe too easy. Who’s next? 😏⚔️");
            else if(random == 5) mvprintw(1, 2, "Your enemies whisper your name in fear now. Nice. 👑💀");
            else if(random == 6) mvprintw(1, 2, "You limp away, but hey—you didn’t die. Small victories! 😵‍💫");
            else if(random ==7) mvprintw(1, 2, "That wasn’t a battle. That was survival. Never again. 🏃");
            else if(random == 8) mvprintw(1, 2, "If this was a test, you passed… by the skin of your teeth. 😬📉");
        }
        current_message = -1;
        refresh();
}
char generaterandompass(int type){
    if (type == 3) {
        return 'A' + rand() % 26; // Uppercase letter
    } else if (type == 1) {
        return 'a' + rand() % 26; // Lowercase letter
    } else if (type == 2) {
        return '0' + rand() % 10; // Digit
    }
}
void monster_shot(){
    for(int i = 0; i< monsters_count; i++){
        if(player.row >= monster[i].row- 1 && player.row <= monster[i].row +1
        && player.col >= monster[i].col -1 && player.col <= monster[i].col +1
        && monster[i].alive){
           if(strcmp(monster[i].name, "Deamon") == 0) current_message = 19;
            else if(strcmp(monster[i].name, "Giant") == 0) current_message = 20;
            else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0) current_message = 21;
            else if(strcmp(monster[i].name, "Snake") == 0) current_message = 22;
            else if(strcmp(monster[i].name, "Undeed") == 0) current_message = 23;
            player.health --;
        }
}
}
void previous_game(){
    current_floor = 0;
    moves = 0;
    last_shot = 0;
    clear();
    current_message  = 4;
    strcpy(entry.username, current_username);
    while(player.health > 0){
        printMap();
        print_message(current_message);
        mvprintw(35, 10, "Health: %d/%d", player.health, max_health);
        mvprintw(35, 40, "Ancient keys: %d", player.ancientkeys);
        mvprintw(35, 70, "Broken keys: %d", player.brokenkeys);
        mvprintw(35, 100, "Total score: %d", player.score);
        mvprintw(35, 130, "Gold: %d", player.golds);
        
        refresh();

        int ch = getch();
        switch (ch)
        {
        case 'o':
            WINDOW *winc = newwin(20, 60, 5, 10);
            keypad(winc, TRUE);
            box(winc, 0, 0);
            for(int i =0; i< monsters_count; i++){
                mvwprintw(winc, 2 + i, 12, "%d)%s %d %d", i, monster[i].name, monster[i].row, monster[i].col);
            }
            wrefresh(winc);
            getch();
            delwin(winc);
            break;
        case 'y':
            movePlayer(player.row -1, player.col -1);
            if(player.speed) movePlayer(player.row -1, player.col -1);
            move_monsters();
            break;
        case 'u':
            movePlayer(player.row-1, player.col+1);
            if(player.speed) movePlayer(player.row-1, player.col+1);
            move_monsters();
            break;
        case 'h':
            movePlayer(player.row, player.col-1);
            if(player.speed) movePlayer(player.row, player.col-1);
            move_monsters();
            break;
        case 'j':
            movePlayer(player.row-1, player.col);
            if(player.speed) movePlayer(player.row-1, player.col);
            move_monsters();
            break;
        case 'k' :
            movePlayer(player.row+1, player.col);
            if(player.speed) movePlayer(player.row+1, player.col);
            move_monsters();
            break;
        case 'l' :
            movePlayer(player.row, player.col+1);
            if(player.speed) movePlayer(player.row, player.col+1);
            move_monsters();
            break;
        case 'b':
            movePlayer(player.row+1, player.col-1);
            if(player.speed) movePlayer(player.row+1, player.col-1);
            move_monsters();
            break;
        case 'n':
            movePlayer(player.row+1, player.col+1);
            if(player.speed) movePlayer(player.row+1, player.col+1);
            move_monsters();
            break;
        case 'r':
            if(player.brokenkeys >= 2){
                player.ancientkeys ++;
                player.brokenkeys = player.brokenkeys -2;
            }
            break;
        case 'q':
            monsters_count = 0;
            save_map("saved.dat");
            return;
        case 'm':
            printall();
            break;
        case 'p':
            {WINDOW *winp = newwin(14, 76, 5, 10);
            keypad(winp, TRUE);
            box(winp, 0, 0);
            mvwprintw(winp, 1, 2, "-LIST OF POTIONS-");
            mvwprintw(winp, 3, 2, "You have %d health potion(s).", player.hps);
            mvwprintw(winp, 5,2, "You have %d speed potion(s).", player.sps);
            mvwprintw(winp, 7, 2, "You have %d damage potion(s).", player.dps);
            mvwprintw(winp, 9, 2, "Enter the first letter of the potion you want to consume!");
            wrefresh(winp);
            while(1){
                ch = getch();
                if(ch == 'h'){
                    int random = rand() % 4;
                    if(random == 0)mvwprintw(winp, 11, 2, "You chug it down. It tastes awful, but at least you're not dying... yet.");
                    else if(random == 1) mvwprintw(winp, 11, 2, "You feel reinvigorated! The dungeon is slightly less terrifying now.");
                    else if(random == 2) mvwprintw(winp, 11, 2, "Your wounds close up. Science? Magic? Who cares- it works!");
                    else if(random == 3) mvwprintw(winp, 11, 2, "You drink it. Your body appreciates it. Your taste buds do not.");
                    player.healthp = 1;
                    player.speed = 0;
                    player.power = 0;
                    wrefresh(winp);
                    getch();
                    break;
                }
                if(ch == 's'){
                    int random = rand() % 5;
                    if(random == 0) mvwprintw(winp, 11, 2, "Your legs start moving before your brain catches up. Hold on tight!");
                    if(random == 1) mvwprintw(winp, 11, 2, "You suddenly feel the need to run everywhere. Even when standing still.");
                    if(random == 2) mvwprintw(winp, 11, 2, "Your heartbeat accelerates. Either you're faster, or you're having a panic attack.");
                    if(random == 3) mvwprintw(winp, 11, 2, "You blink, and now you're five feet ahead. That was not normal.");
                    if(random == 4) mvwprintw(winp, 11, 2, "Your reflexes sharpen. Your feet demand adventure");
                    player.speed = 1;
                    player.power = 0;
                    player.healthp = 0;
                    wrefresh(winp);
                    getch();
                    break;
                }
                if(ch == 'd'){
                    int random = rand() % 3;
                    if(random == 0) mvwprintw(winp, 11, 2, "The weapon in your hand feels lighter. The enemies look more nervous.");
                    if(random == 1) mvwprintw(winp, 11, 2, "A rush of strength fills you! Your enemies have no idea what's coming");
                    if(random == 2) mvwprintw(winp, 11, 2, "You feel unstoppable. Maybe even too powerful. This should be fun.");
                    player.power  =1;
                    wrefresh(winp);
                    getch();
                    break;
                }
                if(ch == 27 || ch == 'q'){
                    break;
                }
            }
            delwin(winp);
                break;}
        case 'i':{
            WINDOW *winn = newwin(20, 60, 5, 10);
            keypad(winn, TRUE);
            box(winn, 0, 0);
            mvwprintw(winn, 1, 2, "-LIST OF WEAPONS-");
            wattron(winn, COLOR_PAIR(2));
            if(player.weapon_in_hand == 1) mvwprintw(winn, 3, 2, "You have a mace in your hand!");
            else if(player.weapon_in_hand == 2) mvwprintw (winn, 3, 2, "You have daggers in your hand!");
            else if(player.weapon_in_hand == 3) mvwprintw(winn, 3, 2, "You have magic wands in your hand!");
            else if(player.weapon_in_hand == 4) mvwprintw(winn, 3, 2, "You have normal arrows in your hand!");
            else if(player.weapon_in_hand == 5) mvwprintw(winn, 3, 2, "You have a sword in your hand!");
            wattroff(winn, COLOR_PAIR(2));
            if(player.mace != 0)mvwprintw(winn, 5, 2, "(m)%d mace(s)", player.mace);
            if(player.mace == 0)mvwprintw(winn, 5, 2, "You don't have any maces");
            if(player.dagger != 0) mvwprintw(winn, 7, 2, "(d)%d dagger(s) ", player.dagger);
            if(player.dagger == 0) mvwprintw(winn, 7, 2, "You don't have any daggers");
            if(player.magic_wand != 0) mvwprintw(winn, 9, 2, "(g)%d magic wand(s)", player.magic_wand);
            if(player.magic_wand == 0) mvwprintw(winn, 9, 2, "You don't have any magic wands");
            if(player.normal_arrow != 0) mvwprintw(winn, 11, 2, "(n)%d normal arrow(s)", player.normal_arrow);
            if(player.normal_arrow == 0) mvwprintw(winn, 11, 2, "You don't have any normal arrows");
            if(player.sword != 0) mvwprintw(winn, 13, 2, "(s)%d sword(s)", player.sword);
            if(player.sword == 0) mvwprintw(winn, 13, 2, "You don't have any swords");
            wrefresh(winn);
            while(1){
                ch = getch();
                if( ch == 'w'){
                    mvwprintw(winn, 15, 2, "                                                     ");
                    if(player.weapon_in_hand == 1) mvwprintw(winn, 15, 2, "You put your mace in your backpack!");
                    else if(player.weapon_in_hand == 2) mvwprintw (winn, 15, 2, "You put your daggers in your backpack!");
                    else if(player.weapon_in_hand == 3) mvwprintw(winn, 15, 2, "You put your magic_wands in your backpack!");
                    else if(player.weapon_in_hand == 4) mvwprintw(winn, 15, 2, "You put your normal arrows in your backpack!");
                    else if(player.weapon_in_hand == 5) mvwprintw(winn, 15, 2, "You put your sword in your backpack!");
                    else mvwprintw(winn, 13, 2, "You have nothing in your hand");
                    player.weapon_in_hand = 0;
                }
                else if(ch == 'm'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                       ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have a mace in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 1;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(1)); 
                        mvwprintw(winn, 15, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 'd'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have daggers in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 2;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(1));
                        mvwprintw(winn, 15, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 'g'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have magic wands in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 3;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(1));
                        mvwprintw(winn, 15, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 'n'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have normal arrows in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 4;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(1));
                        mvwprintw(winn, 15, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 's'){
                    if(player.weapon_in_hand == 0){
                        mvwprintw(winn, 15, 2, "                                                      ");
                        attron(COLOR_PAIR(2));
                        mvwprintw(winn, 15, 2, "You now have swords in your hand!");
                        attroff(COLOR_PAIR(2));
                        player.weapon_in_hand = 5;
                        wrefresh(winn);
                        getch();
                        break;
                    }
                    else{
                        mvwprintw(winn, 15, 2, "                                            ");
                        attron(COLOR_PAIR(1));
                        mvwprintw(winn, 13, 2, "You must put your current item in your backpack first!");
                        attroff(COLOR_PAIR(1));
                    }
                }
                else if(ch == 'q' || ch == 27) break;
                else {
                    attron(COLOR_PAIR(1));
                    mvwprintw(winn, 15, 2, "                                                      ");
                    mvwprintw(winn, 15, 2, "You don't have any of these.");
                    attroff(COLOR_PAIR(1));
                }
                wrefresh(winn);
            }
            wrefresh(winn);
            delwin(winn);
            break;}
        case 'a':
            lastshot(last_shot);
            break;
        case 'e':
            {WINDOW *win = newwin(14, 50, 5, 10);
            keypad(win, TRUE);
            box(win, 0, 0);
            mvwprintw(win, 1, 2, "-LIST OF FOODS-");
            mvwprintw(win, 3, 2, "hunger : %d", max_health-player.health);
            mvwprintw(win,5, 2, "you have %d foods with you.", player.normal_food + player.fancy_food + player.magical_food + player.rotten_food);
            mvwprintw(win, 7, 2, "do you want to consume food? (y/n)");
            wrefresh(win);
            ch = getch();
            if(ch == 'y') {
                if(player.normal_food + player.fancy_food + player.magical_food +player.rotten_food == 0){
                    wattron(win, COLOR_PAIR(1));
                    mvwprintw(win, 9, 2, "You don't have any food with you");
                    wattroff(win, COLOR_PAIR(1));
                    wrefresh(win);
                }
                else{
                    int not_consumed  = 1;
                    while(not_consumed){
                        int random = rand() % 4;
                        if(random == 0 && player.normal_food > 0){
                            player.normal_food --;
                            not_consumed = 0;
                            if(player.health < max_health) player.health ++;
                            if(player.health < max_health && player.healthp) player.health ++;
                            mvwprintw(win, 9, 2, "You consumed a normal food!");
                            wrefresh(win);
                        }
                        else if(random == 1 && player.fancy_food > 0){
                            player.fancy_food --;
                            not_consumed = 0;
                            if(player.health < max_health) player.health ++;
                            if(player.health < max_health && player.healthp) player.health ++;
                            player.power = 1;
                            wattron(win, COLOR_PAIR(2));
                            mvwprintw(win , 9, 2, "You consumed a fancy food!");
                            wattroff(win, COLOR_PAIR(2));
                            wrefresh(win);
                        }
                        else if(random == 2 && player.magical_food > 0){
                            player.magical_food --;
                            not_consumed = 0;
                            if(player.health < max_health) player.health ++;
                            if(player.health < max_health && player.healthp) player.health ++;
                            player.speed = 1;
                            wattron(win, COLOR_PAIR(2));
                            mvwprintw(win, 9, 2, "You consumed a magical food!");
                            wattroff(win, COLOR_PAIR(2));
                            wrefresh(win);
                        }
                        else if(random == 3 && player.rotten_food > 0){
                            player.rotten_food --;
                            not_consumed = 0;
                            player.health  --;
                            if(player.health < max_health && player.healthp) player.health ++;
                            player.speed = 1;
                            wattron(win, COLOR_PAIR(1));
                            mvwprintw(win, 9, 2, "You consumed a rotten food!");
                            wattroff(win, COLOR_PAIR(1));
                            wrefresh(win);
                        }

                    }
                }
                getch();
                delwin(win);
            }
            else {
                delwin(win);
            }
            break;}
        case 'f':
            ch = getch();
            if(ch == 'y'){
                while(game_map[player.row-1][player.col-1] != WALLH && game_map[player.row-1][player.col-1] != WALLV && game_map[player.row-1][player.col-1] != WALLNO && game_map[player.row-1][player.col-1] != PILLAR && game_map[player.row-1][player.col-1] != END &&
                game_map[player.row-1][player.col-1] != ANCIENTKEY &&game_map[player.row-1][player.col-1] != STAIRCASE && game_map[player.row-1][player.col-1] != REVEALEDTRAP && game_map[player.row-1][player.col-1] != HEALTH_POTION && 
                game_map[player.row-1][player.col-1] != PASSWORDDOOR && game_map[player.row-1][player.col-1] != PASSWORD && game_map[player.row-1][player.col-1] != TRAP && game_map[player.row-1][player.col-1] != SPEED_POTION && game_map[player.row-1][player.col-1] != DAMAGE_POTION ){
                    movePlayer(player.row-1, player.col-1);
                    move_monsters();
                }
            }
            if(ch == 'u'){
                while(game_map[player.row-1][player.col+1] != WALLH && game_map[player.row-1][player.col+1] != WALLV && game_map[player.row-1][player.col+1] != WALLNO && game_map[player.row-1][player.col+1] != PILLAR && game_map[player.row-1][player.col+1] != END &&
                game_map[player.row-1][player.col+1] != ANCIENTKEY &&game_map[player.row-1][player.col+1] != STAIRCASE && game_map[player.row-1][player.col+1] != REVEALEDTRAP && game_map[player.row-1][player.col+1] != HEALTH_POTION && 
                game_map[player.row-1][player.col+1] != PASSWORDDOOR && game_map[player.row-1][player.col+1] != PASSWORD && game_map[player.row-1][player.col+1] != TRAP && game_map[player.row-1][player.col+1] != SPEED_POTION && game_map[player.row-1][player.col+1] != DAMAGE_POTION ){
                    movePlayer(player.row-1, player.col+1);
                    move_monsters();
                }
            }
            if(ch == 'l'){
                while(game_map[player.row][player.col+1] != WALLH && game_map[player.row][player.col+1] != WALLV && game_map[player.row][player.col+1] != WALLNO && game_map[player.row][player.col+1] != PILLAR && game_map[player.row][player.col+1] != END &&
                game_map[player.row][player.col+1] != ANCIENTKEY &&game_map[player.row][player.col+1] != STAIRCASE && game_map[player.row][player.col+1] != REVEALEDTRAP && game_map[player.row][player.col+1] != HEALTH_POTION && 
                game_map[player.row][player.col+1] != PASSWORDDOOR && game_map[player.row][player.col+1] != PASSWORD && game_map[player.row][player.col+1] != TRAP && game_map[player.row][player.col+1] != SPEED_POTION && game_map[player.row][player.col+1] != DAMAGE_POTION ){
                    movePlayer(player.row, player.col+1);
                    move_monsters();
                }
            }
            if(ch == 'j'){
                while(game_map[player.row-1][player.col] != WALLH && game_map[player.row-1][player.col] != WALLV && game_map[player.row-1][player.col] != WALLNO && game_map[player.row-1][player.col] != PILLAR && game_map[player.row-1][player.col] != END &&
                game_map[player.row-1][player.col] != ANCIENTKEY &&game_map[player.row-1][player.col] != STAIRCASE && game_map[player.row-1][player.col] != REVEALEDTRAP && game_map[player.row-1][player.col] != HEALTH_POTION && 
                game_map[player.row-1][player.col] != PASSWORDDOOR && game_map[player.row-1][player.col] != PASSWORD && game_map[player.row-1][player.col] != TRAP && game_map[player.row-1][player.col] != SPEED_POTION && game_map[player.row-1][player.col] != DAMAGE_POTION ){
                    movePlayer(player.row-1, player.col);
                    move_monsters();
                }
            }
            if(ch == 'k'){
                while(game_map[player.row+1][player.col] != WALLH && game_map[player.row+1][player.col] != WALLV && game_map[player.row+1][player.col] != WALLNO && game_map[player.row+1][player.col] != PILLAR && game_map[player.row+1][player.col] != END &&
                game_map[player.row+1][player.col] != ANCIENTKEY &&game_map[player.row+1][player.col] != STAIRCASE && game_map[player.row+1][player.col] != REVEALEDTRAP && game_map[player.row+1][player.col] != HEALTH_POTION && 
                game_map[player.row+1][player.col] != PASSWORDDOOR && game_map[player.row+1][player.col] != PASSWORD && game_map[player.row+1][player.col] != TRAP && game_map[player.row+1][player.col] != SPEED_POTION && game_map[player.row+1][player.col] != DAMAGE_POTION ){
                    movePlayer(player.row+1, player.col);
                    move_monsters();
                }
            }
            if(ch == 'h'){
                while(game_map[player.row][player.col-1] != WALLH && game_map[player.row][player.col-1] != WALLV && game_map[player.row][player.col-1] != WALLNO && game_map[player.row][player.col-1] != PILLAR && game_map[player.row][player.col-1] != END &&
                game_map[player.row][player.col-1] != ANCIENTKEY &&game_map[player.row][player.col-1] != STAIRCASE && game_map[player.row][player.col-1] != REVEALEDTRAP && game_map[player.row][player.col-1] != HEALTH_POTION && 
                game_map[player.row][player.col-1] != PASSWORDDOOR && game_map[player.row][player.col-1] != PASSWORD && game_map[player.row][player.col-1] != TRAP && game_map[player.row][player.col-1] != SPEED_POTION && game_map[player.row][player.col-1] != DAMAGE_POTION ){
                    movePlayer(player.row, player.col-1);
                    move_monsters();
                }
            }
            if(ch == 'b'){
                while(game_map[player.row+1][player.col-1] != WALLH && game_map[player.row+1][player.col-1] != WALLV && game_map[player.row+1][player.col-1] != WALLNO && game_map[player.row+1][player.col-1] != PILLAR && game_map[player.row+1][player.col-1] != END &&
                game_map[player.row+1][player.col-1] != ANCIENTKEY &&game_map[player.row+1][player.col-1] != STAIRCASE && game_map[player.row+1][player.col-1] != REVEALEDTRAP && game_map[player.row+1][player.col-1] != HEALTH_POTION && 
                game_map[player.row+1][player.col-1] != PASSWORDDOOR && game_map[player.row+1][player.col-1] != PASSWORD && game_map[player.row+1][player.col-1] != TRAP && game_map[player.row+1][player.col-1] != SPEED_POTION && game_map[player.row+1][player.col-1] != DAMAGE_POTION ){
                    movePlayer(player.row+1, player.col-1);
                    move_monsters();
                }
            }
            if(ch == 'n'){
                while(game_map[player.row+1][player.col+1] != WALLH && game_map[player.row+1][player.col+1] != WALLV && game_map[player.row+1][player.col+1] != WALLNO && game_map[player.row+1][player.col+1] != PILLAR && game_map[player.row+1][player.col+1] != END &&
                game_map[player.row+1][player.col+1] != ANCIENTKEY &&game_map[player.row+1][player.col+1] != STAIRCASE && game_map[player.row+1][player.col+1] != REVEALEDTRAP && game_map[player.row+1][player.col+1] != HEALTH_POTION && 
                game_map[player.row+1][player.col+1] != PASSWORDDOOR && game_map[player.row+1][player.col+1] != PASSWORD && game_map[player.row+1][player.col+1] != TRAP && game_map[player.row+1][player.col+1] != SPEED_POTION && game_map[player.row+1][player.col+1] != DAMAGE_POTION ){
                    movePlayer(player.row+1, player.col+1);
                    move_monsters();
                }
            }
            break;
        
        case 'g':
        g = 0;
        break;
        case ' ':
         throwweapon();
         break;
        default:
            break;
        }
    }
    clear();
    attron(COLOR_PAIR(1));
    print_message(3);
    attroff(COLOR_PAIR(1));
    refresh();
    napms(5000);
    getch();
    entry.score = 0;
    entry.gold_pieces = 0;
    entry.games_finished = 0;
    monsters_count = 0;
    sword_count = 0;
    pre_game_area();

}