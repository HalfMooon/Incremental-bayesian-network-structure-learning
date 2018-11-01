#include"utils.h"

using namespace std;



vector<double>meanList;             //ѵ��������ֵ
vector<double>standardList;         //ѵ����������



int64_t GetTime()
{
#ifdef _WIN32
	// ��1601��1��1��0:0:0:000��1970��1��1��0:0:0:000��ʱ��(��λ100ns)
#define EPOCHFILETIME   (116444736000000000UL)
	FILETIME ft;
	LARGE_INTEGER li;
	int64_t tt = 0;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	// ��1970��1��1��0:0:0:000�����ڵ�΢����(UTCʱ��)
	tt = (li.QuadPart - EPOCHFILETIME) / 10;
	return tt;
#else
	timeval tv;
	gettimeofday(&tv, 0);
	return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
#endif 
	return 0;
}




/*����һЩ�����Ժ���*/

double S2D(const string str)
{
	istringstream iss(str);
	double num;
	iss >> num;
	return num;
}

deque<vector<double>> loadtxt(const char* filename, int d)
{
	ifstream inFile(filename, ios::in);
	if (!inFile.is_open())
	{
		cout << "Error opening file"; exit(1);
	}

	string lineStr;
	deque<vector<double>> strArray;		//RawX

	string FeatureName;
	getline(inFile, FeatureName);           //�����һ����������������ã��˴�ɾ����һ��


	while (getline(inFile, lineStr))	//����Ϊ��λ����lineStr
	{
		stringstream ss(lineStr);
		string str;
		vector<double> lineArray;  	//RawX��������
		while (getline(ss, str, ','))	//�ԣ�Ϊ�ָ��ss��str
		{
			lineArray.push_back(S2D(str));

		}
		lineArray.erase(lineArray.begin());     //ȥ����һ��
		strArray.push_back(lineArray);
	}
	return strArray;
}

vector<vector<double>> loadShowB(const char* filename, int d)
{
	ifstream inFile(filename, ios::in);
	if (!inFile.is_open())
	{
		cout << "Error opening file"; exit(1);
	}

	string lineStr;
	vector<vector<double>> strArray;		//RawX

	while (getline(inFile, lineStr))	//����Ϊ��λ����lineStr
	{
		stringstream ss(lineStr);
		string str;
		vector<double> lineArray;  	//RawX��������
		while (getline(ss, str, ','))	//�ԣ�Ϊ�ָ��ss��str
		{
			lineArray.push_back(S2D(str));

		}
		strArray.push_back(lineArray);
	}

	return strArray;
}

double Sign(double op)
{
	double result;
	if (op>0)
		result = 1.0;
	else if (op<0)
		result = -1.0;
	else
		result = 0;
	return result;
}

double Shape(vector<double> lineStr)
{
	double r = lineStr.size() - 1;
	return r;
}


void GetColData(const deque<vector<double>>&rawX, unsigned i,vector<double>&data)	//ΪScale����
{
	data.clear();
	for (unsigned j = 0; j < rawX.size(); j++)
		data.push_back(rawX[j][i]);

}


double Average(vector<double>data)	//ΪScale����
{
	double sum = 0;
	for (unsigned i = 0; i < data.size(); i++)
	{
		sum += data[i];
	}
	return sum / data.size();

}


double Standard(vector<double>data, double mean)	//ΪScale����
{
	double sum = 0;
	for (unsigned i = 0; i < data.size(); i++)
	{
		sum += pow((data[i] - mean), 2);
	}
	return sqrt(sum / data.size());
}


//   ���򻯽ӿ�
void Scale(const deque<vector<double>>& rawX,deque<vector<double>>&X)
{
	if (X.size() <= 0){
		cerr << "no data !" << endl;
	}
	double mean, standard;
	for (unsigned i = 0; i < X[0].size(); i++)
	{

		vector<double>col;
		GetColData(X, i,col);
		mean = Average(col);
		standard = Standard(col, mean);

		meanList.push_back(mean);
		standardList.push_back(standard);

		for (unsigned j = 0; j < X.size(); j++)
		{

			if (standard != 0){
				if (fabs(X[j][i] - mean) < 0.00001)
				{
					X[j][i] = 0;
				}
				else
				{
					X[j][i] = (X[j][i] - mean) / standard;
				}
			}
			else
			{
				if (fabs(X[j][i] - mean) <0.00001)
				{
					X[j][i] = 0;
				}
				else
				{
					X[j][i] = (X[j][i] - mean);
				}
			}
			
		}
	}
	/*
	for (int i = 0; i < X.size(); i++)
	{
		for (int j = 0; j < X[i].size(); j++)
		{
			cout << X[i][j] << ' ';
		}
		cout << endl;
	}
	cout << "mean and standard..." << endl;
	for (int i = 0; i < meanList.size(); i++)
	{
		cout << meanList[i] << ' ';
	}
	cout << endl;
	for (int j = 0; j < standardList.size(); j++)
	{
		cout << standardList[j] << ' ';
	}
	cout << endl;
	*/
}


