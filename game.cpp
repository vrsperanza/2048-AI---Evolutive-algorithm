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
        case 1: return (map >> left(i, j)) & 0xF;
        case 2: return (map >> down(i, j)) & 0xF;
        case 3: return (map >> right(i, j)) & 0xF;
    }
}

char getMap(const unsigned long long map, char i, char j){
    return (map >> left(i, j)) & 0xF;
}

void setMap(unsigned long long & map, int i, int j, char newVal){
    map = ((map & ~(((long long)0xF) << left(i, j))) | (((long long)newVal) << left(i, j)));
}

void game::setMap(char i, char j, char newVal){
    switch(direction){
        case 0: map = ((map & ~(((long long)0xF) << up(i, j))) | ((long long)newVal << up(i, j))); return;
        case 1: map = ((map & ~(((long long)0xF) << left(i, j))) | ((long long)newVal << left(i, j))); return;
        case 2: map = ((map & ~(((long long)0xF) << down(i, j))) | ((long long)newVal << down(i, j))); return;
        case 3: map = ((map & ~(((long long)0xF) << right(i, j))) | ((long long)newVal << right(i, j))); return;
    }
}

bool game::innerMove(char dir){
    direction = dir;
	
    bool change = false;
    for(char i = 0; i < 4; i++){
        for(char j = 0; j < 3; j++){
            if(getMap(i, j) == 0){
                for(char k = j+1; k < 4; k++){
                    if(getMap(i, k) != 0){
                        setMap(i, j, getMap(i, k));
                        setMap(i, k, 0);
                        change = true;
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

                        score += (1 << getMap(i, j));
                        change = true;
                    }

                    break;
                }
            }
        }
    }
	
    return change;
}

bool game::move(vector<char> dir){
    for(char c : dir)
        if(innerMove(c))
            return true;
    return false;
}

unsigned long long game::previewMove(char dir){
	unsigned long long oldMap = map;
	int oldScore = score;
	
	if(!innerMove(dir))
		return 0;
	
	score = oldScore;
	unsigned long long newMap = map;	
	map = oldMap;
	
	return newMap;
}

void game::printMap(){
    cout << "Score: " << score << endl;

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
        int i = (r & 0b1100) >> 2;
        int j = (r & 0b11);

        if(getMapNorm(i, j) == 0){
            setMapNorm(i, j, ((r >> 4) % 10 == 0) + 1);
            break;
        }
    }

    return true;
}

game::game(){
    map = 0;
    direction = 0;
    score = 0;
}

void game::humanGame(){
    game g;
    while(g.spawn()){
        cout << g.score << endl;
        g.printMap();

        char c;
        cin >> c;
        char id = c == 'w' ? 0 : c == 'a' ? 1 : c == 's' ? 2 : 3;
        vector<char> move;
        move.push_back(id);
        while(!g.move(move)){
            cin >> c;
            id = c == 'w' ? 0 : c == 'a' ? 1 : c == 's' ? 2 : 3;
        }
    }

    cout << g.score << endl;
}

unsigned long long rotate(const unsigned long long map){
    unsigned long long result;

    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            setMap(result, i, j, getMap(map, j, 3-i));

    return result;
}

unsigned long long mirror(const unsigned long long map){
    unsigned long long result;

    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            setMap(result, i, j, getMap(map, i, 3-j));

    return result;
}

unsigned long long standardMap(const unsigned long long map){
    unsigned long long result = -1;
    unsigned long long rot = map;

    result = min(result, rot);
    
    for(int i = 0; i < 3; i++){
        rot = rotate(rot);
        result = min(result, rot);
    }

    rot = mirror(rot);
    result = min(result, rot);

    for(int i = 0; i < 3; i++){
        rot = rotate(rot);
        result = min(result, rot);
    }

    return result;
}