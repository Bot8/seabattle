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
    BG_SELECTION,
    BG_CONFIRM,
    BG_PC_AUTOSET,
    BG_SHOTING,
    BG_END_GAME,
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
    A_YES=121,
    A_NO=110
};

struct s_surroundings {
    short NorthSide;
    short SouthSide;
    short EastSide;
    short WestSide;
};

struct s_new_ship {
    short id;
    short direction;
};

struct s_ship {
    short size;
    short dead;
    s_surroundings surroundings;
};

struct s_fleet {
    short ** battleGround;
    s_ship * ships;
    s_new_ship newShip;
    int pointer[2];
};

void printCell (short int cellstate, bool selected, bool fogOfWar) {
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
            //cout << cellstate;
            if(fogOfWar==true) {
                cout << " ";
            } else {
                cout << "#";
            }
            break;
    }
    if(selected==true) {
        cout << "]";
    } else {
        cout << " ";
    }
}

void printButtleground(int battlegroundSize, s_fleet & Fleet, short stage = BG_SHOTING, bool fogOfWar = false){
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
                int newShipSize = Fleet.ships[Fleet.newShip.id].size;
                if((Fleet.newShip.direction==D_HORIZONTAL) && (Fleet.pointer[0] == i) && (j>=Fleet.pointer[1] && j<Fleet.pointer[1]+newShipSize)) {
                    selected = true;
                } else if((Fleet.newShip.direction==D_VERTIVAL) && (Fleet.pointer[1] == j) && (i>=Fleet.pointer[0] && i<Fleet.pointer[0]+newShipSize)) {
                    selected = true;
                } else {
                    selected = false;
                }
            } else {
                selected = false;
            }
            printCell(Fleet.battleGround[i][j], selected, fogOfWar);
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

void moveShip(s_fleet & Fleet, int action, int battlegroundSize) {
    int movePoint;
    int moveDirection;

    checkDirection(action, movePoint, moveDirection);

    int overflow = Fleet.pointer[movePoint] + moveDirection;
    int newShipSize = Fleet.ships[Fleet.newShip.id].size;

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
    int newShipSize = Fleet.ships[Fleet.newShip.id].size;

    switch(Fleet.newShip.direction) {
        case D_HORIZONTAL:
            turnTo = 0;
            break;
        case D_VERTIVAL:
            turnTo = 1;
            break;
    }

    if(Fleet.pointer[turnTo]+newShipSize<=battlegroundSize) {
        Fleet.newShip.direction = (Fleet.newShip.direction==D_HORIZONTAL) ? D_VERTIVAL : D_HORIZONTAL;
    }
}

void craftFleet(s_fleet & Fleet, int battlegroundSize){
    int types[4][2] = {
            {4,1},
            {3,2},
            {2,3},
            {1,4}
    };

    Fleet.ships = new s_ship [10];

    int typeIndex = 0;
    for(int i = 0; i<10; i++) {
        Fleet.ships[i].size = types[typeIndex][0];
        Fleet.ships[i].dead = 0;
        types[typeIndex][1]--;
        if(types[typeIndex][1] < 1) {
            typeIndex++;
        }
    }

    Fleet.newShip.id = 0;
    Fleet.newShip.direction = D_HORIZONTAL;
    Fleet.pointer[0] = Fleet.pointer[1] = 0;

    for (int i = 0; i<battlegroundSize; i++) {
        for (int j = 0; j<battlegroundSize; j++) {
            Fleet.battleGround[i][j] = CL_EMPTY;
        }
    }
}

bool changeShip(s_fleet & Fleet){

    Fleet.newShip.id++;

    if(Fleet.newShip.id>=10) {
        return false;
    }

    Fleet.pointer[0] = Fleet.pointer[1] = 0;

    return true;
}

