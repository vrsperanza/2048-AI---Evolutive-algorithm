#include <iostream>
#include <time.h>
#include <bitset>
#include "game.h"

using namespace std;

class bot{
    public:

    char getMove(long long map){
        return rand()%4;
    }

    int play(){
        game g;
        while(1){
            if(!g.spawn())
                return g.score();

            g.move(getMove(g.map));
        }
    }
};

int main(){
    srand(time(NULL));

    bot b;
    for(int i = 0; i < 100000; i++)
        cout << b.play() << endl;
}