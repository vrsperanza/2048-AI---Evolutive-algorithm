#include <iostream>
#include <time.h>
#include <bitset>
#include <math.h>
#include <random>
#include <algorithm>
#include <vector>
#include "game.h"

using namespace std;

double fRand(double fMin=0, double fMax=1)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

class bot{
    public:

    #define sigmoid(x) 1 / (1 + exp((double) -x))
    #define hiddenLayerSize 4

    double l0[hiddenLayerSize][16 * 16];
    double l0out[hiddenLayerSize];
    double l1[4][hiddenLayerSize];
    double l1out[4];

    double averageScore = 0;
    int totalScore = 0;
    int gamesPlayed = 0;

    char getMove(long long map){
        for(int i = 0; i < hiddenLayerSize; i++){
            double tot = 0;
            for(int j = 0; j < 16; j++)
                tot += l0[i][16 * ((map >> (j << 2)) & 0b11) + j];
            
            l0out[i] = sigmoid(tot);
        }

        double maxVal = 0;
        int maxId = 0;

        for(int i = 0; i < 4; i++){
            double tot = 0;
            for(int j = 0; j < hiddenLayerSize; j++)
                tot += l1[i][j] * l0out[j];
            l1out[i] = sigmoid(tot);

            if(l1out[i] > maxVal){
                maxVal = l1out[i];
                maxId = i;
            }
        }

        return maxId;
    }

    int play(){
        if(gamesPlayed >= 50)
            return averageScore;

        game g;
        while(g.spawn())
            g.move(getMove(g.map));

        totalScore += g.score;
        gamesPlayed++;
        averageScore = totalScore / (double)gamesPlayed;
        return g.score;
    }

    bot(){
        for(int i = 0; i < hiddenLayerSize; i++){
            for(int j = 0; j < 16 * 16; j++)
                l0[i][j] = fRand();
        }

        for(int i = 0; i < 4; i++){
            for(int j = 0; j < hiddenLayerSize; j++)
                l1[i][j] = fRand();
        }
    }

    crossOver(bot a, bot b, double mutationRate, double mutationSize){
        bot parents[2];
        parents[0] = a;
        parents[1] = b;
        for(int i = 0; i < hiddenLayerSize; i++){
            int parentId = fRand() < 0.5;
            for(int j = 0; j < 16 * 16; j++){
                l0[i][j] = parents[parentId].l0[i][j];

                if(fRand() < mutationRate)
                    l0[i][j] += fRand(-mutationSize, mutationSize);
            }

            for(int j = 0; j < 4; j++){
                l1[j][i] = parents[parentId].l1[j][i];

                if(fRand() < mutationRate)
                    l1[j][i] += fRand(-mutationSize, mutationSize);
            }
        }

        averageScore = 0;
        gamesPlayed = 0;
        totalScore = 0;
    }

	bool operator<(const bot& b) const {
		return averageScore < b.averageScore;
	}
};

int main(){
    srand(time(NULL));

    vector<bot> bots(32);

    int gens = 0;

    vector<double> bestAverageScoreHistory;

    while(1){
        bestAverageScoreHistory.push_back(bots[bots.size()-1].averageScore);

        if(bestAverageScoreHistory.size() > 100){
            bool stuck = true;
            for(int i = bestAverageScoreHistory.size()-100; i < bestAverageScoreHistory.size(); i++)
                if(bestAverageScoreHistory[i] != bestAverageScoreHistory[bestAverageScoreHistory.size()-1])
                    stuck = false;
            if(stuck)
                break;
        }

        for(int i = 0; i < bots.size(); i++)
            bots[i].play();
        
        while(!is_sorted(bots.begin(), bots.end())){
            sort(bots.begin(), bots.end());

            for(int i = bots.size()-8; i < bots.size(); i++)
                bots[i].play();
            for(int i = bots.size()-4; i < bots.size(); i++)
                bots[i].play();
            for(int i = bots.size()-2; i < bots.size(); i++)
                bots[i].play();
            bots[bots.size()-1].play();
        }

        for(int i = 0; i < bots.size()-3; i++)
            bots[i].crossOver(bots[rand()%bots.size()], bots[bots.size() - 1 - (rand()%bots.size())/2], 0.1, 1);

        cout << gens << " " << bots[bots.size()-1].averageScore << endl;
        gens++;
    }

    bot b = bots[bots.size()-1];

    game g;

    while(g.spawn()){
        g.move(b.getMove(g.map));
        g.printMap();
    }

    cout << g.score << endl;
}