#include <iostream>
#include <time.h>
#include <bitset>
#include <math.h>
#include <random>
#include <algorithm>
#include <vector>
#include <thread>
#include "game.h"

using namespace std;

const int populationSize = 32;
double CurrGenerationImportance = 1;
const int generationImportanceHalfTime = 1000;

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
    double l0Offset[hiddenLayerSize];
    double l0out[hiddenLayerSize];
    double l1[hiddenLayerSize];
    double l1Offset;

    double averageScore = 0;

    int id;

	double getScore(long long map){
		if(map == 0)
			return -1;
		
        for(int i = 0; i < hiddenLayerSize; i++){
            double tot = 0;
            for(int j = 0; j < 16; j++){
                tot += l0[i][16 * ((map >> (j << 2)) & 0b11) + j];
			}
            
            l0out[i] = sigmoid(tot + l0Offset[i]);
        }
		double tot = 0;
		for(int j = 0; j < hiddenLayerSize; j++)
			tot += l1[j] * l0out[j];
		return tot + l1Offset;
	}
	
    vector<char> getMove(game g){
        vector<pair<double, char> > v;
		for(char i = 0; i < 4; i++)
			v.push_back(make_pair(getScore(g.previewMove(i)), i));
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
                g.move(getMove(g));

            totalScore += g.score;
        }

        return totalScore / (double)iterations;
    }
	
    void playAndShow(){
        game g;
        while(g.spawn())
            g.move(getMove(g));
        g.printMap();
    }

    bot(){
        for(int i = 0; i < hiddenLayerSize; i++){
            for(int j = 0; j < 16 * 16; j++)
                l0[i][j] = fRand();
            l0Offset[i] = fRand();
        }

        for(int j = 0; j < hiddenLayerSize; j++)
			l1[j] = fRand();
		l1Offset = fRand();
		
        averageScore = 0;
        
        id = rand();
    }

    void updateScore(){
        game g;
        while(g.spawn())
            g.move(getMove(g));

        averageScore *= 1 - CurrGenerationImportance;
        averageScore += log(g.score) * CurrGenerationImportance;
    }

    void crossOver(bot a, bot b, double mutationRate, double mutationSize){
        bot parents[2] = {a, b};

        for(int i = 0; i < hiddenLayerSize; i++){
            int parentId = fRand() < 0.5;
            for(int j = 0; j < 16 * 16; j++){
                l0[i][j] = parents[parentId].l0[i][j];
                if(fRand() < mutationRate)
                    l0[i][j] += fRand(-mutationSize, mutationSize);
            }
            l0Offset[i] = parents[parentId].l0Offset[i];
			if(fRand() < mutationRate)
				l0Offset[i] += fRand(-mutationSize, mutationSize);

            l1[i] = parents[parentId].l1[i];
			if(fRand() < mutationRate)
				l1[i] += fRand(-mutationSize, mutationSize);
        }
		l1Offset = (a.l1Offset + b.l1Offset) / 2.0;
		if(fRand() < mutationRate)
			l1Offset += fRand(-mutationSize, mutationSize);
		
		averageScore = (a.averageScore + b.averageScore) / 2.0;
        id = rand();
    }

	bool operator < (const bot& b) const {
		return averageScore < b.averageScore;
	}
};

vector<bot> bots(populationSize);

void updateScore(int botId){
	bots[botId].updateScore();
}

int main(){
    srand(time(NULL));


    int gens = 0;

    vector<double> bestAverageScoreHistory;


    double mutationMultiplier = 1;
    int mutationFactor = 0;

    int bestId = -1;
    int lastBestId = -1;
    int currStability = 0;
    int updateStableScoreCounter = 0;

    while(1){
        CurrGenerationImportance = generationImportanceHalfTime/double(5*generationImportanceHalfTime+gens);

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

        if(currStability >= 50){
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

        //for(int i = 0; i < bots.size()-3; i++)
        //    bots[i].crossOver(bots[bots.size()-1], bots[i], mutationMultiplier * 0.1, mutationMultiplier * 1);
        
        //for(int i = 0; i < bots.size()-3; i++)
        //    bots[i].crossOver(bots[i + 1 + rand()%(bots.size()-i-1)], bots[i], mutationMultiplier * 0.05, mutationMultiplier * 1);

        for(int i = 0; i < bots.size()-8; i++){
            int r = rand();
            int nxt = bots.size()-1;
            while(r & 1){
                r >>= 1;
                nxt--;
            }
            if(nxt <= i)
                nxt = bots.size()-1;

            bots[i].crossOver(bots[nxt], bots[i+8], mutationMultiplier * 0.01, mutationMultiplier * 0.1);
        }

		std::thread threads[populationSize];
        for(int i = 0; i < populationSize; i++)
            threads[i] = thread(updateScore, i);
        for(int i = 0; i < populationSize; i++)
            threads[i].join();

        sort(bots.begin(), bots.end());

        if(gens % 100 == 0){
			double eval = bots[bots.size()-1].evaluate(100);
            cout << "Generation: " << gens << endl;
            cout << "Selection score: " << bots[bots.size()-1].averageScore << endl;
            cout << "Average game score: " << eval << endl;
            cout << "Stability: " << currStability << endl;
            cout << "Mutation rate: " << mutationMultiplier << endl;
            cout << "Curr generation importance: " << CurrGenerationImportance << endl;
            bots[bots.size()-1].playAndShow();
        }
        gens++;
    }
}