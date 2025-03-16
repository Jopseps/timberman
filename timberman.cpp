#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <termios.h>    //conio.h in windows (linux doesnt conio.h library)
#include <unistd.h>
#include <random>

using namespace std;

char getch() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, & oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, & newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, & oldt);
    return ch;
}

char get_arrow_key() {
    char ch = getch();
    if (ch == '\033') { // esc key
        if (getch() == '[') { // waiting for '[' character
            switch (getch()) { // third character determines the arrow direction
            case 'A':   //up key
                return 'w';
            case 'B':   //down key
                return 's';
            case 'C':   //right key
                return 'd';
            case 'D':   //left key
                return 'a';
            }
        }
    }
    return ch; // here returning the original input to use keys for volume control and on/off
}


void speeddraw(int dStart, int dEnd, unsigned int blockID) { //0 is the default value if you dont want any spaces at the start
    //block ID 1 = 🮆
    //block ID 2 = space
    //block ID 3 = stikmen

    for (int i = 0; i < dStart; i++) {
        printf(" ");
    }

    for (int i = 0; i < dEnd; i++) {
        if (blockID == 1) printf("🮆");
        if (blockID == 2) printf(" ");
        if (blockID == 3) printf("🯅");
        if (blockID == 8) printf("⏽");
        if (blockID == 9) printf("⭘");
        if (blockID == 10) printf("▕");
        if (blockID == 11) printf("▁");
        if (blockID == 12) printf("▏");
        if (blockID == 13) printf("▔");
    }
}

int RNG(int max_range){ // works with 1 to the infinity (pseudo random)
    return (rand() % max_range);
}

const int row_count = 8;
int row_info[row_count + 1]; // + 1

int isLastOneHasLog = 0; //to prevent log spam 
void spawn_first_logs(){
    
    row_info[0] = 0; //to prevent spawn death/kill
    for(int i = 1; i < row_count; i++){
        if(isLastOneHasLog == 0){
            row_info[i] = RNG(3); //0 means there is no log 1 means the log is going to spawn left 2 means the log is going to spawn left 
            if(row_info[i] == 1 || row_info[i] == 2){isLastOneHasLog = 1;} //the next log is safe now
            else{isLastOneHasLog = 0;}
            
        }
        else{row_info[i] = 0; isLastOneHasLog = 0;}
    }
    
}
//0 en alt
void next_log(){
    for(int i = 0; i < row_count ; i++){
        row_info[i] = row_info[i+1];
    }
    if(isLastOneHasLog == 0){
        row_info[row_count] = RNG(3);
        if(row_info[row_count] == 1 || row_info[row_count] == 2){isLastOneHasLog = 1;}
        else{isLastOneHasLog = 0;}
    }
    else{row_info[row_count] = 0; isLastOneHasLog = 0;}
}

int max_box_x = 30;
int max_box_y = row_count;
int character_pos = 2; // 0 is none 1 is left 2 is right
int isCharacterAlive = 1;
int score = 0;

void init(){
    character_pos = 2;
    isCharacterAlive = 1;
    score = 0;
}

void draw_scene_row(int rinfo, int charpos){
    if(charpos == 0){ //no character
        if(rinfo == 0){
            speeddraw(max_box_x - 1, 1, 1);
        }
        if(rinfo == 1){
            speeddraw(max_box_x - 3, 3, 1);
        }
        if(rinfo == 2){
            speeddraw(max_box_x - 1, 3, 1);
        }
    }
    if(charpos == 1){ //left
        if(rinfo == 0){
            speeddraw(max_box_x - 3, 1, 3); speeddraw(1,1,1);
        }
        if(rinfo == 1){
            speeddraw(max_box_x - 4, 1, 1); speeddraw(0,1,3); speeddraw(1,1,1); isCharacterAlive = 0; //bombooclaaaat
        }
        if(rinfo == 2){
            speeddraw(max_box_x - 3, 1, 3); speeddraw(1,3,1);
        }
    }
    if(charpos == 2){ //right
        if(rinfo == 0){
            speeddraw(max_box_x-1,1,1); speeddraw(0,1,3);
        }
        if(rinfo == 1){
            speeddraw(max_box_x-3,3,1); speeddraw(0,1,3);
        }
        if(rinfo == 2){
            speeddraw(max_box_x-1,1,1);speeddraw(0,1,3);speeddraw(1,1,1); isCharacterAlive = 0; //bombooclaaatt
        }
    }
}

void printscore(){
    if(score >= 0 && score < 10){
        printf("\n"); speeddraw(max_box_x - 1,0,2); printf("%d", score); printf("\n");
    }
    else if(score >= 10 && score < 100){
        printf("\n"); speeddraw(max_box_x - 2,0,2); printf("%d", score); printf("\n");
    }
    else if(score >= 100 && score < 1000){
        printf("\n"); speeddraw(max_box_x - 2,0,2); printf("%d", score); printf("\n");
    }
    else{
        printf("\n"); speeddraw(max_box_x - 3,0,2); printf("%d", score); printf("\n");
    }
}

void frame_generate(){
    system("clear");
    for(int i = row_count; i > 0; i--){
        draw_scene_row(row_info[(i)],0);
        printf("\n");
    }
    draw_scene_row(row_info[0],character_pos);
    printscore();
}



int main(){
    spawn_first_logs();
    char key = '0';
    while (key != '^[[A' && key != '^[[C' && key != '^[[D' && isCharacterAlive == 1){
        frame_generate(); //isCharacterAlive check after frame_generate()
        if(isCharacterAlive == 0){
            printf("\n"); speeddraw(2 *max_box_x/3,0,2); printf("Game over, Score = %d\n\n", score); break;
        }
        
        key = get_arrow_key();
        if (key == 'w' || key == 's') {
            next_log();
        }
        else if (key == 'd') {
            character_pos = 2;
            next_log();
        }
        else if (key == 'a') {
            character_pos = 1;
            next_log();
        }
        if(key == 'w' || key == 'd' || key == 'a' && isCharacterAlive == 1){score += 1;}
        key = '0';
        std::this_thread::sleep_for(std::chrono::milliseconds(20)); //tick        
    }

    printf("\nPress Enter to play again");
    do{
        key = get_arrow_key();
    }while(key != '\n');
    init();
    main();
}

//Made by Jopseps