#ifndef _SBN_
#define _SBN_

#include<iostream>
#include<vector>
#include<deque>
#include<cmath>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<queue>
#include<ctime>
#include<Windows.h>
using namespace std;


extern deque<vector<double>> rawX;
extern deque<vector<double>> X;
extern vector<vector<double>> B;
extern vector<vector<double>> showB;
extern vector<vector<double>> DAG;
extern vector<vector<double>> P;
extern int dim;
extern int num;


void SBN(char* filename, int d, double penalty, double relation);

void Train();





	//   ********************************����ʽ**************************
void update(vector<double> sample);
void printDAG();
void printB();
void printShowB();

	//**********************************��������************************
double Expection(int x);
double Cov(int x, int y);
double Parameter(int x);    //������  sigma2

#endif