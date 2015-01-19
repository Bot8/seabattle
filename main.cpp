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

enum e_game_stage {
    BG_SELECTION = 0,
    BG_SHOTING = 1,
};

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

struct fleet {
    int ship[10][2];
};

void printCell (short int cellstage, bool selected = false) {
    if(selected==true) {
        cout << "[";
    } else {
        cout << " ";
    }
    switch(cellstage){
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

void printButtleground(short ** battleGround, int battlegroundSize, int pointer[], int * newShip, short stage = BG_SHOTING){

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
            } else if(stage == BG_SELECTION) {

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

void checkDirection(int action, int & movePoint, int & moveDirection) {

    if((action == A_UP) || (action == A_DOWN )){
        movePoint = D_VERTIVAL;
    } else if((action == A_LEFT) || (action == A_RIGHT )) {
        movePoint = D_HORIZONTAL;
    }

    if((action==A_LEFT) || (action==A_UP)){
        moveDirection = D_BACKWARD;
    } else if((action==A_RIGHT) || (action==A_DOWN)) {
        moveDirection = D_FORWARD;
    }

}


void movePointer(int pointer [], int action, int battlegroundSize) {

    int movePoint;
    int moveDirection;

    checkDirection(action, movePoint, moveDirection);

    pointer[movePoint] += moveDirection;
    if(pointer[movePoint] >= battlegroundSize) {
        pointer[movePoint] = 0;
    } else if(pointer[movePoint]<0) {
        pointer[movePoint] = battlegroundSize-1;
    }

}

void moveShip(int pointer[], int action, int battlegroundSize, int newShip[]) {

    int movePoint;
    int moveDirection;

    checkDirection(action, movePoint, moveDirection);

    int overflow = pointer[movePoint] + moveDirection;

    if((newShip[1]==movePoint) && ((overflow + newShip[0]) > battlegroundSize)) {
        return;
    } else if(overflow<0 || overflow >= battlegroundSize) {
        return;
    } else {
        pointer[movePoint] += moveDirection;
    }

}

void turnShip(int pointer[], int battlegroundSize, int newShip[]){
    int turnTo;
    switch(newShip[1]) {
        case D_HORIZONTAL:
            turnTo = 0;
            break;
        case D_VERTIVAL:
            turnTo = 1;
            break;
    }
    if(pointer[turnTo]+newShip[0]<=battlegroundSize) {
        newShip[1] = (newShip[1]==D_HORIZONTAL) ? D_VERTIVAL : D_HORIZONTAL;
    }
}

void craftFleet(int ships [10][2]){
    int types[4][2] = {
            {1,4},
            {2,3},
            {3,2},
            {4,1},
    };
    int typeIndex = 0;
    for(int i = 0; i<10; i++) {
        ships[i][0] = types[typeIndex][0];
        ships[i][1] = 0;
        types[typeIndex][1]--;
        if(types[typeIndex][1] < 1) {
            typeIndex++;
        }
    }
}

void printShipType(int shipSize, int countShipType) {

    cout << countShipType << " ";

    for(int i = 0; i<shipSize; i++) {
        cout << "#";
    }
    cout << endl;
}

void inspectFleet(int ships [10][2]){

    int shipSize = 1;
    int countShipType = 0;

//    for (int i = 0; i<10; i++) {
//        cout << "Ship id=" << i << " size=" << ships[i][0] << " dead=" <<ships[i][1] << endl;
//    }

    for (int i = 0; i<10; i++) {
        if(ships[i][0]!=shipSize) {
            printShipType(shipSize, countShipType);
            shipSize++;
            countShipType = 0;
        }
        if(ships[i][0]!=ships[i][1]){
            countShipType++;
        }
    }

    printShipType(shipSize, countShipType);

}

int main(){

    srand(time(NULL));

    system("clear");

    /*
        battleground size
     */

    int battlegroundSize = 10;

    int pointer[2] = {0, 0};

    fleet myFleet, PCFleet;

    craftFleet(myFleet.ship);

    int newShip[2] = {3, D_HORIZONTAL};

    /*
        init battleground
     */

    short ** battleGround = new short * [battlegroundSize];
    for (int i = 0; i<battlegroundSize; i++) {
        battleGround[i] = new short [battlegroundSize];
    }

    int action;
    e_game_stage stage = BG_SELECTION;

    do {

        system("clear");

        inspectFleet(myFleet.ship);
        printButtleground(battleGround, battlegroundSize, pointer, newShip, stage);

        cout << endl << "action [" << action << "]" << endl;
        cout << endl << "new ship [" << newShip[0] << ":"<< newShip[1] << "]" << endl;

        cout << endl << "pointer [" << pointer[0]+1 << ":" << pointer[1]+1 << "]" << endl;

        cout << endl << "Controls:";

        cout << endl << "w, a, s, d - to movie";
        if(stage==BG_SELECTION) {
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
                switch (stage) {
                    case BG_SHOTING:
                        movePointer(pointer, action, battlegroundSize);
                    case BG_SELECTION:
                        moveShip(pointer, action, battlegroundSize, newShip);
                }
                break;
            case A_DO:
                break;
            case A_TURN:
                turnShip(pointer, battlegroundSize, newShip);
                break;
        }

    } while (action!=A_EXIT);

    cout << endl << "Press any key to exit..." << endl;

    mygetch();

    system("clear");

    return 0;
}