#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string.h>
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

enum e_shooters{
    SHOTER_PC,
    SHOTER_HUMAN
};

enum e_shot_direction {
     UNKNOWN = 0,
     NORTH = 1,
     SOUTH = 2,
     EAST = 3,
     WEST = 4
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
    short battlegroundSize;
    s_ship * ships;
    short shipsAllive;
    s_new_ship newShip;
    int pointer[2];
};

struct s_pc_memory {
    short * detectPoint;
    bool shipDetected;
    e_shot_direction direction;
    e_shot_direction tempDirection;
} pcMemory;

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

void printButtleground(s_fleet & Fleet, short stage = BG_SHOTING, bool fogOfWar = false){
    bool selected;
    cout << "  ";
    for(int j = 0; j<Fleet.battlegroundSize; j++) {
        cout << setw(2) << " " << j+1;
    }
    cout << endl;
    for(int i = 0; i<Fleet.battlegroundSize; i++) {
        cout << setw(2) << i+1 << " ";
        for(int j = 0; j<Fleet.battlegroundSize; j++) {
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


void movePointer(int pointer [], int action, short battlegroundSize) {
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

void moveShip(s_fleet & Fleet, int action) {
    int movePoint;
    int moveDirection;

    checkDirection(action, movePoint, moveDirection);

    int overflow = Fleet.pointer[movePoint] + moveDirection;
    int newShipSize = Fleet.ships[Fleet.newShip.id].size;

    if((Fleet.newShip.direction==movePoint) && ((overflow + newShipSize) > Fleet.battlegroundSize)) {
        return;
    } else if(overflow<0 || overflow >= Fleet.battlegroundSize) {
        return;
    } else {
        Fleet.pointer[movePoint] += moveDirection;
    }
}

void turnShip(s_fleet & Fleet){
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

    if(Fleet.pointer[turnTo]+newShipSize<=Fleet.battlegroundSize) {
        Fleet.newShip.direction = (Fleet.newShip.direction==D_HORIZONTAL) ? D_VERTIVAL : D_HORIZONTAL;
    }
}

void craftFleet(s_fleet & Fleet){
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

    for (int i = 0; i<Fleet.battlegroundSize; i++) {
        for (int j = 0; j<Fleet.battlegroundSize; j++) {
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

bool placeShipToBG(s_fleet & Fleet){
    int newShipSize = Fleet.ships[Fleet.newShip.id].size;
    s_surroundings surroundings;
    switch(Fleet.newShip.direction) {
        case D_HORIZONTAL:
            if(Fleet.pointer[1]+newShipSize>Fleet.battlegroundSize) {
                return false;
            }
            surroundings.SouthSide = Fleet.pointer[0]+1;
            surroundings.EastSide = Fleet.pointer[1]+newShipSize;
            break;
        case D_VERTIVAL:
            if(Fleet.pointer[0]+newShipSize>Fleet.battlegroundSize) {
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

    if(surroundings.SouthSide>Fleet.battlegroundSize-1) {
        surroundings.SouthSide = Fleet.battlegroundSize-1;
    }

    surroundings.WestSide = Fleet.pointer[1]-1;
    if(surroundings.WestSide<0) {
        surroundings.WestSide = 0;
    }

    if(surroundings.EastSide>Fleet.battlegroundSize-1) {
        surroundings.EastSide = Fleet.battlegroundSize-1;
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

bool setShip(s_fleet & Fleet, e_game_stage & stage, bool autoSet = false, bool changeStageOnSuccess = true){
    bool settingShip = false;
    bool shipSetted;
    int iterations = 0;
    if(autoSet==true) {
        while (settingShip == false){
            iterations++;
            if(iterations>100){
                craftFleet(Fleet);
            }
            Fleet.newShip.direction = rand()%2;
            Fleet.pointer[0] = rand()%10;
            Fleet.pointer[1] = rand()%10;
            settingShip = placeShipToBG(Fleet);
        }
    } else {
        settingShip = placeShipToBG(Fleet);
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
    }
    return totalAllive;
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
    Fleet.battlegroundSize = battlegroundSize;
    Fleet.battleGround = initBattleGround(battlegroundSize);
    craftFleet(Fleet);
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
    if(Fleet.ships[shipId].dead == Fleet.ships[shipId].size){
       surroundShip(Fleet, shipId);
       Fleet.shipsAllive--;
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

void inverseDirection(e_shot_direction & direction) {
    switch(direction){
        case NORTH:
            direction = SOUTH;
            break;
        case SOUTH:
            direction = NORTH;
            break;
        case EAST:
            direction = WEST;
            break;
        case WEST:
            direction = EAST;
            break;

    }
}

bool PCMovePointer(s_fleet & Fleet, e_shot_direction direction){
cout << endl<< "move" <<direction;
    short point;
    short i, j;
    if(direction==NORTH || direction==SOUTH) {
        point = 0;
    } else if(direction==EAST || direction==WEST) {
        point = 1;
    }

    if(direction==SOUTH || direction==EAST) {
        Fleet.pointer[point]++;
        if(Fleet.pointer[point]>=Fleet.battlegroundSize) {
        cout << "break on 474";
            return false;
        }
    } else if(direction==NORTH || direction==WEST) {
        Fleet.pointer[point]--;
        if(Fleet.pointer[point]<0) {
        cout << "break on 480";
            return false;
        }
    }
    i = Fleet.pointer[0];
    j = Fleet.pointer[1];
    if(Fleet.battleGround[i][j]<CL_EMPTY) {
    cout << "break on 487";
        return false;
    } else {
    cout << "move";
        return true;
    }
}

bool returnPointerToDetectPoint(s_fleet & Fleet, s_pc_memory & pcMemory){

    Fleet.pointer[0] = pcMemory.detectPoint[0];
    Fleet.pointer[1] = pcMemory.detectPoint[1];

}

bool PCShoot(s_fleet & Fleet, s_pc_memory & pcMemory){
    int i, j;
    bool shipShoted, pointerMoved;
    short shipsAllive = Fleet.shipsAllive;
    e_shot_direction shotDirectionsList [4] = {NORTH, SOUTH, EAST, WEST};

    if(pcMemory.shipDetected == false){

        do{
            i = rand()%10;
            j = rand()%10;
        } while(Fleet.battleGround[i][j]<CL_EMPTY);

        Fleet.pointer[0] = i;
        Fleet.pointer[1] = j;

    } else {

        if(pcMemory.direction==UNKNOWN) {
            do{
                returnPointerToDetectPoint(Fleet, pcMemory);
                pcMemory.tempDirection = shotDirectionsList[rand()%4];
                pointerMoved = PCMovePointer(Fleet, pcMemory.tempDirection);
                i = Fleet.pointer[0];
                j = Fleet.pointer[1];
    //            cout << "[" << pcMemory.detectPoint[0]+1 << ":" << pcMemory.detectPoint[1]+1<< "] ";
  //              cout << "[" << i+1 << ":" << j+1 << "] " << Fleet.battleGround[i][j] << "<" << CL_EMPTY <<endl;
//                cout <<"moved=" << pointerMoved <<endl;
      //          mygetch();
            } while(pointerMoved!=true);
        } else {
            pointerMoved = PCMovePointer(Fleet, pcMemory.direction);
            if(pointerMoved==false){

                returnPointerToDetectPoint(Fleet, pcMemory);
                inverseDirection(pcMemory.direction);
                PCMovePointer(Fleet, pcMemory.direction);
            }
        }
    }
//i = Fleet.pointer[0];
  //              j = Fleet.pointer[1];
//                cout <<"Shot to " << "[" << i+1 << ":" << j+1 << "] " << endl;
    shipShoted = shot(Fleet);

    if(shipsAllive != Fleet.shipsAllive){
        pcMemory.shipDetected = false;
        pcMemory.direction = UNKNOWN;
    } else if(shipShoted == true) {
        if(pcMemory.shipDetected == false) {
            pcMemory.shipDetected = true;
            pcMemory.detectPoint[0] = i;
            pcMemory.detectPoint[1] = j;
        } else if(pcMemory.shipDetected == true && pcMemory.direction==UNKNOWN) {
            pcMemory.direction = pcMemory.tempDirection;
        }
    } else if(shipShoted == false) {
        if(pcMemory.shipDetected == true) {
            returnPointerToDetectPoint(Fleet, pcMemory);
            inverseDirection(pcMemory.direction);
            PCMovePointer(Fleet, pcMemory.direction);
        }
    }
    //mygetch();
    return shipShoted;
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

void changeShooter(e_shooters & currentShooter){
     currentShooter = (currentShooter==SHOTER_HUMAN) ? SHOTER_PC : SHOTER_HUMAN;
}

int main(){
    srand(time(NULL));
    system("clear");

    int battlegroundSize = 10;
    int action;
    short shipsAllive;
    bool shotSucces;
    char whoWins[255];

    s_fleet myFleet, PCFleet;
    e_game_stage stage = BG_SELECTION;
    e_shooters currentShooter;

    myFleet = initFleet(battlegroundSize);

    do {
        system("clear");

        cout << "Shooter " << currentShooter<<endl;

        printButtleground(myFleet, stage);

        shipsAllive = inspectFleet(myFleet.ships, stage);
        if(shipsAllive==0) {
            strcpy(whoWins, "PC");
        }

        if(stage == BG_SHOTING || stage == BG_END_GAME) {
            printButtleground(PCFleet, stage);
            shipsAllive = inspectFleet(PCFleet.ships, stage);
            if(shipsAllive==0) {
                strcpy(whoWins, "HUMAN");
            }
            //            printButtleground(PCFleet, stage, true);

        }
        if(stage == BG_END_GAME) {
            cout << "END GAME " << whoWins << " WINS!" << endl;
        } else  if(currentShooter == SHOTER_PC) {

            shotSucces = PCShoot(myFleet, pcMemory);
//            if(shotSucces == false) {
//                changeShooter(currentShooter);
//            }
            usleep(500000);
            continue;
        }

        printControls(stage);

        action = mygetch();

        if(stage==BG_CONFIRM) {
            switch (action) {
                case A_YES:
                    stage = BG_SHOTING;
                    myFleet.pointer[0]=myFleet.pointer[1]=-1;
                    PCFleet = initFleet(battlegroundSize);
                    while (setShip(PCFleet, stage, true, false));
                    currentShooter = SHOTER_HUMAN;
                    pcMemory.detectPoint = new short [2];
                    pcMemory.shipDetected = false;
                    pcMemory.direction = UNKNOWN;
                    break;
                case A_NO:
                    stage = BG_SELECTION;
                    craftFleet(myFleet);
                    break;
                default:
                    continue;
            }
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
                        moveShip(myFleet, action);
                        break;
                }
                break;
            case A_AUTOLOCATE:
                switch (stage) {
                    case BG_SELECTION:
                        setShip(myFleet, stage, true);
                        break;
                }
                break;
            case A_DO:
                switch (stage) {
                    case BG_SHOTING:
                        shotSucces = shot(PCFleet);
                        if(shotSucces == false) {
                            changeShooter(currentShooter);
                        }
                        break;
                    case BG_SELECTION:
                        setShip(myFleet, stage);
                        break;
                }
                break;
            case A_TURN:
                switch (stage) {
                    case BG_SELECTION:
                        turnShip(myFleet);
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
