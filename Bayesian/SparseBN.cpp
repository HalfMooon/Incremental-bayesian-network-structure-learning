#include"SparseBN.h"
#include"utils.h"
#include"Reason.h"
using namespace std;

deque<vector<double>> rawX;
deque<vector<double>> X;
vector<vector<double>> B;
vector<vector<double>> showB;
vector<vector<double>> DAG;
vector<vector<double>> P;
double lamda1;
long double lamda2;
int dim;
int num;
double relationThreshold;


void SBN(char* filename, int d, double penalty, double relation){

	dim = d;
	relationThreshold = relation;
	rawX = loadtxt(filename, dim);
	
	cout << "standard......................" << endl;
	cout << "������СΪ...................." << rawX.size() << endl;
	X = rawX;
	Scale(rawX, X);   //standard   0 mean and 1 variance



	num = X.size();
	B = Zero(dim - 1, dim);     //(p-1)*p ��
	showB = Zero(dim - 1, dim);         //������ʾ DAG �õ� B����ԭʼB����̭��ϵ��ֵ���µģ���������Ӧ��֦����֮ DAG ����� showB �Ƕ�Ӧ�ģ����������Ҳ�ǻ������showB�� ��ֱ�Ӷ�B��֦������B����������ʱ�������ۼ�
	lamda1 = penalty*(0.11*num);    			//penalty
	//lamda2 = (((num - 1)*(num - 1)*dim) / lamda1 - lamda1)*10;
	
	
	lamda2 = 1000 * num;
	
	
	cout << num << ' ' << dim << ' ' << lamda1 <<"�õ���lamda2 "<<lamda2<<endl;
	//vector<double> lamdaset=[Linspace(penalty,(dim*(num-1)*(num-1))/lamda1-lamda1,10)];   //set  lamda2 from lamda1 to (n-1)2 *p/lamda1 - lamda1   �����ã���ʱû���ϣ�����lamda��Ϊ�����������ڣ�

	DAG = Zero(dim, dim);     //DAG
	P = Zero(dim, dim);
}


vector<double> BFS(int i){      //   �ٶ��Ѽ���
	//�õ�DAG��iΪ���ıհ� 
     
	for (int j = 0; j < dim; j++)
		P[i][j] = 0;    //i��Ӧ������գ�����BFS����
	queue<double> q;
	q.push(i);
	while (!q.empty())
	{
		double node = q.front();
		q.pop();
		if (P[i][node] == 0)
		{
			P[i][node] = 1;
			for (int adj = 0; adj < dim; adj++)
			{
				if (DAG[node][adj] == 1 && P[i][adj] == 0)
					q.push(adj);
			}
		}
	}
	
	
	return P[i];    //����P����һ�У�ȷ���հ�
}


void refreshDAG(int i){                       //BCD ��50άÿһά������ʱ��ˢ�¶�Ӧ�е�DAG������BFS����Ч
	for (int j = 0; j < dim; j++)
		DAG[j][i] = 0;                              //��Ӧ����գ�����ˢ��һ��    Ϊ���գ�B����������DAG�޸�ʱÿ��ȫ��ˢ��

	for (int j = 0; j < (dim - 1); j++)
	{
		if (j >= i)
		{
			if (B[j][i] != 0)
				DAG[j + 1][i] = 1;

		}
		else
		{
			if (B[j][i] != 0)
				DAG[j][i] = 1;

		}
	}
}

vector<double> getBij(int i, int j)  // get Bi / j    dim is(p - 2) * 1
{
	vector<double>result;
	result.clear();
	RemoveElem(B, i, j,result);
	return result;
}



void getXij(deque<vector<double>>&result, int dim1, int dim2)      //�������ֻ��Ҫ�Ƴ�����         ����ٶȱ�python�죬������������  ���6ms python14ms
{
	int k;
	if (dim1 < dim2){ k = dim1; dim1 = dim2; dim2 = k; }
	for (unsigned i = 0; i < result.size(); i++)
	{
		result[i].erase(result[i].begin() + dim1);
		result[i].erase(result[i].begin() + dim2);
	}
	
	return;
}


