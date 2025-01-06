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
char game_mode[20] = "";
char character_color[20]= "";
#define FILENAME "users.txt"
#define HALLFILE "halloffame.txt"
#define MAXROW 35
#define MAXCOL 150
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
    ANCIENTKEY = 16
} map_elements;
map_elements game_map[MAXROW][MAXCOL];
map_elements previous[MAXROW][MAXCOL];
int visible[MAXROW][MAXCOL];
int unlocked[MAXROW][MAXCOL];
typedef struct {
    char map[MAXROW][MAXCOL];
    int player_row, player_col;
} Floor;
// typedef struct{
//     firstfloor;
//     secondfloor;
//     row;
//     col;
// } Stairs;
// Stairs stairs[4];
Floor floors[4];
int current_floor = 0;
char pass[5];
typedef struct {
    int row;
    int col;
    int health;
    int ancientkeys;
    int brokenkeys;
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

typedef struct {
    char username[50];
    int score;
    int gold_pieces;
    int games_finished;
    time_t first_game_time;
} HallOfFameEntry;
Room rooms[10];
int room_index = 0;
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
void createrandomdoor(int row, int col, int roomRows, int roomCols);
void placetrap(int row, int col, int roomRows, int roomCols);
void corridors(int numRooms, Room *rooms);
void generate_random_map(int staircase_row, int staircase_col, int staircase_roomRows, int staircase_roomCols);
void removeUnconnectedDoors();
void movePlayer(int newRow, int newCol);
void printMap();
void createrandompillar(int row, int col, int roomRows, int roomCols);
int battleroom();
void placestaircase(int row, int col, int roomRows, int roomCols);
void saveFloor();
void loadFloor(int floor);
void placepassword(int row, int col, int roomRows, int roomCols);
void generatedoorpass();
void removepassword(int row, int col, int roomRows, int roomCols);
void placeancientkey(int row, int col, int roomRows, int roomCols);
int main() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    //setlocale(LC_ALL, ""); 
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    //init_color(COLOR_CYAN, 255, 255, 0);  
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    
    // if (can_change_color()) {
    //     init_color(COLOR_RED + 1, 1000, 647, 0); 
    // }

    // init_pair(6, COLOR_RED + 1, COLOR_BLACK); 
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
            } //hall of fame
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
                HallOfFameEntry entry = {0};
                strcpy(entry.username, current_username);
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
    mvwprintw(win, 3, 2, "Username: ");
    mvwprintw(win, 3, 12, "                     ");
    //mvwprintw(win, 8, 2, "                                       ");
    //mvwprintw(win, 10, 2, "DEBUG: Before username input"); // Debugging
    wrefresh(win);

    i = 0;
    memset(username, 0, sizeof(username));
    while (1) {
        ch = wgetch(win);
        //mvwprintw(win, 11, 2, "DEBUG: Username char = %c", ch); // Debugging character
        //wrefresh(win);

        if (ch == '\n' || ch == 10 || ch == '\r' || ch == KEY_ENTER) {
            username[i] = '\0'; // Null-terminate username
            //mvwprintw(win, 12, 2, "DEBUG: Username entered = %s", username); // Debugging username
            wrefresh(win);
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (i > 0) {
                i--;
                mvwaddch(win, 3, 12 + i, ' ');
                wmove(win, 3, 12 + i);
            }
        } else if (i < 49 && isprint(ch)) {
            username[i++] = ch;
            mvwaddch(win, 3, 12 + i - 1, ch);
        }
        wrefresh(win);
    }
    // mvwprintw(win, 9, 2, "DEBUG: Captured Username = %s", username);
    // mvwprintw(win, 13, 2, "                                        ");
    // mvwprintw(win, 13, 2, "DEBUG: Checking if user exists"); // Debugging
    // wrefresh(win);

    if (user_exists(username)) {
        wattron(win, COLOR_PAIR(1));
        mvwprintw(win, 5, 2, "Username already exists! Pick another one.");
        wattroff(win, COLOR_PAIR(1));
        wrefresh(win);
        continue;
    } else {
        break;
    }

}
flushinp();
mvwprintw(win, 5, 2, "                                          ");
    while (1) {
        mvwprintw(win, 6, 2, "Password: ");
        mvwprintw(win, 6,12, "                     ");
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
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 7,2, "Too short! must include at least 7 characters");
            wattroff(win, COLOR_PAIR(1));
        }
        else if(validate_password(password)==2){
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 7, 2, "Must include at least one digit!");
            wattroff(win, COLOR_PAIR(1));
        }
        else if(validate_password(password)==3){
            wattron(win, COLOR_PAIR(1));
            mvwprintw(win, 7, 2, "Must include at least one uppercase character!");
            wattroff(win, COLOR_PAIR(1));
        }
        else if(validate_password(password)==4){
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
    mvwprintw(win, 7, 2, "                                              ");
    while(1){
        mvwprintw(win, 8, 2, "Email: ");
        mvwprintw(win, 8,12, "                                        ");
        //mvwprintw(win, 8, 2, "                                       ");
        wrefresh(win);
        //mvwgetnstr(win, 5, 12, email, 99);
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
        // mvwprintw(win, 13, 2, "                                        ");
        // wrefresh(win);
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
    mvwprintw(win, 8, 2, "                                       ");
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
            break;
        }
    }
    flushinp();
    mvwprintw(win, 5, 2, "                                                ");
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
            strncpy(current_username, username, sizeof(current_username) - 1);
            current_username[sizeof(current_username) - 1] = '\0';
            wattron(win, COLOR_PAIR(2));
            mvwprintw(win, 8, 2, "Login successful");
            wattroff(win, COLOR_PAIR(2));
            wrefresh(win);
            HallOfFameEntry entry = {0};
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
void savehall(const HallOfFameEntry entry){
    FILE *file = fopen(HALLFILE, "a");
    if(!file) {
        return;
    }
    fprintf(file, "%s %d %d %d %ld\n", entry.username, entry.score, entry.gold_pieces, entry.games_finished, entry.first_game_time);
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
                attron(A_BOLD);
            }
            if(i==0){
                wattron(stdscr, COLOR_PAIR(3) | A_BOLD);
                mvprintw(row, 2, "🥇 legend %s - %d - %d gold - %d games - %ld days",
                         entries[i].username,
                         entries[i].score,
                         entries[i].gold_pieces,
                         entries[i].games_finished,
                         (time(NULL) - entries[i].first_game_time) / (60 * 60 * 24));
                wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
            }
            else if(i==1){
                wattron(stdscr, COLOR_PAIR(3) | A_BOLD);
                mvprintw(row, 2, "🥈 legend %s - %d - %d gold - %d games - %ld days",
                         entries[i].username,
                         entries[i].score,
                         entries[i].gold_pieces,
                         entries[i].games_finished,
                         (time(NULL) - entries[i].first_game_time) / (60 * 60 * 24));
                wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
            }
            else if(i==2){
                wattron(stdscr, COLOR_PAIR(3) | A_BOLD);
                mvprintw(row, 2, "🥉 legend %s - %d - %d gold - %d games - %ld days",
                         entries[i].username,
                         entries[i].score,
                         entries[i].gold_pieces,
                         entries[i].games_finished,
                         (time(NULL) - entries[i].first_game_time) / (60 * 60 * 24));
                wattroff(stdscr, COLOR_PAIR(3) | A_BOLD);
            }
            else {
                mvprintw(row, 2, "%d. %s - %d - %d gold - %d games - %ld days",
                     i,
                     entries[i].username,
                     entries[i].score,
                     entries[i].gold_pieces,
                     entries[i].games_finished,
                     (time(NULL) - entries[i].first_game_time) / (60 * 60 * 24));
        }
                     if(strcmp(entries[i].username, current_username)==0){
                        attroff(A_BOLD);
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
                case STAIRCASE: mvaddch(i, j, 'S'); break;
                case SWALLH: mvaddch(i, j, 'D'); break;
                case SWALLV: mvaddch(i, j, 'D'); break;
                case SWALLNO: mvaddch(i, j, 'D'); break;
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
    clear();
    mvprintw(1, 2, "This is where messages should appear");
    refresh();
    curs_set(0);
    player.health = 5;
    player.ancientkeys = 0;
    player.brokenkeys = 0;
    generate_random_map(0, 0, 0, 0);
    removeUnconnectedDoors();
    while(1){
        printMap();
        mvprintw(35, 10, "Health = %d", player.health);
        mvprintw(35, 60, "ancient keys = %d", player.ancientkeys);
        mvprintw(35, 110, "broken keys = %d", plauer.brokenkeys);
        refresh();

        int ch = getch();
        switch (ch)
        {
        case 'y':
            movePlayer(player.row -1, player.col -1);
            break;
        case 'u':
            movePlayer(player.row-1, player.col+1);
            break;
        case 'h':
            movePlayer(player.row, player.col-1);
            break;
        case 'j':
            movePlayer(player.row-1, player.col);
            break;
        case 'k' :
            movePlayer(player.row+1, player.col);
            break;
        case 'l' :
            movePlayer(player.row, player.col+1);
            break;
        case 'b':
            movePlayer(player.row+1, player.col-1);
            break;
        case 'n':
            movePlayer(player.row+1, player.col+1);
            break;
        case 'a':
            if(player.brokenkeys >= 2){
                player.ancientkeys ++;
                player.brokenkeys = player.brokenkeys -2;
            }
        case 'q':
            return;
        case 'm':
            printall();
        default:
            break;
        }
    }
    refresh();
   HallOfFameEntry entry = {0};
   strcpy(entry.username, current_username);
   if(load_user_data(&entry)){
    entry.score = entry.score + 100;
    entry.gold_pieces = entry.gold_pieces + gold;
    entry.games_finished = entry.games_finished +1 ;
   }
   else {
    entry.score = 100;
    entry.gold_pieces = gold;
    entry.games_finished = 1;
    entry.first_game_time = time(NULL);
   }
   savehall(entry);
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
        "Easy mode",
        "Hard mode",
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
                strcpy(game_mode, "Hard");
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
void createrandomdoor(int row, int col, int roomRows, int roomCols){
    int doors_placed = 0;
    int max_retries = ( rand()%3 ) +1;
    while (max_retries--)
    {
        int wall = rand() % 4;
        int door_row, door_col;
        switch(wall){
            case 0:
            door_row = row;
            door_col = col + rand() %(roomCols);
            break;
            case 1:
            door_row= row + roomRows;
            door_col = col+ rand() % (roomCols);
            break;
            case 2:
            door_row = row+ rand()%(roomRows);
            door_col = col;
            break;
            case 3:
            door_row  = row + rand() % (roomRows);
            door_col = col + roomCols;
            break;
            default:
            continue;
        }
        if (game_map[door_row][door_col] == WALLH || game_map[door_row][door_col] == WALLV && door_row!=0 && door_row!= MAXROW && door_col!=0 && door_col!=MAXCOL && game_map[door_row][door_col] != WALLNO) {
            if(rand()%20 == 0){
                if(game_map[door_row][door_col] == WALLH) game_map[door_row][door_col] = SWALLH;
                if(game_map[door_row][door_col] == WALLV) game_map[door_row][door_col] = SWALLV;
                if(game_map[door_row][door_col == WALLNO]) game_map[door_row][door_col] = SWALLNO;
            }
            else if(rand()% 13 == 0){
                game_map[door_row][door_col] = PASSWORDDOOR;
                placepassword(row, col, roomRows, roomCols);
            }
            else {
                game_map[door_row][door_col] = DOOR;
            }
            doors_placed = 1;
        }
    }
    if( doors_placed ==0 ) createrandomdoor(row, col, roomRows, roomCols);
    
}
void place_door(int row, int col) {
    if (rand()%20 == 0) {
        game_map[row][col] = (game_map[row][col] == WALLH) ? SWALLH :
                             (game_map[row][col] == WALLV) ? SWALLV :
                             SWALLNO;
    } else if (rand() % 13 == 0) { 
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
    int stairroom = rand() % roomcount;
    int ancientkeyroom = rand() % roomcount;
    int retry_count = 0;

    room_index = 0;
    if (staircase_row > 0 && staircase_col > 0) {
        createRoom(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        //createrandomdoor(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        createrandompillar(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        makeitvisible(staircase_row, staircase_col, staircase_roomRows, staircase_roomCols);
        rooms[room_index].center_x = staircase_col + staircase_roomCols / 2;
        rooms[room_index].center_y = staircase_row + staircase_roomRows / 2;
        rooms[room_index].width = staircase_roomCols;
        rooms[room_index].length = staircase_roomRows;
        game_map[player.row][player.col] = STAIRCASE;
        room_index++;
    }
    for (int i = 0; i < roomcount; i++) {
        int start_row = rand() % (MAXROW - 9);
        int start_col = rand() % (MAXCOL - 9);
        int rowscount = rand() % 5 + 5;
        int colscount = rand() % 5 + 5;

        if (!itoverlaps(start_row, start_col, rowscount, colscount)) {
            createRoom(start_row, start_col, rowscount, colscount);
            createrandomdoor(start_row, start_col, rowscount, colscount);
            createrandompillar(start_row, start_col, rowscount, colscount);
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
            if (room_index == stairroom && current_floor== 0) placestaircase(start_row, start_col, rowscount, colscount);
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
    if(game_map[newRow][newCol] == ANCIENTKEY) {
        player.ancientkeys ++;
        game_map[newRow][newCol] = FLOOR;
    }
    if (game_map[newRow][newCol] == PASSWORD) {
    generatedoorpass();
    mvprintw(1, 75, "%s", pass);
    refresh();
    napms(10000);
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
            mvprintw(1, 2, "You broke your ancient key:(  if you have two broken ancient keys press a");    
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
                    mvprintw(1, 2, "Wrong password! Try again! The password you entered: %s, the actual password: %s", input, pass);
                    attroff(COLOR_PAIR(5));
                    refresh();
                }
                if (retries == 1) {
                    mvprintw(1, 2, "                                                                                  ");
                    attron(COLOR_PAIR(1));
                    mvprintw(10, 10, "Wrong password! Try again!");
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

    if(game_map[newRow][newCol] == TRAP) battleroom();
    if (game_map[newRow][newCol] == STAIRCASE && current_floor == 0) {
        saveFloor();
        int start_row = newRow, end_row = newRow, start_col = newCol, end_col = newCol;
        while (start_row > 0 && game_map[start_row - 1][newCol] != EMPTY && game_map[start_row - 1][newCol] !=CORRIDOR) start_row--;
        while (end_row < MAXROW - 1 && game_map[end_row + 1][newCol] != EMPTY && game_map[end_row + 1][newCol] != CORRIDOR) end_row++;
        while (start_col > 0 && game_map[newRow][start_col - 1] != EMPTY && game_map[newRow][start_col - 1] != CORRIDOR) start_col--;
        while (end_col < MAXCOL - 1 && game_map[newRow][end_col + 1] != EMPTY && game_map[newRow][end_col + 1] != CORRIDOR) end_col++;
        int room_rows = end_row - start_row + 1;
        int room_cols = end_col - start_col + 1;
        current_floor++;
        generate_random_map(start_row, start_col, room_rows-1, room_cols-1);
        makeitvisible(start_row, start_col, room_rows, room_cols);
        player.row = start_row + room_rows / 2;
        player.col = start_col + room_cols / 2;
    }


    if(game_map[newRow][newCol] == DOOR || game_map[newRow][newCol]==SWALLH
    || game_map[newRow][newCol]==SWALLV || game_map[newRow][newCol]==SWALLNO
    || game_map[newRow][newCol]==PASSWORDDOOR) {
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
}
void printMap() {
    clear(); 

    for (int i = 0; i < MAXROW; i++) {
        for (int j = 0; j < MAXCOL; j++) {
            
            if(visible[i][j]){
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
                case TRAP: mvaddch(i, j, '.'); break;
                case STAIRCASE: mvaddch(i, j, 'S'); break;
                case SWALLH: mvaddch(i, j, '_'); break;
                case SWALLV: mvaddch(i, j, '|'); break;
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
    
    refresh();
}
void createrandompillar(int row, int col, int roomRows, int roomCols){
    int number_of_pillars = rand() % 2 ;
    while(number_of_pillars --){
        // row+1 ta row+roomrows-1
        // col+1 ta col +roomcols -1
        int rown = ( rand() %(roomRows-2) ) +row+1;
        int coln = (rand() % (roomCols-2)) +col +1;
        if(game_map[rown][coln] == FLOOR )game_map[rown][coln] = PILLAR;
        else {
            createrandomdoor(row, col, roomRows, roomCols);
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
    //stairs[current_floor].firstfloor = current_floor;
    //stairs[current_floor].secondfloor = current_floor +1;
    }
    else{
        placestaircase(row, col, roomRows, roomCols);
    }
}
void saveFloor() {
    memcpy(floors[current_floor].map, game_map, sizeof(game_map));
    floors[current_floor].player_row = player.row;
    floors[current_floor].player_col = player.col;
}
void loadFloor(int floor) {
    current_floor = floor;
    memcpy(game_map, floors[floor].map, sizeof(game_map));
    player.row = floors[floor].player_row;
    player.col = floors[floor].player_col;
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