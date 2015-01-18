#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "c_string.h"

using namespace std;

void printCell (short int cellState, bool selected = false) {
    if(selected==true) {
        cout << "[";
    } else {
        cout << " ";
    }
    switch(cellState){
        case 0:
            cout << " ";
            break;
        case 1:
            cout << "#";
            break;
        case 2:
            cout << "x";
            break;
        case 3:
            cout << "*";
            break;
    }
    if(selected==true) {
        cout << "]";
    } else {
        cout << " ";
    }
}

void printButtleground(short ** battleGround, int battlegroundSize, int pointer[]){

    bool selected;

    cout << "  ";

    for(int j = 0; j<battlegroundSize; j++) {
        cout << setw(2) << " " << j+1;
    }

    cout << endl;


    for(int i = 0; i<battlegroundSize; i++) {
        cout << setw(2) << i+1 << " ";
        for(int j = 0; j<battlegroundSize; j++) {
            selected = ((pointer[0]==i) && (pointer[1]==j)) ? true : false;
            printCell(battleGround[i][j], selected);
        }
        cout << endl;
    }
}

enum e_action {A_UP=119, A_DOWN=115, A_LEFT=97, A_RIGHT=100, A_DO=32, A_EXIT=27};


void movePointer(int action, int pointer [], int battlegroundSize) {

    int movePoint;
    int moveDirection = 1;

    if((action == A_UP) || (action == A_DOWN )){
        movePoint = 0;
    } else if((action == A_LEFT) || (action == A_RIGHT )) {
        movePoint = 1;
    }

    if((action==A_LEFT) || (action==A_UP)){
        moveDirection = -1;
    }

    pointer[movePoint] += moveDirection;
    if(pointer[movePoint] >= battlegroundSize) {
        pointer[movePoint] = 0;
    } else if(pointer[movePoint]<0) {
        pointer[movePoint] = battlegroundSize-1;
    }
}


int main(){

    srand(time(NULL));

    system("clear");

    /*
        battleground size
     */

    int battlegroundSize = 10;

    int pointer[2] = {0, 0};

    /*
        init battleground
     */

    short ** battleGround = new short * [battlegroundSize];
    for (int i = 0; i<battlegroundSize; i++) {
        battleGround[i] = new short [battlegroundSize];
    }

    /*
        Set random field for battleground
     */

    for(int i = 0; i<battlegroundSize; i++) {
        for(int j = 0; j<battlegroundSize; j++) {
            battleGround[i][j] = rand()%4;
        }
    }



    int action;

    do {
        system("clear");

        printButtleground(battleGround, battlegroundSize, pointer);

        cout << endl << "pointer [" << pointer[0]+1 << ":" << pointer[1]+1 << "]" << endl;

        cout << endl << "Controls:" << endl << "w, a, s, d - to movie" << endl <<"space - to action" << endl <<  "esc - to exit" << endl;

        action = mygetch();

        switch (action) {
            case A_UP:
            case A_DOWN:
            case A_LEFT:
            case A_RIGHT:
                movePointer(action, pointer, battlegroundSize);
                break;
        }

    } while (action!=A_EXIT);


    cout << endl << "Press any key to exit..." << endl;

    mygetch();

    system("clear");

    return 0;
}