void Shooting(int i, int shootIter)
{
	vector<double> xi;
	
	GetCol_S(X, i,xi);

	
	for (int t = 0; t < shootIter; t++)     //����������
	{
		vector<double>LastColB;
		GetCol_M(B, i, LastColB);
		double temp = Norm(LastColB);     //ÿ��ѭ������һ�ε�Bi �浽temp��
		for (int j = 0; j < dim; j++)    //J ��ÿһ��ѭ��������B�� (p-1)*p  ����һ�� �� �� p-1 ��Ԫ�أ� �ر�ģ���j>i��ʱ������Ҫ-1����Ϊռ���˱���i��λ�ã��ű��p-1ά
		{
			
			if (j == i)
				continue;    //��֤ÿ��j�ĵ������� (p-1) ��
		    
			//����   n*(dim-2)*(dim-2)*1�����ټ��㣬�����õ�Dot_vv�������Ż����ֽ�����30%�ļ���
			//deque<vector<double>> test(X);  getXij(test, i, j);     //   test ��Xȥ�������Ժ�Ľ��      20ms
			//vector <double>xb;  Dot_vv(test, getBij(i, j), xb);      //50ms

			vector<double> tmp;
			
			

			vector<double>xb;  Dot_vv(X, getBij(i, j), xb, i, j);
			
			
			

			for (unsigned k = 0; k < xi.size(); k++)
				tmp.push_back(xi[k] - xb[k]);			 //(n*1) - (n*48).*(48*1)    �õ��ķ��� tmp shape is ��n*1��


			vector<double> xj;
			GetCol_S(X, j,xj);     					//xj  (n*1)
			double part1 = Dot_av(tmp, xj);

			double Denominator = Dot_av(xi, xi);

			if (Denominator == 0){             //  ���������Գ�ȫΪ0��ȷ����ĸ�����Ϊ0
				part1 = part1 / num;
			}
			else{
				part1 = part1 / Denominator;                //��part1 ��ӦB�ĵ�һ����
			}


			//  part2����ô��ʱ��
			double part2;
			if (Denominator == 0){
				part2 = (lamda1 + lamda2*P[i][j]) / num;
			}
			else{
				part2 = (lamda1 + lamda2*P[i][j]) / Denominator;
			}


			//cout << "����part " << part1 << "    " << part2 <<"  ��ʱ��Pij  "<<P[i][j]<<endl;

			if (P[i][j] == 1 && abs(part1) > part2)  cout << "*************�����˱ܻ��ͷ����������������������������ּ�������********************" << endl;

			double sign = Sign(part1);

			//cout << "part1 part2 " << part1 << ' ' << part2 << endl;

			double result;
			if (fabs(part1) - part2 < 0)
				result = 0;
			else
				result = (fabs(part1) - part2)*sign;
			// �� shooting ѵ�������Ȳ��ܹ�ϵ��ֵ������
			if (j < i)
				B[j][i] = result;
			else
				B[j - 1][i] = result;

		}
		GetCol_M(B, i, LastColB);
		double newtemp = Norm(LastColB);
		std::cout << "shooting iter " << i << "��Bi->: " << newtemp << endl;
		
		if (fabs(newtemp - temp) < 0.01)      //�����ֵС��0.01 ����ǰ�˳����������100�ε���
			break;
	}

}


void BCD(int BCDIter)
{
	for (int t = 0; t < BCDIter; t++)
	{
		cout << "������һ��BCD" << endl;
		cout << "�����" << t << "��BCD����................." << endl;
		for (int i = 0; i < dim; i++)		 //������������ Bi
		{
			int64_t start, end;
			start = GetTime();
			vector<double> Pij = BFS(i);
			cout << "��" << t << "��BCD�ĵ�" << i << "ά����............." << endl;
			Shooting(i, 100);
			refreshDAG(i);    //����DAG �� i  ��Ӧ����һ��

			int sss = 0;
			for (int k = 0; k < dim; k++)
			{
				if (DAG[k][i] == 1) sss++;
			}
			cout << "���������з���ֵ" << sss << "��" << endl;

			for (int k = 0; k < dim - 1;k++)
			{
				cout << B[k][i] << ',';
			}
			cout << endl << endl << endl;


			end = GetTime();
			cout << (end - start)/1000 << endl;
		}
	}
}




void CheckAndPrune()  // �� B ��֦������ɻ�    ����һ���ȽϺ��ʵ���ֵ�趨�����Ա�֤����ɻ����ú������Է���һ           ���� ɾ����ϵ��ֵ���µıߣ�����ϡ��ȣ���Ҳһ���̶������˾���
{
	//vector<vector<double>> showB;
	for (int i = 0; i < (dim - 1); i++)
	{
		for (int j = 0; j < dim; j++)
			showB[i][j] = B[i][j];              // �Ȱ�ԭʼ�� B ��ֲ�� showB ����
	}


	for (int i = 0; i < (dim - 1); i++)
	{
		for (int j = i + 1; j < dim; j++)
		{
			if (fabs(showB[i][j]) > fabs(showB[j - 1][i]))
				showB[j - 1][i] = 0;
			else
				showB[i][j] = 0;
		}
	}


	for (int i = 0; i < (dim - 1); i++)  // ��ϵ��ֵ�ж�
	{
		for (int j = 0; j < dim; j++)
			if (fabs(showB[i][j]) < relationThreshold)
				showB[i][j] = 0;
	}

	for (int i = 0; i < dim; i++)		// �����  DAG
	{
		for (int j = 0; j < dim; j++)
			DAG[i][j] = 0;
	}

	for (int i = 0; i < (dim - 1); i++)		 // ���������ˢ��һ�� DAG
	{
		for (int j = 0; j < dim; j++)
		{
			if (showB[i][j] != 0)
			{
				if (i >= j)
					DAG[i + 1][j] = 1;
				else
					DAG[i][j] = 1;
			}
		}
	}
}



