#include <iostream>
#include <time.h>
#include <bitset>
using namespace std;

class game{
    public:

    unsigned long long map;

    #define up(i, j) ((j << 4) + (i << 2))
    #define down(i, j) (((3-j) << 4) + (i << 2))
    #define left(i, j) ((i << 4) + (j << 2))
    #define right(i, j) ((i << 4) + ((3-j) << 2))

    char getMapNorm(int i, int j){
        return (map >> left(i, j)) & 0xF;
    }

    void setMapNorm(int i, int j, char newVal){
        map = ((map & ~(((long long)0xF) << left(i, j))) | (((long long)newVal) << left(i, j)));
    }

    char direction;
    char getMap(char i, char j){
        switch(direction){
            case 0: return (map >> up(i, j)) & 0xF;
            case 1: return (map >> down(i, j)) & 0xF;
            case 2: return (map >> left(i, j)) & 0xF;
            case 3: return (map >> right(i, j)) & 0xF;
        }
    }

    void setMap(char i, char j, char newVal){
        switch(direction){
            case 0: map = ((map & ~(((long long)0xF) << up(i, j))) | ((long long)newVal << up(i, j))); return;
            case 1: map = ((map & ~(((long long)0xF) << down(i, j))) | ((long long)newVal << down(i, j))); return;
            case 2: map = ((map & ~(((long long)0xF) << left(i, j))) | ((long long)newVal << left(i, j))); return;
            case 3: map = ((map & ~(((long long)0xF) << right(i, j))) | ((long long)newVal << right(i, j))); return;
        }
    }

    bool move(char dir){
        direction = dir;

        cout << "MOVE: " << (int)dir << endl;

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

            printMap();
        }

        cout << "MOVED" << endl;
    }

    void printMap(){
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

    game(){
        map = 0;
        direction = 0;
    }
};

int main(){
    srand(time(NULL));
    game g;

    while(1){
        bool dead = true;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                if(g.getMapNorm(i, j) == 0){
                    dead = false;
                    break;
                }
            }

            if(!dead)
                break;
        }

        if(dead)
            break;

        while(1){
            int i = rand()%4;
            int j = rand()%4;

            if(g.getMapNorm(i, j) == 0){
                g.setMapNorm(i, j, rand()%2 + 1);
                break;
            }
        }

        g.printMap();

        char c;
        cin >> c;

        if(c == 'w')
            g.move(0);
        else if(c == 's')
            g.move(1);
        else if(c == 'a')
            g.move(2);
        else if(c == 'd')
            g.move(3);
    }
}