//   ����x*yά��0����
vector<vector<double>> Zero(int x, int y)
{
	vector<double> temp(y, 0);
	vector<vector<double>>arr(x, temp);
	return arr;
}


// ɾ����Ӧ�У��±��0��ʼ
void RemoveCol(deque<vector<double>>&data, int d)
{
	for (unsigned i = 0; i < data.size(); i++)
	{
		data[i].erase(data[i].begin() + d);
	}
}


void RemoveCols(deque<vector<double>>&data, int dim1, int dim2)      //�������ֻ��Ҫ�Ƴ�����
{
	int k;
	if (dim1 < dim2){ k = dim1; dim1 = dim2; dim2 = k; }
	for (unsigned i = 0; i < data.size(); i++)
	{
		data[i].erase(data[i].begin() + dim1);
	}
	for (unsigned i = 0; i < data.size(); i++)
	{
		data[i].erase(data[i].begin() + dim2);
	}
}


void RemoveElem(const vector<vector<double>>& B, int i, int j,vector<double>&result)
{    
	result.clear();
	for (unsigned k = 0; k < B.size(); k++){
		result.push_back(B[k][i]);     //��ȡ��i��
	}
	if (j < i){
		result.erase(result.begin() + j);
	}
	else{
		result.erase(result.begin() + (j - 1));
	}
}


double Norm(vector<double> sample)
{
	double sum = 0.0;
	for (unsigned i = 0; i < sample.size(); i++)
	{
		sum += sample[i] * sample[i];
	}
	return sqrt(sum);
}


/*����������Dot���������غ�����һ��������������������ģ�����һ��double������һ�����������������ģ�����һ�����������߶��Ǿ���˷����������*/

//        n*p   *     p*1 �����������һ��double����result ά��Ϊn*1
//     Dot_vv(array a,vector b,vector result)
void Dot_vv(const deque<vector<double>>&a, const vector<double>&b, vector<double>&result)
{
	result.clear();
	for (unsigned i = 0; i < a.size(); i++)
	{
		double sum = 0.0;
		if (a[i].size() != b.size()){
			cerr << "a[i] & b don't have same dim !" << endl;
			exit(1);   //  ǿ���˳�
		}

		for (unsigned j = 0; j < a[i].size(); j++)
		{

			sum += a[i][j] * b[j];
		}

		result.push_back(sum);
	}

}



void Dot_vv(const deque<vector<double>>&a, const vector<double>&b, vector<double>&result,int col1,int col2) //������a��col1��col2���ų���Ȼ���b���о���˷�������ŵ�result����
{
	result.clear();
	for (unsigned i = 0; i < a.size(); i++)
	{
		int count = 0;
		int k = 0;
		double sum = 0.0;
		if (a[i].size() != b.size() + 2)
		{
			cerr << "a[i] & b don't have same dim !" << endl;
			exit(1);
		}
		for (unsigned j = 0; j < a[i].size(); j++)
		{
			if (j == col1 || j == col2) {
				k++;
				continue;
			}
			if (b[j - k] == 0) continue;
			sum += a[i][j] * b[j-k];
		}
		result.push_back(sum);
	}
}


//  �����Ǹ�ֵ���ٶ�Ӱ�첻��
double Dot_av(const vector<double>&a, const vector<double>&b)
{
	if (a.size() != b.size())
	{
		cerr << "a&b don't have same dim !" << endl;
		//return 0;
		exit(1);
	}
	double sum = 0.0;
	for (unsigned i = 0; i < a.size(); i++)
	{

		sum += a[i] * b[i];
	}

	return sum;
}


/*���python�б���  B[:,i]���������ȡĳ������ĵ�i��,ͬ���������������һ������������ģ�һ�������������B,P��������� ��dim��0��ʼ */
void GetCol_S(const deque<vector<double>>&data, int d,vector<double>&result)
{
	result.clear();
	for (unsigned i = 0; i < data.size(); i++)     //����ÿһ������
		result.push_back(*(data[i].begin() + d));

}


void GetCol_M(const vector<vector<double>>&data, int d, vector<double>&result)
{
	result.clear();
	for (unsigned i = 0; i < data.size(); i++){     //����ÿһ������
		result.push_back(data[i][d]);
	}
}



void comb(vector<int>list,int s, int n, int m,vector<vector<int>>&result,vector<int>&condition)
{
	int i;

	if (s > n)
		return;

	if (condition.size() == m)
	{
		result.push_back(condition);
		return;
	}

	condition.push_back(s);
	comb(list,s + 1, n, m,result,condition);
	condition.pop_back();
	comb(list,s + 1, n, m,result,condition);
}



vector<vector<int>>Combinations(vector<int>list, int len){     //���list�г���Ϊlen���������

		int i;
		vector<vector<int>>result;
		vector<int>condition;
		if (len > list.size())
		{
			cerr << "���볤�ȹ���!" << endl;
			//return result;
			exit(1);
		}
		comb(list, 0, (int)list.size(), len, result, condition);
		return result;
}


