#ifndef _GAME_H_
#define _GAME_H_

#include <vector>
using namespace std;

class game{
    private:
    char direction;
    char getMapNorm(int i, int j);
    void setMapNorm(int i, int j, char newVal);
    char getMap(char i, char j);
    void setMap(char i, char j, char newVal);
    bool innerMove(char dir);

    public:
    int score;
    unsigned long long map;
    bool move(vector<char> dir);
    void humanGame();
    void printMap();
    bool spawn();
	unsigned long long previewMove(char dir);
    game();
};

#endif