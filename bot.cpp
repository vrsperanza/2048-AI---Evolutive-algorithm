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
const double startGenerationImportance = 0.01;
const int generationImportanceHalfTime = -1;
const int trainingIterations = 5000;
const int hiddenLayerSize = 3;

double currGenerationImportance = startGenerationImportance;

default_random_engine generator;
normal_distribution<double> distribution(0,1);
double fRand(double fMin=0, double fMax=1){
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

class bot{
    public:

    #define sigmoid(x) 1 / (1 + exp((double) -x))

    double l0[hiddenLayerSize][16][16];
    double l0Offset[hiddenLayerSize];
    double l1[hiddenLayerSize];

    double averageScore = 0;

    // Recupera a pontuação dada pela rede neural para um certo tabuleiro
	double getScore(const unsigned long long map){
		if(map == 0)
			return numeric_limits<double>::lowest();

        unsigned long long standard = standardMap(map);
		
		double result = 0;
        for(int l = 0; l < hiddenLayerSize; l++){
            double tot = 0;
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    tot += l0[l][4*i + j][getMap(standard, i, j)];
                }
            }
            
			result += l1[l] * ((tot + l0Offset[l]) > 0);
        }
		return result;
	}
	
    // Calcula a pontuação de cada tabuleiro resultante de cada movimento
    // E retorna a ordem em que os movimentos foram pontuados.
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

    // Avalia o desempenho do robo em "iterations" jogos
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
	
    // Simula um jogo e mostra o resultado final
    void playAndShow(){
        game g;
        while(g.spawn())
            g.move(getMove(g));
        g.printMap();
    }

    void showoff(){
        game g;
        while(g.spawn()){
            g.printMap();
            getchar();
            g.move(getMove(g));
        }
        g.printMap();

        cout << "Game over, final score: " << g.score << endl;
    }

    // Instancia um robô vazio
    bot(){
        for(int i = 0; i < hiddenLayerSize; i++){
            for(int j = 0; j < 16; j++)
                for(int k = 0; k < 16; k++)
                    l0[i][j][k] = distribution(generator);
            l0Offset[i] = distribution(generator);
        }

        for(int j = 0; j < hiddenLayerSize; j++)
			l1[j] = distribution(generator);
		
        averageScore = 0;
    }

    void clone(bot parent){
        for(int i = 0; i < hiddenLayerSize; i++){
            for(int j = 0; j < 16; j++)
                for(int k = 0; k < 16; k++)
                    l0[i][j][k] = parent.l0[i][j][k];
            l0Offset[i] = parent.l0Offset[i];
            l1[i] = parent.l1[i];
        }
    }

    // Simula um jogo e adiciona esse jogo a pontuação
    // atual do robo, de acordo com a importância da geração atual
    void updateScore(){
        game g;
        while(g.spawn())
            g.move(getMove(g));

        averageScore *= 1 - currGenerationImportance;
		averageScore += g.score * currGenerationImportance;
    }

    // Cruza dois robos, transformando essa instancia no filho de ambos
    // Ao cruzar seleciona aleatóriamente quanto do pai ou da mãe será
    // e soma uma mutação a cada peso de cada neurônio
    void crossOver(const bot & a, const bot & b, double mutationSize){
        //if(fRand() < 0.5){
        //    clone(b);
        //}
        //else{
            for(int i = 0; i < hiddenLayerSize; i++){
                double aWeight = fRand();
                for(int j = 0; j < 16; j++)
                    for(int k = 0; k < 16; k++)
                        l0[i][j][k] = (aWeight * a.l0[i][j][k] + (1 - aWeight) * b.l0[i][j][k]) + mutationSize * distribution(generator) * distribution(generator);
                l0Offset[i] = (aWeight * a.l0Offset[i] + (1 - aWeight) * b.l0Offset[i]) + mutationSize * distribution(generator) * distribution(generator);

                l1[i] = (aWeight * a.l1[i] + (1 - aWeight) * b.l1[i]) + mutationSize * distribution(generator) * distribution(generator);
            }
        //}
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

    double mutationMultiplier = 1;
    int mutationFactor = 0;

    int currStability = 0;
	
	double bestScore = 0;
	
    while(gens != trainingIterations){
        // Importancia da geração atual diminui com o tempo
		if(generationImportanceHalfTime > 0)
			currGenerationImportance = startGenerationImportance * generationImportanceHalfTime/double(generationImportanceHalfTime+gens);
		
        // Mutação variavel
		if(bestScore < bots[populationSize-1].averageScore){
			bestScore = bots[populationSize-1].averageScore;
			currStability = 0;
			mutationFactor = 0;
			mutationMultiplier = 1;
		}
		else
			currStability++;

        if(currStability >= 1000){
            mutationFactor++;
            currStability = 0;

            if(mutationFactor < 20)
                mutationMultiplier = 1 / double(1 << mutationFactor);
            else if(mutationFactor < 40)
                mutationMultiplier = (1 << (mutationFactor-20));
            else{
                mutationFactor = 0;
                mutationMultiplier = 1;
            }
        }

        // Metade da população cruza com os melhores,
        // o melhor tendo 50% de chance de ser escolhido,
        // o segundo melhor 25% e assim por diante.
        // A outra metade é substituida por seus filhos
        for(int i = 0; i < 0.75 * bots.size(); i++){
            int r = rand();
            int nxt = bots.size()-1;
            while(r & 1){
                r >>= 1;
                nxt--;
            }
            if(nxt <= i)
                nxt = bots.size()-1;

            bots[i].crossOver(bots[nxt], bots[i], mutationMultiplier);
        }
		
        // Cálculo da pontuação de cada bot.
        // Região de crítica de processamento, paralelizada.
        // Testamos o métodos explicado em sala para herança e
        // nesse caso, aparentemente, calcular o valor do atual
        // sendo uma porcentagem do valor antigo completada pelo
        // valor novo funcionou melhor.
		thread threads[populationSize];
        for(int i = 0; i < populationSize; i++)
            threads[i] = thread(updateScore, i);
        for(int i = 0; i < populationSize; i++)
            threads[i].join();
		
        // Ordena os robos reavaliados
        sort(bots.begin(), bots.end());

        // A cada 100 gerações, imprime os dados na tela e simula um jogo,
        // imprimindo seu estado final.
        if(gens % 100 == 0){
			double eval = bots[bots.size()-1].evaluate(100);
            cout << "Generation: " << gens << endl;
            cout << "Best selection score seen: " << bestScore << endl;
            cout << "Selection score: " << bots[bots.size()-1].averageScore << endl;
            cout << "Average game score: " << eval << endl;
            cout << "Stability: " << currStability << endl;
            cout << "Mutation rate: " << mutationMultiplier << endl;
            cout << "Curr generation importance: " << currGenerationImportance << endl;
            bots[bots.size()-1].playAndShow();
        }
        gens++;
    }

    bot bestBot = bots[bots.size()-1];
    while(1)
        bestBot.showoff();
}