void Train()	 // �� BCD ѵ���µ� B, ��黷�͹�ϵ��ֵ��֦�����¶�ά���� DAG
{
	time_t start, end;
	time(&start);
	BCD(10);  // ѵ�������в��� B �� DAG�����Ԥ������ά��ͬ����ѵ�����˽ṹ�Ժ�ʼ��黷����֦
	time(&end);
	cout << "BCD����" << endl;
	CheckAndPrune();
	cout << "��֦����" << endl;
	cout << "ѵ��ʱ��..................................................." << difftime(end, start) << "��" << endl;
}



void update(vector<double> sample)     //��������
{
	rawX.pop_front();
	rawX.push_back(sample);   // ��sample��װ��һ�У�����rawX��һ��ɾ���ٲ������������һ��
	Scale(rawX,X);

	printf("try to update..........\n");

	BCD(1);				// �����Ժ����DAG
	CheckAndPrune();   //���µ� B ���м�飬�����ݹ�ϵ��ֵ�Ȳ������� B �� DAG

	printf("update end...............\n");
}

/****************************************��ӡ***************************************************/
void printDAG()  // ��� DAG
{
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			if (DAG[i][j] == 1)			// print('���ڱ� ', i + 1, j + 1)
				cout << "G[" << i+1 << "]" << "[" << j+1 << "]" << "= 1" << endl;
			//print('G[', i, ',', j, ']=1');
		}
	}
}

void printB()
{
	for (int i = 0; i < dim - 1; i++)
	{
		for (int j = 0; j < dim; j++)
			cout << B[i][j] << " , ";
		cout << endl;
	}
}

void printShowB()
{
	for (int i = 0; i < dim - 1; i++)
	{
		for (int j = 0; j < dim; j++)
		
			cout << showB[i][j] << " , ";
		cout << endl;
	}
}


double Expection(int x)   // ���� x ������
{
	vector<double>Xx;
	GetCol_S(X, x,Xx);
	double total = 0;
	for (unsigned i = 0; i < Xx.size(); i++)
		total += Xx[i];  // �ۼӵڶ���
	return total / num;
}

double Cov(int x, int y)
{
	vector<double>mul;
	vector<double>Xx, Xy;
	GetCol_S(X, x,Xx);
	GetCol_S(X, y,Xy);
	for (unsigned i = 0; i < Xy.size(); i++)			//���x,y��
		mul.push_back(Xx[i] * Xy[i]);
	//mul=self.X[:,x]*self.X[:,y]
	double total = 0;
	for (unsigned i = 0; i < mul.size(); i++)		//mul���
		total += mul[i];
	total = total / num;   //  ���� E[XY]
	double covXY = total - Expection(x)*Expection(y);
	return covXY;
}

double Parameter(int x){         //���� P(X| u1 u2 u3 u4.......) �����Ը�˹����Ӧ�ķ���
	double temp = 0;
	int posi = 0, posj = 0;
	for (int i = 0; i < dim - 1; i++)
		for (int j = 0; j < dim - 1; j++){
			if (i >= x) posi = i + 1;
			else posi = i;

			if (j >= x) posj = j + 1;
			else posj = j;
			temp += showB[i][x] * showB[j][x] * Cov(posi, posj);
		}
	double sigma2 = Cov(x, x) - temp;
	return sigma2;
}


/*
int main(){
	
	
	vector<int>aa,bb;
	aa.push_back(3);
	aa.push_back(4);
	aa.push_back(7);

	vector<int>::iterator it = find(aa.begin(), aa.end(), 4);
	if (it != aa.end()){
		aa.erase(it);
		cout << "yeah" << endl;
		for (int k = 0; k < aa.size(); k++) cout << aa[k] << ' ';
	}
	else cout << "fuck" << endl;




	/*
	SBN("D:\\data8000.csv",62,1.0,0.02);
	train();

	printDAG();
	cout << "--------------------------------------------" << endl;
	printB();
	cout << "********************************************" << endl;
    printShowB();
	*/
	/*
	int num = 0;
	for (int i = 0; i < sbn.B.size(); i++)
		for (int j = 0; j < sbn.B[0].size(); j++)
		{
			if (sbn.B[i][j] != 0) num++;
		}
	cout << "Mat B nonzeros " << num << endl;

	num = 0;
	for (int i = 0; i < sbn.showB.size(); i++)
		for (int j = 0; j < sbn.showB[0].size(); j++)
		{
			if (sbn.showB[i][j] != 0) num++;
		}
	cout << "Mat showB nonzeros " << num << endl;

	num = 0;
	for (int i = 0; i < sbn.DAG.size(); i++)
		for (int j = 0; j < sbn.DAG[0].size(); j++)
		{
			if (sbn.DAG[i][j] != 0) num++;
		}
	cout << "Mat DAG nonzeros " << num << endl;
	*/
//	return 0;

