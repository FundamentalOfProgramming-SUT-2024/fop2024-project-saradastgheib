#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>     
//403105941
char current_username[50] = "";
char game_mode[20] = "Easy";
char character_color[20]= "";
#define FILENAME "users.txt"
#define HALLFILE "halloffame.txt"
#define MAXROW 35
#define MAXCOL 150
int max_health;
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
map_elements game_map[MAXROW][MAXCOL];
map_elements previous[MAXROW][MAXCOL];
int visible[MAXROW][MAXCOL];
int unlocked[MAXROW][MAXCOL];
int type[MAXROW][MAXCOL]; // 3 for treasure 2 for enchant 
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
int load_user_data(HallOfFameEntry *entry);
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
int battleroom();
void placestaircase(int row, int col, int roomRows, int roomCols);
void placepassword(int row, int col, int roomRows, int roomCols);
void generatedoorpass();
void removepassword(int row, int col, int roomRows, int roomCols);
void placeancientkey(int row, int col, int roomRows, int roomCols);
void placepotions(int row, int col, int roomRows, int roomCols);
void make_it_treasure(int row, int col, int roomRows, int roomCols);
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
int main() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
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
    WINDOW *win = newwin(12, 50, 5, 10);
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
        mvwprintw(win, 8,12, "                                  ");
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
                    mvwaddch(win, 8, 12+i, ' ');
                    wmove(win, 8, 12+i);
                }
            }
            else if(i<49 && isprint(ch)) {
                email[i++] = ch;
                mvwaddch(win, 8, 12+i-1, ch);
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
    mvwprintw(win, 9, 2, "                    ");
    User user;
    strcpy(user.username, username);
    strcpy(user.password, password);
    strcpy(user.email, email);
    save_user(user);

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
            ch = wgetch(win);
            if(ch == '\n' || ch == KEY_ENTER) {
                password[i]= '\0';
                break;
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
            wrefresh(win);
        }
        password[i] = '\0';
        if(check_password(username, password)){
            strcpy(current_username, username);
            current_username[sizeof(current_username) - 1] = '\0';
            wattron(win, COLOR_PAIR(2));
            mvwprintw(win, 8, 2, "                           ");
            mvwprintw(win, 8, 2, "Login successful");
            wattroff(win, COLOR_PAIR(2));
            wrefresh(win);
            //HallOfFameEntry entry = {0};
            strcpy(entry.username, current_username);
            savehall(entry);
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
                // prevoius game
            }
            else if(choice ==2){
                startplaying();
            }
        }
    }
}
void savehall(const HallOfFameEntry entry) {
    HallOfFameEntry entries[100];
    int count = 0;
    int updated = 0;
    FILE *file = fopen(HALLFILE, "r");
    if (file) {
        while (fscanf(file, "%s %d %d %d %ld",
                      entries[count].username,
                      &entries[count].score,
                      &entries[count].gold_pieces,
                      &entries[count].games_finished,
                      &entries[count].first_game_time) == 5) {
            if (strcmp(entries[count].username, entry.username) == 0) {
                entries[count].score += entry.score;
                entries[count].gold_pieces = entry.gold_pieces;
                entries[count].games_finished += 1;
                updated = 1;
            }
            count++;
        }
        fclose(file);
    }
    if (!updated) {
        if (count < 100) { 
            entries[count++] = entry;
        } else {
            fprintf(stderr, "Error: Too many entries in the hall of fame.\n");
            return;
        }
    }
    file = fopen(HALLFILE, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open file for writing.\n");
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
    int gold = 0;
    current_floor = 0;
    moves = 0;
    last_shot = 0;
    clear();
    mvprintw(1, 2, "This is where messages should appear");
    refresh();
    curs_set(0);
    if(strcmp(game_mode, "Legendary") == 0) max_health = 5;
    else if(strcmp(game_mode, "Insane") == 0) max_health = 10;
    else if(strcmp(game_mode, "Hard") == 0) max_health = 15;
    else if(strcmp(game_mode, "Normal") == 0) max_health = 20;
    else if(strcmp(game_mode , "Easy") == 0) max_health = 25;
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
            return;
        case 'm':
            printall();
            break;
        case 'p':
            WINDOW *winp = newwin(14, 76, 5, 10);
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
                break;
        case 'i':
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
            if(player.mace != 0)mvwprintw(winn, 5, 2, "(m)%d mace(s)  damage: 5  ", player.mace);
            if(player.mace == 0)mvwprintw(winn, 5, 2, "You don't have any maces");
            if(player.dagger != 0) mvwprintw(winn, 7, 2, "(d)%d dagger(s)", player.dagger);
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
            break;
        case 'a':
            lastshot(last_shot);
            break;
        case 'e':
            WINDOW *win = newwin(14, 50, 5, 10);
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
                    getch();
                    delwin(win);
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
            break;
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
        default:
            break;
        }
    }
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(17, 65, "You lost :( Try again?");
    attroff(COLOR_PAIR(1));
    refresh();
    napms(7000);
    entry.score = 0;
    entry.gold_pieces = 0;
    entry.games_finished = 0;
    monsters_count = 0;
    sword_count = 0;
    pre_game_area();
}
int load_user_data(HallOfFameEntry *entry){
    FILE *file = fopen(HALLFILE, "r");
    if(!file) return 0;
    while(fscanf(file, "%s %d %d %d %ld",
    entry->username,
    &entry->score,
    &entry->gold_pieces,
    &entry->games_finished,
    &entry->first_game_time)==5){
        if(strcmp(entry->username, current_username)==0){
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
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
            }
            else if(choice == 3){
                strcpy(game_mode, "Insane");
            }
            else if(choice == 4){
                strcpy(game_mode, "Legendary");
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
    srand(time(NULL));
    initialize_map();
    int roomcount = rand() % 5 + 6;
    int traproom = rand() % roomcount;
    int stairroom = rand() % (roomcount-1) + 1;
    int ancientkeyroom = rand() % roomcount;
    int retry_count = 0;
    int extra_room = 0;
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
        // player.speed = 0;
        // player.power = 0;
        // player.healthp = 0;
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
    if(moves % 100 == 74){
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
        player.speed = 0;
        player.power = 0;
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
                mvprintw(1, 2, "You unlocked the door!");
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
        
        
    if(load_user_data(&entry)){
        player.score += player.golds * (current_floor+1) / 3;
        entry.score = entry.score + player.golds * (current_floor+1) / 3;
        entry.gold_pieces = entry.gold_pieces + player.golds;
    }
    else {
        entry.score = player.golds * (current_floor+1) / 3;
        entry.gold_pieces = player.golds;
        //entry.games_finished = 1;
        entry.first_game_time = time(NULL);
    }
    savehall(entry);
        entry.score = 0;
        entry.gold_pieces = 0;
        monsters_count = 0;
        current_floor++;
        player.golds = 0;
        generate_random_map(start_row, start_col, room_rows-1, room_cols-1);
        makeitvisible(start_row, start_col, room_rows, room_cols);
        player.row = start_row + room_rows / 2;
        player.col = start_col + room_cols / 2;
    }


if(game_map[newRow][newCol] == DOOR || game_map[newRow][newCol]==SWALLH
    || game_map[newRow][newCol]==SWALLV || game_map[newRow][newCol]==SWALLNO
    || (game_map[newRow][newCol]==PASSWORDDOOR && unlocked[newRow][newCol])) {
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
    }
    for(int i =0; i<monsters_count; i++){
        if(player.row >= monster[i].row - 1 && player.row <= monster[i].row + 1
        && player.col >= monster[i].col - 1 && player.col <= monster[i].col + 1 
        && monster[i].hits < monster[i].lives && monster[i].moves < monster[i].max_moves)
        monster[i].moving = 1;
    }

if(game_map[newRow][newCol] == END){
    clear();
    attron(COLOR_PAIR(2));
    mvprintw(17, 75, "Congrtulations! You won the game");
    attroff(COLOR_PAIR(2));
    refresh();
    getch();
    napms(10000);
    if(load_user_data(&entry)){
        entry.score = entry.score + player.golds*(current_floor+1)/3;
        entry.gold_pieces = entry.gold_pieces + player.golds;
        entry.games_finished = entry.games_finished +1 ;
    }
    else {
        entry.score = player.golds * (current_floor+1) / 3;
        entry.gold_pieces = player.golds;
        entry.games_finished = 1;
        entry.first_game_time = time(NULL);
    }
    savehall(entry);
    entry.score = 0;
    entry.gold_pieces = 0;
    entry.games_finished = 0;
    monsters_count = 0;
    sword_count = 0;
    main_menu();
}
g = 1;
moves ++;
}
void printMap() {
    clear(); 

    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            
            if(visible[i][j]){
                
                switch (game_map[i][j]) {
                case WALLV: 
                if(type[i][j] == 2) attron(COLOR_PAIR(3));
                mvaddch(i, j, '|'); 
                if(type[i][j] == 2) attroff(COLOR_PAIR(3));
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
                if(type[i][j] == 2) attron(COLOR_PAIR(3));
                mvaddch(i, j, '_');
                if(type[i][j] == 2) attroff(COLOR_PAIR(3));
                 break;
                case WALLNO: mvaddch(i, j, ' '); break;
                case TRAP: mvaddch(i, j, '.'); break;
                case STAIRCASE: mvaddch(i, j, '<'); break;
                case SWALLH: 
                if(type[i][j] == 2) attron(COLOR_PAIR(3));
                mvaddch(i, j, '_');
                if(type[i][j] == 2) attroff(COLOR_PAIR(3));
                 break;
                case SWALLV:
                if(type[i][j] == 2) attron(COLOR_PAIR(3)); 
                mvaddch(i, j, '|'); 
                if(type[i][j] == 2) attroff(COLOR_PAIR(3));
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
        if(monster[i].row!=-1 && monster[i].col!= -1){
            if(strcmp(monster[i].name, "Deamon") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'D');
            else if(strcmp(monster[i].name, "Fire Breathing Monster") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'F');
            else if(strcmp(monster[i].name, "Giant") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'G');
            else if(strcmp(monster[i].name, "Snake") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'S');
            else if(strcmp(monster[i].name, "Undeed") == 0 && visible[monster[i].row][monster[i].col] && monster[i].alive) mvaddch(monster[i].row, monster[i].col, 'U');
        }
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
int battleroom(){
    player.health --;
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
        int random = rand();
        if(random % 3 == 0){
            game_map[rown][coln] = SPEED_POTION;
        }
        if(random%3 == 1){
            game_map[rown][coln] = DAMAGE_POTION;
        }
        if(random%3==2){
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
void trapsfortreasure(int row, int col, int roomRows, int roomCols){
    int retries = 5;
    while (retries--)
    {
        int rown = (rand()% (roomRows-2)) + row + 1;
        int coln = (rand()% (roomCols-2)) + col + 1;
        if(game_map[rown][coln] == FLOOR) game_map[rown][coln] == TRAP;
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
    else if(game_map[rown][coln]== FLOOR && random ==3) game_map[rown][coln] == SPEED_POTION;
    }
    

}
void placegold(int row, int col, int roomRows, int roomCols){
    int count = rand() % 10;
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
        if(monster[i].moving){
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
                            attron(COLOR_PAIR(1));
                            mvprintw(1,2, "You got hit by %s!", monster[i].name);
                            attroff(COLOR_PAIR(1));
                            player.health --;
                            getch();
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
                            attron(COLOR_PAIR(1));
                            mvprintw(1,2, "You got hit by %s!", monster[i].name);
                            attroff(COLOR_PAIR(1));
                            player.health --;
                            getch();
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
                            attron(COLOR_PAIR(1));
                            mvprintw(1,2, "You got hit by %s!", monster[i].name);
                            attroff(COLOR_PAIR(1));
                            player.health --;
                            getch();
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
                            attron(COLOR_PAIR(1));
                            mvprintw(1,2, "You got hit by %s!", monster[i].name);
                            attroff(COLOR_PAIR(1));
                            player.health --; 
                            getch();
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
    for (int i = 0; i < monsters_count && i < 20; i++) {
        if (!monster[i].alive) continue; 
        if (abs(monster[i].row - player.row) <= 1 &&
            abs(monster[i].col - player.col) <= 1) {
            monster[i].hits += 5;
            if(player.power) monster[i].hits += 5;
            mvprintw(1, 2, "You hit %s", monster[i].name);
            if (monster[i].hits >= monster[i].lives) {
                monster[i].alive = 0;
                monster[i].moves = monster[i].max_moves;
                attron(COLOR_PAIR(2));
                mvprintw(1, 2, "You killed %s", monster[i].name);
                attroff(COLOR_PAIR(2));
            }
            refresh();
            getch();
        }
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
            last_shot = 22;
        }
        if(ch == 'j'){
            // int hit = 0;
            // for(int i = player.row - 1; i>= player.row - 5; i--){
            //     if(!hit){
            //         for(int j = 0; j < monsters_count; j++){
            //             if(monster[j].row == i && monster[j].col == player.col && monster[j].alive){
            //                 monster[j].hits += 5;
            //                 mvprintw(1, 2, "You hit %s", monster[j].name);
            //                 hit = 1;
            //                 player.dagger --;
            //                 refresh();
            //                 getch();
            //             }
            //             if(monster[j].hits >= monster[j].lives && monster[j].alive){
            //                 monster[j].moving = 0;
            //                 monster[j].moves = monster[j].max_moves;
            //                 attron(COLOR_PAIR(2));
            //                 mvprintw(1,2, "You killed %s", monster[j].name);
            //                 attroff(COLOR_PAIR(2));
            //                 monster[j].alive = 0;
            //                 refresh();
            //                 getch();
            //             }
            //             if(game_map[i][player.col] == WALLH || game_map[i][player.col] == SWALLH){
            //                 player.dagger --;
            //                 game_map[i + 1][player.col] = dagger_SHOT;
            //                 hit = 1;
            //             }
            //         }
            //     }
            // }
            int hit = 0;
    for (int i = player.row - 1; i >= player.row - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    monster[j].hits += 12;
                    if(player.power) monster[j].hits += 12;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i + 1] = dagger_SHOT;
                hit = 1;
                player.dagger--; 
            }
        }
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
        if(ch == 'b'){
            // int hit = 0;
            // for(int i = 1; i<= 5; i++){
            //     if(!hit){
            //         for(int j = 0; j < monsters_count; j++){
            //             if(monster[j].row == player.row + i && monster[j].col == player.col - i && monster[j].alive){
            //                 monster[j].hits += 5;
            //                 mvprintw(1, 2, "You hit %s", monster[j].name);
            //                 hit = 1;
            //                 player.dagger --;
            //                 refresh();
            //                 getch();
            //             }
            //             if(monster[j].hits >= monster[j].lives && monster[j]. alive){
            //                 monster[j].moving = 0;
            //                 monster[j].moves = monster[j].max_moves;
            //                 monster[j].alive = 0;
            //                 attron(COLOR_PAIR(2));
            //                 mvprintw(1, 2, "You killed %s", monster[j].name);
            //                 attroff(COLOR_PAIR(2));
            //                 refresh();
            //                 getch();
            //             }
            //             if(game_map[player.row + i][player.col - i] == WALLV || game_map[player.row + i][player.col - i] == WALLH || game_map[player.row + i][player.col - i] == WALLNO
            //             || game_map[player.row + i][player.col - i] == SWALLV ||game_map[player.row + i][player.col - i] == SWALLH || game_map[player.row + i][player.col - i] == SWALLNO  ){
            //                 player.dagger --;
            //                 game_map[player.row + i  - 1][player.col - i + 1] = dagger_SHOT;
            //                 hit = 1;
            //             }
            //         }
            //     }
            // }
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.dagger--; 
                    if(player.power) monster[j].hits += 12;
                    monster[j].hits += 12;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    if(player.power) monster[j].hits += 12;
                    monster[j].hits += 12;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
            last_shot = 32;
        }
        if(ch == 'j'){
            int hit = 0;
            for (int i = player.row - 1; i >= player.row - 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
    last_shot = 41;
}
        if(ch == 'k'){
            // int hit = 0;
            // for(int i = player.row + 1; i<= player.row +5; i++){
            //     if(!hit){
            //         for(int j = 0; j < monsters_count; j++){
            //             if(monster[j].row == i && monster[j].col == player.col && monster[j].alive){
            //                 monster[j].hits += 5;
            //                 mvprintw(1, 2, "You hit %s", monster[j].name);
            //                 hit = 1;
            //                 player.normal_arrow --;
            //                 refresh();
            //                 getch();
            //             }
            //             if(monster[j].hits >= monster[j].lives && monster[j].alive){
            //                 monster[j].moving = 0;
            //                 monster[j].alive = 0;
            //                 monster[j].moves = monster[j].max_moves;
            //                 attron(COLOR_PAIR(2));
            //                 mvprintw(1,2, "You killed %s", monster[j].name);
            //                 attroff(COLOR_PAIR(2));
            //                 refresh();
            //                 getch();
            //             }
            //             if(game_map[i][player.col] == WALLH || game_map[i][player.col] == SWALLH){
            //                 player.normal_arrow --;
            //                 game_map[i-1][player.col] = normal_arrow_SHOT;
            //                 hit =1;
            //             }
            //         }
            //     }
            // }
            int hit = 0;
    for (int i = player.row + 1; i <= player.row + 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
        if(ch == 'j'){
            // int hit = 0;
            // for(int i = player.row - 1; i>= player.row - 5; i--){
            //     if(!hit){
            //         for(int j = 0; j < monsters_count; j++){
            //             if(monster[j].row == i && monster[j].col == player.col && monster[j].alive){
            //                 monster[j].hits += 5;
            //                 mvprintw(1, 2, "You hit %s", monster[j].name);
            //                 hit = 1;
            //                 player.normal_arrow --;
            //                 refresh();
            //                 getch();
            //             }
            //             if(monster[j].hits >= monster[j].lives && monster[j].alive){
            //                 monster[j].moving = 0;
            //                 monster[j].moves = monster[j].max_moves;
            //                 attron(COLOR_PAIR(2));
            //                 mvprintw(1,2, "You killed %s", monster[j].name);
            //                 attroff(COLOR_PAIR(2));
            //                 monster[j].alive = 0;
            //                 refresh();
            //                 getch();
            //             }
            //             if(game_map[i][player.col] == WALLH || game_map[i][player.col] == SWALLH){
            //                 player.normal_arrow --;
            //                 game_map[i + 1][player.col] = normal_arrow_SHOT;
            //                 hit = 1;
            //             }
            //         }
            //     }
            // }
            int hit = 0;
    for (int i = player.row - 1; i >= player.row - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
        if(ch == 'h'){
            int hit = 0;
    for (int i = player.col - 1; i >= player.row - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i + 1] = normal_arrow_SHOT;
                hit = 1;
                player.normal_arrow--; 
            }
        }
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
        if(ch == 'b'){
            // int hit = 0;
            // for(int i = 1; i<= 5; i++){
            //     if(!hit){
            //         for(int j = 0; j < monsters_count; j++){
            //             if(monster[j].row == player.row + i && monster[j].col == player.col - i && monster[j].alive){
            //                 monster[j].hits += 5;
            //                 mvprintw(1, 2, "You hit %s", monster[j].name);
            //                 hit = 1;
            //                 player.normal_arrow --;
            //                 refresh();
            //                 getch();
            //             }
            //             if(monster[j].hits >= monster[j].lives && monster[j]. alive){
            //                 monster[j].moving = 0;
            //                 monster[j].moves = monster[j].max_moves;
            //                 monster[j].alive = 0;
            //                 attron(COLOR_PAIR(2));
            //                 mvprintw(1, 2, "You killed %s", monster[j].name);
            //                 attroff(COLOR_PAIR(2));
            //                 refresh();
            //                 getch();
            //             }
            //             if(game_map[player.row + i][player.col - i] == WALLV || game_map[player.row + i][player.col - i] == WALLH || game_map[player.row + i][player.col - i] == WALLNO
            //             || game_map[player.row + i][player.col - i] == SWALLV ||game_map[player.row + i][player.col - i] == SWALLH || game_map[player.row + i][player.col - i] == SWALLNO  ){
            //                 player.normal_arrow --;
            //                 game_map[player.row + i  - 1][player.col - i + 1] = normal_arrow_SHOT;
            //                 hit = 1;
            //             }
            //         }
            //     }
            // }
            int hit = 0;
    for (int i = 1; i <= 5; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row + i && monster[j].col ==  player.col - i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    if(player.power) monster[j].hits += 5;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
    }
    else if(player.weapon_in_hand == 4 && player.normal_arrow <= 0){
        attron(COLOR_PAIR(1));
        mvprintw(1, 2, "You don't have enough normal arrows!");
        attroff(COLOR_PAIR(1));
        refresh();
        getch();
    }
    if(player.weapon_in_hand == 5){
        for(int i =0; i< monsters_count; i++){
            if(monster[i].row <= player.row + 1 && monster[i].row >= player.row - 1
            && monster[i].col <= player.col + 1 && monster[i].col >= player.col -1
            && monster[i].alive){
                monster[i].hits += 10;
                if(player.power) monster[i].hits += 10;
                mvprintw(1, 2, "You hit %s", monster[i].name);
                refresh();
                getch();
            }
            if(monster[i].hits >= monster[i].lives && monster[i].alive){
                monster[i].moving = 0;
                monster[i].alive = 0;
                monster[i].moves = monster[i].max_moves;
                attron(COLOR_PAIR(2));
                mvprintw(1, 2, "You killed %s", monster[i].name);
                attroff(COLOR_PAIR(2));
                refresh();
                getch();
            }
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
            mvprintw(1, 2, "You hit %s", monster[i].name);
            if (monster[i].hits >= monster[i].lives) {
                monster[i].alive = 0;
                monster[i].moves = monster[i].max_moves;
                attron(COLOR_PAIR(2));
                mvprintw(1, 2, "You killed %s", monster[i].name);
                attroff(COLOR_PAIR(2));
            }
            refresh();
            getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
            for(int i = player.row + 1; i<= player.row +5; i++){
                if(!hit){
                    for(int j = 0; j < monsters_count; j++){
                        if(monster[j].row == i && monster[j].col == player.col && monster[j].alive){
                            monster[j].hits += 12;
                            mvprintw(1, 2, "You hit %s", monster[j].name);
                            hit = 1;
                            player.dagger --;
                            refresh();
                            getch();
                        }
                        if(monster[j].hits >= monster[j].lives && monster[j].alive){
                            monster[j].moving = 0;
                            monster[j].alive = 0;
                            monster[j].moves = monster[j].max_moves;
                            attron(COLOR_PAIR(2));
                            mvprintw(1,2, "You killed %s", monster[j].name);
                            attroff(COLOR_PAIR(2));
                            refresh();
                            getch();
                        }
                        if(game_map[i][player.col] == WALLH || game_map[i][player.col] == SWALLH){
                            player.dagger --;
                            game_map[i-1][player.col] = dagger_SHOT;
                            hit =1;
                        }
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[player.row][i] == SWALLV)) {
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    player.dagger --; 
                    monster[j].hits += 12;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
            for (int i = player.row - 1; i >= player.row - 10; i++) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == i && monster[j].col == player.col && monster[j].alive) {
                    hit = 1;
                    player.magic_wand --; 
                    if(player.power) monster[j].hits += 15;
                    monster[j].hits += 15;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
    for (int i = player.col - 1; i >= player.row - 5; i--) {
        if (!hit) {
            for (int j = 0; j < monsters_count; j++) {
                if (monster[j].row == player.row && monster[j].col == i && monster[j].alive) {
                    hit = 1;
                    player.normal_arrow--; 
                    monster[j].hits += 5;
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
                    break; 
                }
            }
            if (!hit && (game_map[i][player.col] == WALLV || game_map[player.row][i] == SWALLV)) {
                game_map[player.row][i + 1] = normal_arrow_SHOT;
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                    mvprintw(1, 2, "You hit %s", monster[j].name);
                    if (monster[j].hits >= monster[j].lives) {
                    monster[j].alive = 0;
                    monster[j].moving = 0;
                    monster[j].moves = monster[j].max_moves;
                    attron(COLOR_PAIR(2));
                    mvprintw(1, 2, "You killed %s", monster[j].name);
                    attroff(COLOR_PAIR(2));
                }
                    getch();
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
                mvprintw(1, 2, "You hit %s", monster[i].name);
                refresh();
                getch();
            }
            if(monster[i].hits >= monster[i].lives && monster[i].alive){
                monster[i].moving = 0;
                monster[i].alive = 0;
                monster[i].moves = monster[i].max_moves;
                attron(COLOR_PAIR(2));
                mvprintw(1, 2, "You killed %s", monster[i].name);
                attroff(COLOR_PAIR(2));
                refresh();
                getch();
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
        mvprintw(1, 2, "                                                                                  ");
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
        current_message = -1;
        refresh();
}