bool placeShipToBG(s_fleet & Fleet, int battlegroundSize){
    int newShipSize = Fleet.ships[Fleet.newShip.id].size;
    s_surroundings surroundings;
    switch(Fleet.newShip.direction) {
        case D_HORIZONTAL:
            if(Fleet.pointer[1]+newShipSize>battlegroundSize) {
                return false;
            }
            surroundings.SouthSide = Fleet.pointer[0]+1;
            surroundings.EastSide = Fleet.pointer[1]+newShipSize;
            break;
        case D_VERTIVAL:
            if(Fleet.pointer[0]+newShipSize>battlegroundSize) {
                return false;
            }
            surroundings.SouthSide = Fleet.pointer[0]+newShipSize;
            surroundings.EastSide = Fleet.pointer[1]+1;
            break;
    }

    surroundings.NorthSide = Fleet.pointer[0]-1;
    if(surroundings.NorthSide<0) {
        surroundings.NorthSide = 0;
    }

    if(surroundings.SouthSide>battlegroundSize-1) {
        surroundings.SouthSide = battlegroundSize-1;
    }

    surroundings.WestSide = Fleet.pointer[1]-1;
    if(surroundings.WestSide<0) {
        surroundings.WestSide = 0;
    }

    if(surroundings.EastSide>battlegroundSize-1) {
        surroundings.EastSide = battlegroundSize-1;
    }

    for(int i = surroundings.NorthSide; i<=surroundings.SouthSide; i++) {
        for (int j = surroundings.WestSide; j<=surroundings.EastSide; j++) {
            if(Fleet.battleGround[i][j]>CL_EMPTY) {
                return false;
            }
        }
    }

    switch(Fleet.newShip.direction) {
        case D_HORIZONTAL:
            for(int i = Fleet.pointer[1]; i<Fleet.pointer[1]+newShipSize; i++ ){
                Fleet.battleGround[Fleet.pointer[0]][i] = Fleet.newShip.id;
            }
            break;
        case D_VERTIVAL:
            for(int i = Fleet.pointer[0]; i<Fleet.pointer[0]+newShipSize; i++){
                Fleet.battleGround[i][Fleet.pointer[1]] = Fleet.newShip.id;
            }
            break;
    }

    Fleet.ships[Fleet.newShip.id].surroundings = surroundings;

    return true;
}

bool setShip(s_fleet & Fleet, int battlegroundSize, e_game_stage & stage, bool autoSet = false, bool changeStageOnSuccess = true){
    bool settingShip = false;
    bool shipSetted;
    int iterations = 0;
    if(autoSet==true) {
        while (settingShip == false){
            iterations++;
            if(iterations>100){
                craftFleet(Fleet, battlegroundSize);
            }
            Fleet.newShip.direction = rand()%2;
            Fleet.pointer[0] = rand()%10;
            Fleet.pointer[1] = rand()%10;
            settingShip = placeShipToBG(Fleet, battlegroundSize);
        }
    } else {
        settingShip = placeShipToBG(Fleet, battlegroundSize);
        if(settingShip==false) {
            return true;
        }
    }
    Fleet.newShip.direction = Fleet.pointer[0] = Fleet.pointer[1] = 0;
    shipSetted = changeShip(Fleet);
    if(shipSetted==false && changeStageOnSuccess == true) {
        stage = BG_CONFIRM;
    }
    return shipSetted;
}

void printShipType(int shipSize, int countShipType) {
    cout << countShipType << " ";
    for(int i = 0; i<shipSize; i++) {
        cout << "#";
    }
    cout << endl;
}

bool inspectFleet(s_ship * ships, e_game_stage & stage){
    int shipSize = ships[0].size;
    int countShipType = 0;
    int totalAllive = 0;
    for (int i = 0; i<10; i++) {
        if(ships[i].size!=shipSize) {
            printShipType(shipSize, countShipType);
            shipSize = ships[i].size;
            countShipType = 0;
        }
        if(ships[i].size!=ships[i].dead){
            countShipType++;
            totalAllive++;
        }
    }
    printShipType(shipSize, countShipType);
    cout << "Totla Allive " << totalAllive << " ships" << endl;
    if(totalAllive==0) {
        stage = BG_END_GAME;
        return false;
    } else {
        return true;
    }
}

short ** initBattleGround(int battlegroundSize){
    short  **battleGround = new short * [battlegroundSize];
    for (int i = 0; i<battlegroundSize; i++) {
        battleGround[i] = new short [battlegroundSize];
    }
    return battleGround;
}

s_fleet initFleet(int battlegroundSize){
    s_fleet Fleet;
    Fleet.battleGround = initBattleGround(battlegroundSize);
    craftFleet(Fleet, battlegroundSize);
    return Fleet;
}

