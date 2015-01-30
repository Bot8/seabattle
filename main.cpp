#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string.h>
#include "c_string.h"

using namespace std;

/*
    ship directions aliaces
*/

enum e_directions {
    D_VERTIVAL = 0,
    D_HORIZONTAL = 1,
    D_FORWARD = 1,
    D_BACKWARD = -1
};

/*
    cell states
*/

enum e_cell {
    CL_MISS = -3,
    CL_DEAD,
    CL_EMPTY,
};

/*
    game stages aliaces
    BG_SELECTION - i am setting ships
    BG_CONFIRM - confirm ships disposition
    BG_PC_AUTOSET - PC seting ships
    BG_SHOTING
    BG_END_GAME
*/

enum e_game_stage {
    BG_SELECTION,
    BG_CONFIRM,
    BG_PC_AUTOSET,
    BG_SHOTING,
    BG_END_GAME,
};

/*
    key control
*/

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

/*
    shot direction aliaces for PC mind
*/

enum e_shot_direction {
     UNKNOWN = 0,
     NORTH = 1,
     SOUTH = 2,
     EAST = 3,
     WEST = 4
};

/*
    ship suroundigs
    needs to surround ship with dots when ship is dead
*/

struct s_surroundings {
    short NorthSide;
    short SouthSide;
    short EastSide;
    short WestSide;
};

/*
    new ship options
    id - from array of ships
    direction - hertical or horizontal
*/

struct s_new_ship {
    unsigned short id:4;
    short direction;
};

/*
    setted ship options
    size
    dead - num of cille decks
    struct s_surroundings descr. on line 83
*/

struct s_ship {
    unsigned short size:4;
    unsigned short dead:4;
    s_surroundings surroundings;
};

/*
    battleGround - battleGround maitrix
    battlegroundSize - size of matrix side
    ships - array of ships
    shipsAllive
    newShip - helper to set the new ship
    pointer - current pointer position on battleGround

*/

struct s_fleet {
    short ** battleGround;
    unsigned short battlegroundSize:4;
    s_ship * ships;
    unsigned short shipsAllive:4;
    s_new_ship newShip;
    short pointer[2];
};

/*
    detectPoint - pointe where firs shoted to the ship deck
    shipDetected - flag of detecting new ship
    direction - direction where to move to kill detected ship
    tempDirection - needs to found right direction
*/

struct s_pc_memory {
    short * detectPoint;
    bool shipDetected;
    e_shot_direction direction;
    e_shot_direction tempDirection;
} pcMemory;

/*
    prints cell depends on cellstate
    selected  - flag to surrond cell with []
    fogOfWar - hiding ships
*/

