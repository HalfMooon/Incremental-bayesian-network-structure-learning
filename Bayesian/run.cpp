#include"utils.h"
#include"SparseBN.h"
#include"Reason.h"
#include"batchSparseBN.h"
#include<map>
#include <iomanip>

using namespace std;


map<double, double> loadMap(const char* filename)
{
	ifstream inFile(filename, ios::in);
	if (!inFile.is_open())
	{
		cout << "Error opening file"; exit(1);
	}

	string lineStr;
	map<double, double> strMap;
	while (getline(inFile, lineStr))	//����Ϊ��λ����lineStr
	{
		stringstream ss(lineStr);
		string key, value;
		getline(ss, key, ':');	//�ԣ�Ϊ�ָ��ss��str
		getline(ss, value);
		strMap[S2D(key)] = S2D(value);
	}
	return strMap;
}




int main(){

	
	//cout << "��ȡʱ��..." << ((double)end - (double)start)/1000<< endl;
	
	//SBN("E:\\fangtian\\S410039\\DataDim57-new.csv", 56, 2, 0.1);     //��ʼ������������csv�ĵ�һ�к͵�һ�У�һ������������һ�������ڣ�

	/*    SBN(path,dim,penalty,ralationthreshold)       */


	string path;
	int dims=0;
	double penalty=1.0, relation=0.1;
	string param_txt;
	string sigma2_txt;
	string edge_txt;
	cout << "file path: ";
	cin >> path;
	cout << "dim: ";
	cin >> dims;
	cout << "penalty param:";
	cin >> penalty;
	cout << "relation threshold: ";
	cin >> relation;
	cout << "param matrix save path:(such as��E:\\param.txt)";
	cin >> param_txt;
	cout << "sigma2 save path:(such as��E:\\sigma2.txt)";
	cin >> sigma2_txt;
	cout << "edge save path:(such as: E:\\edge.txt)";
	cin >> edge_txt;
	
	cout << "loading data....." << endl;
	SBN((char*)path.data(), dims, penalty, relation);


	Train();
	cout << "*******************************computing sigma2*****************************************" << endl;
	vector<double>sigma2=getSigma2();

	cout << "param is      " << path.data() << "  dim is" << dim << " penalty " << penalty << " relation threshold  " << relation<<endl;

	cout << "**********************************DAG**********************************" << endl;
	printDAG();
	cout << endl << endl;
	cout << "************************************B*******************************" << endl;
	printB();
	cout << endl << endl;
	cout << "**********************************showB********************************" << endl;
	printShowB();

	int edge = 0;
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			if (DAG[i][j] == 1)
				edge++;
		}
	}

	ofstream MatrixB;
	MatrixB.open(param_txt);


	ofstream Sigma;
	Sigma.open(sigma2_txt);

	ofstream Edge;
	Edge.open(edge_txt);

	for (int i = 0; i < dim - 1; i++)
	{
		for (int j = 0; j < dim-1; j++)
		{
			MatrixB << showB[i][j] << ",";
		}
		MatrixB << showB[i][dim-1];
			MatrixB << "\n";
	}



	for (int i = 0; i < sigma2.size()-1; i++)
	{
		Sigma << sigma2[i] << ",";
	}
	Sigma << sigma2[sigma2.size() - 1];



	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			if (DAG[i][j] == 1)			// print('���ڱ� ', i + 1, j + 1)
				Edge << "G(" << i + 1 << ")" << "(" << j + 1 << ")" << "= 1" << endl;
		}
	}
	MatrixB.close();
	Sigma.close();
	Edge.close();

	cout << "**********************************ͳ�ƽ��Ϊ*******************************" << endl;
	cout << "node: " << dim << "edge: " << edge << endl;
	cout << "��������������ֹ......" << endl;
	int tmp;
	cin >> tmp;

	/**
	cout << "�����������ֽ����ƶ�" << endl;
	cin >> qq;
	
	cout << "***********************************��������ƶ�**************************************" << endl;
	
	getSample(rawX[10]);    //��ԭʼ���ݵ�һ��Ͷ�룬�õ����򻯽�����sampleList
	cout << "*******************************�ɹ���ȡ���������*************************************" << endl;
	getSampleData(X);      //  �����򻯵�ѵ���������ϴ���������ټ���������ֵ����ɢ���õ�DiscreX
	cout << "******************************************�ɹ���ɢ��**********************************" << endl;
	getSigma2();
	cout << "******************************************�ɹ���ȡsigma*******************************" << endl;
	loopStart(0);

	cout << "**********************************�쳣·��*************************************" << endl;
	showPath();
	**/
	
}

