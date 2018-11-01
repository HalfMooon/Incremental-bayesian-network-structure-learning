#include"BatchSparseBN.h"
#include"SparseBN.h"
#include"utils.h"

using namespace std;


double batchLamda1;
double batchLamda2;
double batchRelationThreshold;


int batchNumber;   //����

void Split(deque<vector<double>>&rawData, deque<vector<double>>&Data, int start, int end)    //��ȫ�����ֳɶ�Ӧλ�õ�����
{
	assert(start >= 0);
	assert(end < rawData.size());
	Data.clear();
	for (int i = start; i < end; i++)
	{
		Data.push_back(rawData[i]);
	}
}



void BatchSBN(char* filename, int d, double penalty, double relation, int batch)
{

	dim = d;
	batchNumber = batch;
	batchRelationThreshold = relation;
	rawX = loadtxt(filename, dim);

	Split(rawX, X, 0, rawX.size() / batch);    //ȡǰʮ��֮һ����

	Scale(rawX, X);   //����
	cout << "��һ��������.............." << endl;
	for (int i = 0; i < dim; i++)
	{
		cout << X[0][i] << " ";
	}
	num = X.size();
	B = Zero(dim - 1, dim);     //(p-1)*p ��
	showB = Zero(dim - 1, dim);         //������ʾ DAG �õ� B����ԭʼB����̭��ϵ��ֵ���µģ���������Ӧ��֦����֮ DAG ����� showB �Ƕ�Ӧ�ģ����������Ҳ�ǻ������showB�� ��ֱ�Ӷ�B��֦������B����������ʱ�������ۼ�
	batchLamda1 = penalty*(0.12*num);    			//penalty
	batchLamda2 = (((num - 1)*(num - 1)*dim) / batchLamda1 - batchLamda1)*1.2;

	DAG = Zero(dim, dim);     //DAG
	P = Zero(dim, dim);
}

vector<double> BatchBFS(int i){
	//�õ�DAG��iΪ���ıհ�
	for (int j = 0; j < dim; j++)
		P[i][j] = 0;
	queue<double> q;
	q.push(i);
	while (!q.empty())
	{
		double node = q.front();
		q.pop();
		if (P[i][node] == 0)
		{
			P[i][node] = 1;
			for (int j = 0; j < dim; j++)
			{
				if (DAG[node][j] == 1 && P[i][j] == 0)
					q.push(j);
			}
		}
	}
	return P[i];    //����P����һ�У�ȷ���հ�
}


void BatchRefreshDAG(int i){                       //BCD ��50άÿһά������ʱ��ˢ�¶�Ӧ�е�DAG������BFS����Ч
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


vector<double> BatchGetBij(int i, int j)
{
	vector<double>result;
	result.clear();
	RemoveElem(B, i, j, result);
	return result;
}



void BatchGetXij(deque<vector<double>>&result, int dim1, int dim2)
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


void BatchShooting(int i, int shootIter)
{
	vector<double> xi;

	GetCol_S(X, i, xi);


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

			vector<double>xb;  Dot_vv(X, BatchGetBij(i, j), xb, i, j);


			for (unsigned k = 0; k < xi.size(); k++)
				tmp.push_back(xi[k] - xb[k]);			 //(n*1) - (n*48).*(48*1)    �õ��ķ��� tmp shape is ��n*1��


			vector<double> xj;
			GetCol_S(X, j, xj);     					//xj  (n*1)
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
				part2 = (batchLamda1 + batchLamda2*P[i][j]) / num;
			}
			else{
				part2 = (batchLamda1 + batchLamda2*P[i][j]) / Denominator;
			}

			double sign = Sign(part1);

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
		//std::cout << "����shooting���" << i << "�ε�����Bi�����: " << newtemp << endl;
		if (fabs(newtemp - temp) < 0.001)      //�����ֵС��0.01 ����ǰ�˳����������100�ε���
			break;
	}

}


void BatchBCD(int BCDIter)
{
	for (int t = 0; t < BCDIter; t++)
	{
		cout << "�����"<<t<<"��BCD����................." << endl;
		for (int i = 0; i < dim; i++)		 //������������ Bi
		{
			vector<double> Pij = BatchBFS(i);
			cout << "��" << t << "��BCD�ĵ�" << i << "ά����............." << endl;
			BatchShooting(i, 100);
			BatchRefreshDAG(i);    //����DAG �� i  ��Ӧ����һ��
		}
	}
}


void BatchCheckAndPrune()  // �� B ��֦
{
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
			if (fabs(showB[i][j]) < batchRelationThreshold)
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


void BatchTrain()
{
	time_t start, end;
	time(&start);
	BatchBCD(5);   //�ȶԵ�һ������ѵ��
	
	for (int i = 1; i < batchNumber; i++)
	{
		cout << "*******************************��ʼ��" << i + 1 << "�����ݵ�ѵ��*********************************" << endl;
		//Split(rawX, X, (int)(i*(int)rawX.size() / batchNumber), (int)((i + 1)*(int)rawX.size() / batchNumber));
		Split(rawX, X, (int)((i - 1)*num + (num / 2)),(int)(i*num + (num / 2)));
		
		Scale(rawX, X);

		BatchBCD(2);
		cout << "***********************************��һ��**************************" << endl;
	}
	
	cout << "****************************************�������������ٴ�ѭ��һ��***********************************" << endl;
	X.clear();
	X = rawX;
	Scale(rawX, X);

	batchLamda1 = batchLamda1*batchNumber;
	num = X.size();
	batchLamda2 = (((num - 1)*(num - 1)*dim) / batchLamda1 - batchLamda1)*1.1;


	BatchBCD(10);
	time(&end);
	cout << "Batchѵ��ʱ��..................................................." << difftime(end, start) << "��" << endl;
	BatchCheckAndPrune();
	
}

