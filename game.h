#ifndef _GAME_H_
#define _GAME_H_

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
    bool move(char dir);
    void humanGame();
    void printMap();
    bool spawn();
    game();
};

#endif