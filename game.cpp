#include <iostream>
#include <time.h>
#include <bitset>
#include "game.h"
using namespace std;

#define up(i, j) ((j << 4) + (i << 2))
#define down(i, j) (((3-j) << 4) + (i << 2))
#define left(i, j) ((i << 4) + (j << 2))
#define right(i, j) ((i << 4) + ((3-j) << 2))

char game::getMapNorm(int i, int j){
    return (map >> left(i, j)) & 0xF;
}

void game::setMapNorm(int i, int j, char newVal){
    map = ((map & ~(((long long)0xF) << left(i, j))) | (((long long)newVal) << left(i, j)));
}

char game::getMap(char i, char j){
    switch(direction){
        case 0: return (map >> up(i, j)) & 0xF;
        case 1: return (map >> down(i, j)) & 0xF;
        case 2: return (map >> left(i, j)) & 0xF;
        case 3: return (map >> right(i, j)) & 0xF;
    }
}

void game::setMap(char i, char j, char newVal){
    switch(direction){
        case 0: map = ((map & ~(((long long)0xF) << up(i, j))) | ((long long)newVal << up(i, j))); return;
        case 1: map = ((map & ~(((long long)0xF) << down(i, j))) | ((long long)newVal << down(i, j))); return;
        case 2: map = ((map & ~(((long long)0xF) << left(i, j))) | ((long long)newVal << left(i, j))); return;
        case 3: map = ((map & ~(((long long)0xF) << right(i, j))) | ((long long)newVal << right(i, j))); return;
    }
}

void game::move(char dir){
    direction = dir;

    for(char i = 0; i < 4; i++){
        for(char j = 0; j < 3; j++){
            if(getMap(i, j) == 0){
                for(char k = j+1; k < 4; k++){
                    if(getMap(i, k) != 0){
                        setMap(i, j, getMap(i, k));
                        setMap(i, k, 0);
                        break;
                    }
                }

                if(getMap(i, j) == 0)
                    break;
            }
            
            for(char k = j+1; k < 4; k++){
                if(getMap(i, k) != 0){
                    if(getMap(i, j) == getMap(i, k)){
                        setMap(i, k, 0);
                        setMap(i, j, getMap(i, j) + 1);
                    }

                    break;
                }
            }
        }
    }
}

int game::score(){
    int tot = 0;
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            tot += (1 << (int)getMapNorm(i, j));
    return tot;
}

void game::printMap(){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            int x = (1 << (int)getMapNorm(i, j));
            if(x == 1)
                cout << "*\t";
            else
                cout << x << "\t";
        }
        cout << endl;
    }
    cout << "------------------------------------------------------" << endl;
}

bool game::spawn(){
    bool dead = true;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            if(getMapNorm(i, j) == 0){
                dead = false;
                break;
            }
        }

        if(!dead)
            break;
    }

    if(dead)
        return false;

    while(1){
        int r = rand();
        int i = (r & 0b11000) >> 3;
        int j = (r & 0b110) >> 1;

        if(getMapNorm(i, j) == 0){
            setMapNorm(i, j, (r & 0b1) + 1);
            break;
        }
    }

    return true;
}

game::game(){
    map = 0;
    direction = 0;
}