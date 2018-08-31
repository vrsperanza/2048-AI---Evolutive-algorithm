#include <iostream>
#include <time.h>
#include <bitset>
#include <math.h>
#include <random>
#include <algorithm>
#include <vector>
#include "game.h"

using namespace std;

#define DefaultEvaluationIterations 10

double fRand(double fMin=0, double fMax=1)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

class bot{
    public:

    #define sigmoid(x) 1 / (1 + exp((double) -x))
    #define hiddenLayerSize 16

    double l0[hiddenLayerSize][16 * 16];
    double l0out[hiddenLayerSize];
    double l1[4][hiddenLayerSize];
    double l1out[4];

    double averageScore;

    int id;

    vector<char> getMove(long long map){
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

        vector<pair<double, char> > v;
        
        for(char i = 0; i < 4; i++)
            v.push_back(make_pair(l1out[i], i));
        sort(v.rbegin(), v.rend());
        vector<char> move;

        for(char i = 0; i < 4; i++)
            move.push_back(v[i].second);

        return move;
    }

    double evaluate(int iterations){
        int totalScore = 0;
        for(int i = 0; i < iterations; i++){
            game g;
            while(g.spawn())
                g.move(getMove(g.map));

            totalScore += g.score;
        }

        return totalScore / (double)iterations;
    }

    void playAndShow(){
        game g;
        while(g.spawn())
            g.move(getMove(g.map));
        g.printMap();
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

        averageScore = evaluate(DefaultEvaluationIterations);
        
        id = rand();
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

        averageScore = evaluate(DefaultEvaluationIterations);

        id = rand();
    }

	bool operator<(const bot& b) const {
		return averageScore < b.averageScore;
	}
};

int main(){
    srand(time(NULL));

    vector<bot> bots(10);

    int gens = 0;

    vector<double> bestAverageScoreHistory;


    double mutationMultiplier = 1;
    int mutationFactor = 0;

    int bestId = -1;
    int lastBestId = -1;
    int currStability = 0;
    int updateStableScoreCounter = 0;

    while(1){
        bestId = bots[bots.size()-1].id;

        if(bestId == lastBestId){
            currStability++;
        }
        else{
            currStability = 0;
            mutationFactor = 0;
            mutationMultiplier = 1;
        }

        lastBestId = bestId;

        if(currStability >= 10){
            mutationFactor++;
            currStability = 0;

            if(mutationFactor < 20)
                mutationMultiplier = 1 / double(1 << mutationFactor);
            else if(mutationFactor < 40)
                mutationMultiplier = (1 << (mutationFactor-20));
            else{
                mutationFactor = 0;

                cout << "MUTATION RESET" << endl;
            }
        }

        for(int i = 0; i < bots.size()-1; i++)
            bots[i].crossOver(bots[bots.size()-1], bots[i], mutationMultiplier * 0.1, mutationMultiplier * 1);
            //bots[i].crossOver(bots[i + 1 + rand()%(bots.size()-i-1)], bots[i], mutationMultiplier * 0.1, mutationMultiplier * 1);
        bots[bots.size()-1].averageScore = bots[bots.size()-1].evaluate(DefaultEvaluationIterations);
        sort(bots.begin(), bots.end());

        cout << "Generation: " << gens << endl;
        cout << "Best average score: " << bots[bots.size()-1].averageScore << endl;
        cout << "True best average score: " << bots[bots.size()-1].evaluate(50) << endl;
        cout << "Stability: " << currStability << endl;
        cout << "Mutation rate: " << mutationMultiplier << endl;
        bots[bots.size()-1].playAndShow();
        gens++;
    }
}