void printCell (short cellstate, bool selected, bool fogOfWar) {
    if(selected==true) {
        cout << "[";
    } else {
        cout << " ";
    }
    switch(cellstate){
        case CL_EMPTY:
            cout << " ";
            break;
        case CL_DEAD:
            cout << "x";
            break;
        case CL_MISS:
            cout << "*";
            break;
        default:
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
/*
    print current Buttleground
    fogOfWar - hiding ships
*/
void printButtleground(s_fleet & Fleet, short stage = BG_SHOTING, bool fogOfWar = false){
    bool selected;
    cout << "  ";
    /*
        print horizontal coordinates on the top of the Buttleground
    */
    for(int j = 0; j<Fleet.battlegroundSize; j++) {
        cout << setw(2) << " " << j+1;
    }
    cout << endl;

    for(int i = 0; i<Fleet.battlegroundSize; i++) {
        /*
            print vertical coordinates on the left side of the Buttleground
        */
        cout << setw(2) << i+1 << " ";
        for(int j = 0; j<Fleet.battlegroundSize; j++) {
            if((Fleet.pointer[0]==i) && (Fleet.pointer[1]==j)) {
                /*
                    pointer is on current coordinate
                */
                selected = true;
            } else if(stage == BG_SELECTION) {
                /*
                    print new ship shema (where we can set it)
                */
                short newShipSize = Fleet.ships[Fleet.newShip.id].size;
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
/*
    checking direction where to move user pointer depends on user action
*/
void checkDirection(short action, short & movePoint, short & moveDirection) {
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

/*
    move user pointer depends on user action
*/

void movePointer(short pointer [], short action, short battlegroundSize) {
    short movePoint;
    short moveDirection;

    checkDirection(action, movePoint, moveDirection);
    pointer[movePoint] += moveDirection;

    /*
        infinity move
    */
    if(pointer[movePoint] >= battlegroundSize) {
        pointer[movePoint] = 0;
    } else if(pointer[movePoint]<0) {
        pointer[movePoint] = battlegroundSize-1;
    }
}

/*
    move new ship
*/

void moveShip(s_fleet & Fleet, short action) {
    short movePoint;
    short moveDirection;

    checkDirection(action, movePoint, moveDirection);

    /*
        new pointer position
    */
    short overflow = Fleet.pointer[movePoint] + moveDirection;

    short newShipSize = Fleet.ships[Fleet.newShip.id].size;

    if((Fleet.newShip.direction==movePoint) && ((overflow + newShipSize) > Fleet.battlegroundSize)) {
        /*
            new ship out of Battleground
        */
        return;
    } else if(overflow<0 || overflow >= Fleet.battlegroundSize) {
        /*
            pointer out of Battleground
        */
        return;
    } else {
        /*
          move succes
        */
        Fleet.pointer[movePoint] += moveDirection;
    }
}

void turnShip(s_fleet & Fleet){
    short turnTo;
    short newShipSize = Fleet.ships[Fleet.newShip.id].size;

    switch(Fleet.newShip.direction) {
        case D_HORIZONTAL:
            turnTo = 0;
            break;
        case D_VERTIVAL:
            turnTo = 1;
            break;
    }

    /*
        new ship out of Battleground
    */

    if(Fleet.pointer[turnTo]+newShipSize<=Fleet.battlegroundSize) {
        Fleet.newShip.direction = (Fleet.newShip.direction==D_HORIZONTAL) ? D_VERTIVAL : D_HORIZONTAL;
    }
}

/*
    init fleet on the Battleground
*/

void craftFleet(s_fleet & Fleet){
    /*
        ship types
        {ship_size, ships_count}
    */
    short types[4][2] = {
            {4,1},
            {3,2},
            {2,3},
            {1,4}
    };
    /*
        array of ships
    */
    Fleet.ships = new s_ship [10];

    short typeIndex = 0;
    for(int i = 0; i<10; i++) {
        Fleet.ships[i].size = types[typeIndex][0];
        Fleet.ships[i].dead = 0;
        /*
            reduce the amount of ships of current type
        */
        types[typeIndex][1]--;
        if(types[typeIndex][1] < 1) {
            /*
                change ship type
            */
            typeIndex++;
        }
    }

    /*
        init state for new ship setter
    */
    Fleet.newShip.id = 0;
    Fleet.newShip.direction = D_HORIZONTAL;
    Fleet.pointer[0] = Fleet.pointer[1] = 0;

    /*
        fill new battleground with empty cells
        !!!not optimized!!!
    */

    for (int i = 0; i<Fleet.battlegroundSize; i++) {
        for (int j = 0; j<Fleet.battlegroundSize; j++) {
            Fleet.battleGround[i][j] = CL_EMPTY;
        }
    }
}

/*
   move new ship setter to new ship type
*/

bool changeShip(s_fleet & Fleet){

    Fleet.newShip.id++;

    if(Fleet.newShip.id>=10) {
        /*
          all the ships are setted
        */
        return false;
    }

    Fleet.pointer[0] = Fleet.pointer[1] = 0;

    return true;
}

bool placeShipToBG(s_fleet & Fleet){
    short newShipSize = Fleet.ships[Fleet.newShip.id].size;
    /*
        ship outlines
    */
    s_surroundings surroundings;
    switch(Fleet.newShip.direction) {
        case D_HORIZONTAL:
            if(Fleet.pointer[1]+newShipSize>Fleet.battlegroundSize) {
                /*
                    new ship overflows Battleground
                */
                return false;
            }
            surroundings.SouthSide = Fleet.pointer[0]+1;
            surroundings.EastSide = Fleet.pointer[1]+newShipSize;
            break;
        case D_VERTIVAL:
            if(Fleet.pointer[0]+newShipSize>Fleet.battlegroundSize) {
                /*
                    new ship overflows Battleground
                */
                return false;
            }
            surroundings.SouthSide = Fleet.pointer[0]+newShipSize;
            surroundings.EastSide = Fleet.pointer[1]+1;
            break;
    }

    /*
        do not overflow Battleground
    */

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


    /*
        do not set ship to allready setted ship or near
    */
    for(short i = surroundings.NorthSide; i<=surroundings.SouthSide; i++) {
        for (short j = surroundings.WestSide; j<=surroundings.EastSide; j++) {
            if(Fleet.battleGround[i][j]>CL_EMPTY) {
                return false;
            }
        }
    }

    /*
        set new ship
    */

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

    /*
        ship outlines
    */
    Fleet.ships[Fleet.newShip.id].surroundings = surroundings;

    return true;
}

/*
    set the ship
*/
bool setShip(s_fleet & Fleet, e_game_stage & stage, bool autoSet = false, bool changeStageOnSuccess = true){
    /*
      settingShip - current set ship try of autosettter
      shipSetted - global flag what ship setted
      iterations - count autosetter tryes
    */
    bool settingShip = false;
    bool shipSetted;
    short iterations = 0;

    /*
      try to autoset the ship
    */
    if(autoSet==true) {
        while (settingShip == false){
            iterations++;
            if(iterations>100){
                /*
                    can not place ship to Battleground
                    reset battleground
                    !!not optimized!!
                */
                craftFleet(Fleet);
            }
            /*
                random direction
            */
            Fleet.newShip.direction = rand()%2;
            /*
                random position
            */
            Fleet.pointer[0] = rand()%10;
            Fleet.pointer[1] = rand()%10;
            /*
                try to set
            */
            settingShip = placeShipToBG(Fleet);
        }
    } else {
        settingShip = placeShipToBG(Fleet);
        if(settingShip==false) {
            /*
                cannot set the ship
            */
            return true;
        }
    }

    /*
        reset new ship options
    */
    Fleet.newShip.direction = Fleet.pointer[0] = Fleet.pointer[1] = 0;
    /*
        chose new ship
    */
    shipSetted = changeShip(Fleet);

    if(shipSetted==false && changeStageOnSuccess == true) {
        /*
            all ship are setted, change game stage
        */
        stage = BG_CONFIRM;
    }
    return shipSetted;
}

void printShipType(int shipSize, int countShipType) {
    /*
      ship legenda item
    */
    cout << countShipType << " ";
    for(int i = 0; i<shipSize; i++) {
        cout << "#";
    }
    cout << endl;
}

bool inspectFleet(s_ship * ships, e_game_stage & stage){
    short shipSize = ships[0].size;
    short countShipType = 0;
    short totalAllive = 0;
    for (int i = 0; i<10; i++) {
        if(ships[i].size!=shipSize) {
            /*
                ships legenda
            */
            printShipType(shipSize, countShipType);
            shipSize = ships[i].size;
            countShipType = 0;
        }
        if(ships[i].size!=ships[i].dead){
            /*
                Allive ships of current type
            */
            countShipType++;
            /*
               total Allive ships
            */
            totalAllive++;
        }
    }
    printShipType(shipSize, countShipType);
    cout << "Totla Allive " << totalAllive << " ships" << endl;
    if(totalAllive==0) {
        /*
            all ships are dead
        */
        stage = BG_END_GAME;
    }
    return totalAllive;
}

short ** initBattleGround(short battlegroundSize){
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

/*
    surround dead ship with poins
*/

void surroundShip(s_fleet & Fleet, short shipId) {
    for(int i = Fleet.ships[shipId].surroundings.NorthSide; i<=Fleet.ships[shipId].surroundings.SouthSide; i++) {
        for (int j = Fleet.ships[shipId].surroundings.WestSide; j<=Fleet.ships[shipId].surroundings.EastSide; j++) {
            if(Fleet.battleGround[i][j]==CL_EMPTY) {
                Fleet.battleGround[i][j]=CL_MISS;
            }
        }
    }
}

/*
    shoot to ship deck on [col:row] position
*/

void destrouShipDeck(s_fleet & Fleet, short col, short row){
    short shipId = Fleet.battleGround[col][row];
    Fleet.ships[shipId].dead++;
    if(Fleet.ships[shipId].dead == Fleet.ships[shipId].size){
        /*
            ship are dead
        */
       surroundShip(Fleet, shipId);
       Fleet.shipsAllive--;
    }
    Fleet.battleGround[col][row] = CL_DEAD;
}

/*
    shoot to current pointer
*/

bool shot(s_fleet & Fleet) {
    short i = Fleet.pointer[0];
    short j = Fleet.pointer[1];
    if(Fleet.battleGround[i][j] >= 0) {
        /*
            hit
        */
        destrouShipDeck(Fleet, i, j);
        return true;
    }
    /*
        miss
    */
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

/*
    inverce PC shooting direction
*/

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

/*
    inverce PC move pionte on current direction
*/

bool PCMovePointer(s_fleet & Fleet, e_shot_direction direction){
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
            /*
                out of Battleground
            */
            return false;
        }
    } else if(direction==NORTH || direction==WEST) {
        Fleet.pointer[point]--;
        if(Fleet.pointer[point]<0) {
            /*
                out of Battleground
            */
            return false;
        }
    }
    i = Fleet.pointer[0];
    j = Fleet.pointer[1];
    if(Fleet.battleGround[i][j]<CL_EMPTY) {
        /*
            PC dont shoot to empty or dead cells
        */
        return false;
    } else {
        return true;
    }
}

/*
    return pionter to point where enemys ship was detected
*/

bool returnPointerToDetectPoint(s_fleet & Fleet, s_pc_memory & pcMemory){
    Fleet.pointer[0] = pcMemory.detectPoint[0];
    Fleet.pointer[1] = pcMemory.detectPoint[1];
}

bool PCShoot(s_fleet & Fleet, s_pc_memory & pcMemory){
    short i, j;
    bool shipShoted, pointerMoved;
    short shipsAllive = Fleet.shipsAllive;
    e_shot_direction shotDirectionsList [4] = {NORTH, SOUTH, EAST, WEST};

    if(pcMemory.shipDetected == false){
        /*
            randomly searching for the ship
            !!not optimized, we can use some strategy!!
        */
        do{
            i = rand()%10;
            j = rand()%10;
        } while(Fleet.battleGround[i][j]<CL_EMPTY);

        Fleet.pointer[0] = i;
        Fleet.pointer[1] = j;

    } else {
        if(pcMemory.direction==UNKNOWN) {
            /*
                searching for ship direction
            */
            do{
                returnPointerToDetectPoint(Fleet, pcMemory);
                pcMemory.tempDirection = shotDirectionsList[rand()%4];
                pointerMoved = PCMovePointer(Fleet, pcMemory.tempDirection);
                i = Fleet.pointer[0];
                j = Fleet.pointer[1];
            } while(pointerMoved!=true);
        } else {
            /*
                destrroyng ship on current direction
            */
            pointerMoved = PCMovePointer(Fleet, pcMemory.direction);
            if(pointerMoved==false){
                /*
                   reached the and of battleground, inverce direction
                */
                returnPointerToDetectPoint(Fleet, pcMemory);
                inverseDirection(pcMemory.direction);
                PCMovePointer(Fleet, pcMemory.direction);
            }
        }
    }

    shipShoted = shot(Fleet);

    if(shipsAllive != Fleet.shipsAllive){
        /*
            ship is dead, lets find anothe one
        */
        pcMemory.shipDetected = false;
        pcMemory.direction = UNKNOWN;
    } else if(shipShoted == true) {
        if(pcMemory.shipDetected == false) {
            /*
                new ship found
            */
            pcMemory.shipDetected = true;
            pcMemory.detectPoint[0] = i;
            pcMemory.detectPoint[1] = j;
        } else if(pcMemory.shipDetected == true && pcMemory.direction==UNKNOWN) {
            /*
                detected ship direction found
            */
            pcMemory.direction = pcMemory.tempDirection;
        }
    } else if(shipShoted == false) {
        if(pcMemory.shipDetected == true) {
            /*
                reached the end of ship, but ship is not dead. Inverce direction
            */
            returnPointerToDetectPoint(Fleet, pcMemory);
            inverseDirection(pcMemory.direction);
        }
    }
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

void destroyFleet(s_fleet & Fleet) {
    delete [] Fleet.ships;

    for (int i = 0; i<Fleet.battlegroundSize; i++) {
        delete [] Fleet.battleGround[i];
    }
    delete [] Fleet.battleGround;
}

void destroyPCMemory(s_pc_memory & pcMemory) {
    delete [] pcMemory.detectPoint;
}

int main(){
    /*
        randomizer
    */
    srand(time(NULL));
    /*
        first clear console
    */
    system("clear");

    short battlegroundSize = 10;
    /*
        user action
    */
    short action;
    short shipsAllive;
    bool shotSucces;

    bool fleetInitialised[2] = {false, false};
    bool pcMemoryInitialised = false;
    /*
        hide the ships
    */
    bool fogOfWar = true;
    char whoWins[255];

    s_fleet myFleet, PCFleet;
    e_game_stage stage = BG_SELECTION;
    e_shooters currentShooter;

    myFleet = initFleet(battlegroundSize);

    fleetInitialised[0] = true;

    do {
        /*
            clear console
        */
        system("clear");

        /*
            my Battleground
        */

        printButtleground(myFleet, stage);

        shipsAllive = inspectFleet(myFleet.ships, stage);
        if(shipsAllive==0) {
            strcpy(whoWins, "PC");
        }

        if(stage == BG_SHOTING || stage == BG_END_GAME) {
            if(stage == BG_END_GAME) {
                fogOfWar = false;
            }

            /*
                PC Battleground
            */
            printButtleground(PCFleet, stage, fogOfWar);
            shipsAllive = inspectFleet(PCFleet.ships, stage);
            if(shipsAllive==0) {
                strcpy(whoWins, "HUMAN");
            }

        }
        if(stage == BG_END_GAME) {
            /*
                GAME OVER!
            */
            cout << "END GAME " << whoWins << " WINS!" << endl;
        } else  if(currentShooter == SHOTER_PC) {
            /*
                PC shoot turn
            */
            shotSucces = PCShoot(myFleet, pcMemory);
            if(shotSucces == false) {
                /*
                    PC miss
                */
                changeShooter(currentShooter);
            }
            /*
                Delay and next PC shot
            */
            sleep(1);
            continue;
        }

        printControls(stage);

        /*
            get user action
        */
        action = mygetch();

        if(stage==BG_CONFIRM) {
            switch (action) {
                case A_YES:
                    /*
                        confirm my fleet
                        change stage
                        init PC Fleet
                        autoset PC Fleet ships
                        init PC memory
                    */
                    stage = BG_SHOTING;
                    myFleet.pointer[0]=myFleet.pointer[1]=-1;
                    PCFleet = initFleet(battlegroundSize);
                    fleetInitialised[1] = true;
                    while (setShip(PCFleet, stage, true, false));
                    currentShooter = SHOTER_HUMAN;
                    pcMemory.detectPoint = new short [2];
                    pcMemory.shipDetected = false;
                    pcMemory.direction = UNKNOWN;
                    pcMemoryInitialised = true;
                    break;
                case A_NO:
                    /*
                        reset my fleet
                    */
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
                        /*
                            move my pionter
                        */
                        movePointer(PCFleet.pointer, action, battlegroundSize);
                        break;
                    case BG_SELECTION:
                        /*
                            move my new ship
                        */
                        moveShip(myFleet, action);
                        break;
                }
                break;
            case A_AUTOLOCATE:
                switch (stage) {
                    case BG_SELECTION:
                        /*
                            autoset my new ship
                        */
                        setShip(myFleet, stage, true);
                        break;
                }
                break;
            case A_DO:
                switch (stage) {
                    case BG_SHOTING:
                        /*
                            my shot
                        */
                        shotSucces = shot(PCFleet);
                        if(shotSucces == false) {
                            changeShooter(currentShooter);
                        }
                        break;
                    case BG_SELECTION:
                        /*
                            place my new ship
                        */
                        setShip(myFleet, stage);
                        break;
                }
                break;
            case A_TURN:
                switch (stage) {
                    case BG_SELECTION:
                        /*
                            turn my new ship
                        */
                        turnShip(myFleet);
                        break;
                }
                break;
        }

    } while (action!=A_EXIT);

    if(fleetInitialised[0] == false){
        destroyFleet(myFleet);
    }

    if(fleetInitialised[1] == false){
        destroyFleet(PCFleet);
    }

    if(pcMemoryInitialised == false){
        destroyPCMemory(pcMemory);
    }

    cout << endl << "Press any key to exit..." << endl;
    /*
        А тут должна быть очистка памяти, но я не упсеваю
    */
    mygetch();
    system("clear");
    return 0;
}
