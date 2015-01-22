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
    CL_NEW=-4,
    CL_MISS,
    CL_DEAD,
    CL_EMPTY,
};

enum e_game_stage {
    BG_CHECK = -2,
    BG_AUTOSET = -1,
    BG_SELECTION = 0,
    BG_SHOTING = 1,
};

enum e_action {
    A_AUTOLOCATE = 108,
    A_UP=119,
    A_DOWN=115,
    A_LEFT=97,
    A_RIGHT=100,
    A_DO=32,
    A_EXIT=27,
    A_TURN=114,
    A_DELETE=120
};

struct s_new_ship {
    short id;
    short direction;
};

struct s_fleet {
    int ship[10][2];
    s_new_ship newShip;
    int pointer[2];
};

void printCell (short int cellstate, bool selected = false) {
    if(selected==true) {
        cout << "[";
    } else {
        cout << " ";
    }
    switch(cellstate){
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
            cout << cellstate;
            break;
    }
    if(selected==true) {
        cout << "]";
    } else {
        cout << " ";
    }
}

void printButtleground(short ** battleGround, int battlegroundSize, s_fleet & Fleet, short stage = BG_SHOTING){

    bool selected;

    cout << "  ";

    for(int j = 0; j<battlegroundSize; j++) {
        cout << setw(2) << " " << j+1;
    }

    cout << endl;

    for(int i = 0; i<battlegroundSize; i++) {
        cout << setw(2) << i+1 << " ";
        for(int j = 0; j<battlegroundSize; j++) {
            if((Fleet.pointer[0]==i) && (Fleet.pointer[1]==j)) {
                selected = true;
            } else if(stage == BG_SELECTION) {
                int newShipSize = Fleet.ship[Fleet.newShip.id][0];
                if((Fleet.newShip.direction==D_HORIZONTAL) && (Fleet.pointer[0] == i) && (j>=Fleet.pointer[1] && j<Fleet.pointer[1]+newShipSize)) {
                    selected = true;
                } else if(Fleet.newShip.direction==D_VERTIVAL && (Fleet.pointer[1] == j) && (i>=Fleet.pointer[0] && i<Fleet.pointer[0]+newShipSize)) {
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

    cout << "a="<< action << " mp=" << movePoint << " md=" << moveDirection << endl;
    cout << "["<< pointer[0] << ":" << pointer[1] << "]" << endl;

    pointer[movePoint] += moveDirection;
    if(pointer[movePoint] >= battlegroundSize) {
        pointer[movePoint] = 0;
    } else if(pointer[movePoint]<0) {
        pointer[movePoint] = battlegroundSize-1;
    }
    cout << "["<< pointer[0] << ":" << pointer[1] << "]" << endl;

}

void moveShip(s_fleet & Fleet, int action, int battlegroundSize) {

    int movePoint;
    int moveDirection;

    checkDirection(action, movePoint, moveDirection);

    int overflow = Fleet.pointer[movePoint] + moveDirection;

    int newShipSize = Fleet.ship[Fleet.newShip.id][0];


    if((Fleet.newShip.direction==movePoint) && ((overflow + newShipSize) > battlegroundSize)) {
        return;
    } else if(overflow<0 || overflow >= battlegroundSize) {
        return;
    } else {
        Fleet.pointer[movePoint] += moveDirection;
    }

}

void turnShip(s_fleet & Fleet, int battlegroundSize){
    int turnTo;
    switch(Fleet.newShip.direction) {
        case D_HORIZONTAL:
            turnTo = 0;
            break;
        case D_VERTIVAL:
            turnTo = 1;
            break;
    }

    int newShipSize = Fleet.ship[Fleet.newShip.id][0];

    if(Fleet.pointer[turnTo]+newShipSize<=battlegroundSize) {
        Fleet.newShip.direction = (Fleet.newShip.direction==D_HORIZONTAL) ? D_VERTIVAL : D_HORIZONTAL;
    }
}

void craftFleet(s_fleet & Fleet, int battlegroundSize, short ** battleGround){
    int types[4][2] = {
            {1,4},
            {2,3},
            {3,2},
            {4,1},
    };
    int typeIndex = 0;
    for(int i = 0; i<10; i++) {
        Fleet.ship[i][0] = types[typeIndex][0];
        Fleet.ship[i][1] = 0;
        types[typeIndex][1]--;
        if(types[typeIndex][1] < 1) {
            typeIndex++;
        }
    }

    Fleet.newShip.id = 0;
    Fleet.newShip.direction = D_HORIZONTAL;
    Fleet.pointer[0] = 0;
    Fleet.pointer[1] = 0;


    for (int i = 0; i<battlegroundSize; i++) {
        for (int j = 0; j<battlegroundSize; j++) {
            battleGround[i][j] = CL_EMPTY;
        }
    }
}

bool changeShip(s_fleet & Fleet){

    Fleet.newShip.id++;

    if(Fleet.newShip.id>=10) {
        return false;
    }

    Fleet.pointer[0] = 0;
    Fleet.pointer[1] = 0;

    return true;
}

bool placeShipToBG(s_fleet & Fleet, int battlegroundSize, short ** battleGround){
    int newShipSize = Fleet.ship[Fleet.newShip.id][0];
    int rightBoard, leftBoard, bow, feed;
    switch(Fleet.newShip.direction) {
        case D_HORIZONTAL:

            if(Fleet.pointer[1]+newShipSize>battlegroundSize) {
                return false;
            }

            bow = Fleet.pointer[1]+newShipSize;

            if(bow>battlegroundSize-1) {
                bow = battlegroundSize-1;
            }

            feed = Fleet.pointer[1]-1;

            if(feed<0) {
                feed = 0;
            }

            rightBoard = Fleet.pointer[0]+1;

            if(rightBoard>battlegroundSize-1) {
                rightBoard = battlegroundSize-1;
            }

            leftBoard = Fleet.pointer[0]-1;

            if(leftBoard<0) {
                leftBoard = 0;
            }

            for(int i = leftBoard; i<=rightBoard; i++) {
                for (int j = feed; j<=bow; j++) {
                    if(battleGround[i][j]>CL_EMPTY) {
                        return false;
                    }
                }
            }

            for(int i = Fleet.pointer[1]; i<Fleet.pointer[1]+newShipSize; i++ ){
                battleGround[Fleet.pointer[0]][i] = Fleet.newShip.id;
            }
            break;
        case D_VERTIVAL:
            if(Fleet.pointer[0]+newShipSize>battlegroundSize) {
                return false;
            }

            bow = Fleet.pointer[0]+newShipSize;

            if(bow>battlegroundSize-1) {
                bow = battlegroundSize-1;
            }

            feed = Fleet.pointer[0]-1;

            if(feed<0) {
                feed = 0;
            }

            rightBoard = Fleet.pointer[1]+1;

            if(rightBoard>battlegroundSize-1) {
                rightBoard = battlegroundSize-1;
            }

            leftBoard = Fleet.pointer[1]-1;

            if(leftBoard<0) {
                leftBoard = 0;
            }

            for(int i = feed; i<=bow; i++) {
                for (int j = leftBoard; j<=rightBoard; j++) {
                    if(battleGround[i][j]>CL_EMPTY) {
                        return false;
                    }
                }
            }

            for(int i = Fleet.pointer[0]; i<Fleet.pointer[0]+newShipSize; i++){
                battleGround[i][Fleet.pointer[1]] = Fleet.newShip.id;
            }
            break;
    }
    return true;
}

void setShip(s_fleet & Fleet, int battlegroundSize, short ** battleGround, e_game_stage & stage, bool autoSet = false){
    bool settingShip = false;
    bool shipSetted;
    int iterations = 0;

    if(autoSet==true) {
        while (settingShip == false){

            iterations++;

            if(iterations>100){
                craftFleet(Fleet, battlegroundSize, battleGround);
            }

            Fleet.newShip.direction = rand()%2;
            Fleet.pointer[0] = rand()%10;
            Fleet.pointer[1] = rand()%10;

            settingShip = placeShipToBG(Fleet, battlegroundSize, battleGround);

        }
    } else {
        settingShip = placeShipToBG(Fleet, battlegroundSize, battleGround);
        if(settingShip==false) {
            return;
        }
    }

    Fleet.newShip.direction = Fleet.pointer[0] = Fleet.pointer[1] = 0;

    shipSetted = changeShip(Fleet);

    if(shipSetted!=true) {
        stage = BG_SHOTING;
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

    s_fleet myFleet, PCFleet;

    /*
        init battleground
     */

    short ** battleGround = new short * [battlegroundSize];
    for (int i = 0; i<battlegroundSize; i++) {
        battleGround[i] = new short [battlegroundSize];
    }

    craftFleet(myFleet, battlegroundSize, battleGround);

    int action;
    e_game_stage stage = BG_SELECTION;

    do {

        system("clear");

        printButtleground(battleGround, battlegroundSize, myFleet, stage);
        inspectFleet(myFleet.ship);

        cout << endl << "action [" << action << "]" << endl;
        
        cout << endl << "new ship [" << myFleet.newShip.id << ":"<< myFleet.newShip.direction << "]" << endl;

        cout << endl << "pointer [" << myFleet.pointer[0]+1 << ":" << myFleet.pointer[1]+1 << "]" << endl;

        cout << endl << "Controls:";

        cout << endl << "w, a, s, d - to movie";
        if(stage==BG_SELECTION) {
            cout << endl << "l - to autolocate";
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
                        movePointer(myFleet.pointer, action, battlegroundSize);
                        break;
                    case BG_SELECTION:
                        moveShip(myFleet, action, battlegroundSize);
                        break;
                }
                break;
            case A_AUTOLOCATE:
                if(stage==BG_SELECTION){
                    setShip(myFleet, battlegroundSize, battleGround, stage, true);
                }
                break;
            case A_DO:
                setShip(myFleet, battlegroundSize, battleGround, stage);
                break;
            case A_TURN:
                turnShip(myFleet, battlegroundSize);
                break;
        }

    } while (action!=A_EXIT);

    cout << endl << "Press any key to exit..." << endl;

    mygetch();

    system("clear");

    return 0;
}