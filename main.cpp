#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "c_string.h"

using namespace std;

enum e_directions {
    D_VERTIVAL = 0,
    D_HORIZONTAL = 1,
    D_FORWARD = 1,
    D_BACKWARD = -1
};

enum e_cell {
    CL_NEW=-3,
    CL_MISS,
    CL_DEAD,
    CL_EMPTY=0,
};

enum e_buttleground_states {
    BG_SELECTION = 0,
    BG_SHOTING = 1,
};

void printCell (short int cellState, bool selected = false) {
    if(selected==true) {
        cout << "[";
    } else {
        cout << " ";
    }
    switch(cellState){
        case CL_EMPTY:
            cout << " ";
            break;
        case CL_NEW:
            cout << "0";
            break;
        case CL_DEAD:
            cout << "x";
            break;
        case CL_MISS:
            cout << "*";
            break;
        default:
            cout << "#";
            break;
    }
    if(selected==true) {
        cout << "]";
    } else {
        cout << " ";
    }
}

void printButtleground(short ** battleGround, int battlegroundSize, int pointer[], int * newShip, short state = BG_SHOTING){

    bool selected;

    cout << "  ";

    for(int j = 0; j<battlegroundSize; j++) {
        cout << setw(2) << " " << j+1;
    }

    cout << endl;

    for(int i = 0; i<battlegroundSize; i++) {
        cout << setw(2) << i+1 << " ";
        for(int j = 0; j<battlegroundSize; j++) {
            if((pointer[0]==i) && (pointer[1]==j)) {
                selected = true;
            } else if(state == BG_SELECTION) {

                if((newShip[1]==D_HORIZONTAL) && (pointer[0] == i) && (j>=pointer[1] && j<pointer[1]+newShip[0])) {
                    selected = true;
                } else if(newShip[1]==D_VERTIVAL && (pointer[1] == j) && (i>=pointer[0] && i<pointer[0]+newShip[0])) {
                    selected = true;
                } else {
                    selected = false;
                }

            } else {
                selected = false;
            }
            printCell(battleGround[i][j], selected);
        }
        cout << endl;
    }
}

enum e_action {
    A_UP=119,
    A_DOWN=115,
    A_LEFT=97,
    A_RIGHT=100,
    A_DO=32,
    A_EXIT=27,
    A_TURN=114,
    A_DELETE=120
};



void movePointer(int action, int pointer [], int battlegroundSize, int newShip[], short state) {

    int movePoint;
    int moveDirection = D_FORWARD;

    if((action == A_UP) || (action == A_DOWN )){
        movePoint = D_VERTIVAL;
    } else if((action == A_LEFT) || (action == A_RIGHT )) {
        movePoint = D_HORIZONTAL;
    }

    if((action==A_LEFT) || (action==A_UP)){
        moveDirection = D_BACKWARD;
    }

    switch (state) {
        case BG_SHOTING:
            
            pointer[movePoint] += moveDirection;
            if(pointer[movePoint] >= battlegroundSize) {
                pointer[movePoint] = 0;
            } else if(pointer[movePoint]<0) {
                pointer[movePoint] = battlegroundSize-1;
            }
            break;

        case BG_SELECTION:

            int overflow = pointer[movePoint] + moveDirection;

            if((newShip[1]==movePoint) && ((overflow + newShip[0]) > battlegroundSize)) {
                return;
            }else if(overflow<0 || overflow >= battlegroundSize){
                return;
            } else {
                pointer[movePoint] += moveDirection;
            }
            break;
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

    int newShip[2] = {3, D_HORIZONTAL};

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

/*    for(int i = 0; i<battlegroundSize; i++) {
        for(int j = 0; j<battlegroundSize; j++) {
            battleGround[i][j] = rand()%4;
        }
    }*/


    int action;
    e_buttleground_states state = BG_SELECTION;

    do {

        system("clear");

        printButtleground(battleGround, battlegroundSize, pointer, newShip, state);

        cout << endl << "action [" << action << "]" << endl;
        cout << endl << "new ship [" << newShip[0] << ":"<< newShip[1] << "]" << endl;

        cout << endl << "pointer [" << pointer[0]+1 << ":" << pointer[1]+1 << "]" << endl;

        cout << endl << "Controls:";

        cout << endl << "w, a, s, d - to movie";
        if(state==BG_SELECTION) {
            cout << endl << "r - to turn ship";
        }
        cout << endl << "space - to action";
        cout << endl << "esc - to exit";

        cout << endl;

        action = mygetch();

        switch (action) {
            case A_UP:
            case A_DOWN:
            case A_LEFT:
            case A_RIGHT:
                movePointer(action, pointer, battlegroundSize, newShip, state);
                break;
            case A_DO:

                break;
            case A_TURN:
                newShip[1] = (newShip[1]==D_HORIZONTAL) ? D_VERTIVAL : D_HORIZONTAL;
                break;
        }

    } while (action!=A_EXIT);

    cout << endl << "Press any key to exit..." << endl;

    mygetch();

    system("clear");

    return 0;
}