void surroundShip(s_fleet & Fleet, short shipId) {
    for(int i = Fleet.ships[shipId].surroundings.NorthSide; i<=Fleet.ships[shipId].surroundings.SouthSide; i++) {
        for (int j = Fleet.ships[shipId].surroundings.WestSide; j<=Fleet.ships[shipId].surroundings.EastSide; j++) {
            if(Fleet.battleGround[i][j]==CL_EMPTY) {
                Fleet.battleGround[i][j]=CL_MISS;
            }
        }
    }
}

void destrouShipDeck(s_fleet & Fleet, short col, short row){
    short shipId = Fleet.battleGround[col][row];
    Fleet.ships[shipId].dead++;
    if(Fleet.ships[shipId].dead ==  Fleet.ships[shipId].size){
       surroundShip(Fleet, shipId);
    }
    Fleet.battleGround[col][row] = CL_DEAD;
}

bool shot(s_fleet & Fleet) {
    int i = Fleet.pointer[0];
    int j = Fleet.pointer[1];
    if(Fleet.battleGround[i][j] >= 0) {
        destrouShipDeck(Fleet, i, j);
        return true;
    }
    switch (Fleet.battleGround[i][j]) {
        case CL_EMPTY:
            Fleet.battleGround[i][j] = CL_MISS;
            break;
        case CL_MISS:
        case CL_DEAD:
            break;
    }
    return false;
}

void printControls(e_game_stage stage){
    cout << endl << "esc - to exit";
    if(stage==BG_CONFIRM){
        cout << endl << "Ship setted. Confirm? [y/n]";
        return;
    }
    cout << endl << "Controls:";
    cout << endl << "w, a, s, d - to movie";
    if(stage==BG_SELECTION) {
        cout << endl << "l - to autolocate";
        cout << endl << "r - to turn ship";
    }
    cout << endl << "space - to action";
}

int main(){
    srand(time(NULL));
    system("clear");

    int battlegroundSize = 10;
    int action;
    bool shipsInspector;
    char * whoWins;
    s_fleet myFleet, PCFleet;
    e_game_stage stage = BG_SELECTION;

    myFleet = initFleet(battlegroundSize);

    do {
        system("clear");

        printButtleground(battlegroundSize, myFleet, stage);

        shipsInspector = inspectFleet(myFleet.ships, stage);
        if(shipsInspector==false) {
            whoWins = "PC";
        }

        if(stage == BG_SHOTING) {
            printButtleground(battlegroundSize, PCFleet, stage);
            shipsInspector = inspectFleet(PCFleet.ships, stage);
            if(shipsInspector==false) {
                whoWins = "HUMAN";
            }
            //            printButtleground(battlegroundSize, PCFleet, stage, true);

        }

        printControls(stage);

        action = mygetch();

        if(stage==BG_CONFIRM) {
            switch (action) {
                case A_YES:
                    stage = BG_SHOTING;
                    myFleet.pointer[0]=myFleet.pointer[1]=-1;
                    PCFleet = initFleet(battlegroundSize);
                    while (setShip(PCFleet, battlegroundSize, stage, true, false));
                    break;
                case A_NO:
                    stage = BG_SELECTION;
                    craftFleet(myFleet, battlegroundSize);
                    break;
                default:
                    continue;
            }
        }
        if(stage == BG_END_GAME) {
            cout << "END GAME " << whoWins << " WINS!" << endl;
        }
        switch (action) {
            case A_UP:
            case A_DOWN:
            case A_LEFT:
            case A_RIGHT:
                switch (stage) {
                    case BG_SHOTING:
                        movePointer(PCFleet.pointer, action, battlegroundSize);
                        break;
                    case BG_SELECTION:
                        moveShip(myFleet, action, battlegroundSize);
                        break;
                }
                break;
            case A_AUTOLOCATE:
                switch (stage) {
                    case BG_SELECTION:
                        setShip(myFleet, battlegroundSize, stage, true);
                        break;
                }
                break;
            case A_DO:
                switch (stage) {
                    case BG_SHOTING:
                        shot(PCFleet);
                        break;
                    case BG_SELECTION:
                        setShip(myFleet, battlegroundSize, stage);
                        break;
                }
                break;
            case A_TURN:
                switch (stage) {
                    case BG_SELECTION:
                        turnShip(myFleet, battlegroundSize);
                        break;
                }
                break;
        }

    } while (action!=A_EXIT);

    cout << endl << "Press any key to exit..." << endl;
    mygetch();
    system("clear");
    return 0;
}
