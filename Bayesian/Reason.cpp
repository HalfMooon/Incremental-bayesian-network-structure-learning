#include"SparseBN.h"
#include"utils.h"
#include"Reason.h"
using namespace std;

map<int, vector<int>>path;    //·���ֵ�

vector<vector<double>>DiscreX;     //��Ӧpython�汾��dataListNew

vector<double>sigma2;     //��SpareBN�л�ȡsimga2


vector<double>sampleList; //���������




void getSample(vector<double>Sample)      //�������Ĵ������������,  ��ɢ����ȫ��Ĺ���
{
	double temp;
	for (int i = 0; i < dim; i++)
	{
		temp = (Sample[i] - meanList[i]) / standardList[i];
		sampleList.push_back(temp);
	}
}

void getSampleData(deque<vector<double>>&dataList)    //������ɢ����ʮ�ȷ�
{
	vector<double>Max;  //�洢����ά�ȵ���ֵ
	vector<double>Min;
	int i, j, x;
	double FeatMax, FeatMin;

	for (i = 0; i < dim; i++)
	{
		FeatMax = dataList[0][i];
		FeatMin = dataList[0][i];
		for (j = 0; j < dataList.size(); j++)
		{
			if (dataList[j][i]>FeatMax) FeatMax = dataList[j][i];
			if (dataList[j][i]<FeatMin) FeatMin = dataList[j][i];
		}
		if (sampleList[i]>FeatMax) FeatMax = sampleList[i];
		if (sampleList[i]<FeatMin) FeatMin = sampleList[i];

		Max.push_back(FeatMax);     //��ά���������ֵ
		Min.push_back(FeatMin);     //��ά��������Сֵ
	}

	vector<double>DiscreSample;
	for (i = 0; i < dataList.size(); i++)   //����ÿһ������i
	{
		for (j = 0; j < dataList[i].size(); j++)   //����ÿһ������ j
		{
			for (x = 0; x < 10; x++)
			{
				if (Min[j] + (x + 1)*(Max[j] - Min[j]) / 10 >= dataList[i][j])
				{
					DiscreSample.push_back(x + 1);
					break;
				}
			}
		}
		DiscreX.push_back(DiscreSample);   //�����ɢ������������
		DiscreSample.clear();
	}

	DiscreSample.clear();
	
	for (i = 0; i < sampleList.size(); i++)   //����ÿһ������ j
	{
		for (x = 0; x < 10; x++)
		{
			
			if (Min[i] + (x + 1)*(Max[i] - Min[i]) / 10 >= sampleList[i])
			{
				DiscreSample.push_back(x + 1);
				break;
			}
		}
	}
	DiscreX.push_back(DiscreSample);   //�����ɢ������������

	
	sampleList = DiscreSample;
	// �ٰѸ�ά��ֵ��Ϊ������м�������    ����DiscreX��һ��[num+1������⣩+2����ֵ��]*dim��һ������
	DiscreX.push_back(Max);
	DiscreX.push_back(Min);
	
	//cout << "��������Ϊ***********************************************************" << endl;
	//for (int k = 0; k < DiscreX.size(); k++)
	//{
	//	for (int j = 0; j < DiscreX[k].size(); j++)
	//	{
	//		cout << DiscreX[k][j] << ' ';
	//	}
	//	cout << DiscreX[k].size();
	//	cout << endl;
	//}
	
}


void loopStart(int i)
{
	cout << "������loopStart....................." << i<<endl;

	vector<double>parentNode = getParent(i);    //���showB������i��Ӧ�е���չ������50*1��

	vector<int>omegaNodeNumber = getOmegRank(parentNode);
	if (omegaNodeNumber.size() > 0){
		loopHead(omegaNodeNumber, sampleList, parentNode, i);
	}
	else{
		vector<int>p;
		p.push_back(-1);
		path[i] = p;       //û�и��׵Ļ�   ��-1
		cout << "���" << i << "�޸��ڵ�" << endl;
	}
}


vector<double>getParent(int x)    //��ȡB�����ض��й��ɵ�����,�Լ�ҲҪ���䣬����dimά
{

	vector<double>list1;
	for (int i = 0; i < dim; i++)
	{
		if (i < x) list1.push_back(showB[i][x]);
		else if (i == x) list1.push_back(0.0);
		else list1.push_back(showB[i - 1][x]);
	}

	for (int i = 0; i < list1.size(); i++)
	{
		cout << list1[i] << ' ';
	}
	cout << endl;
	return list1;
}


vector<int>getOmegRank(vector<double> parentNode)
{
	vector<int>parentNodeNumber;
	for (int i = 0; i < parentNode.size(); i++)
	{
		if (parentNode[i] != 0)
			parentNodeNumber.push_back(i);
	}
	return parentNodeNumber;
}

void loopHead(vector<int>omegaNodeNumber, vector<double>sampleList, vector<double>parentNode, int k)    //omegaNodeNumber��int�͵ĸ��׼��� sampleList�Ǵ������������parentNode��showB�ж�Ӧ�е�����
{
	cout << endl;
	cout << "************************׷�ٵ�" << k << "ά��Ӱ�츸�ڵ�*****************************" << endl;
	if (path.count(k) != 0){
		cout << "---------------" << k << "ά�Ѿ�����-----------------" << endl;
		return;
	}
	vector<double>a;
	map<vector<int>, double>tauCList;    //���岻���
	for (int i = 0; i < omegaNodeNumber.size(); i++)
	{
		vector<vector<int>>combins = Combinations(omegaNodeNumber, i + 1);
		cout << "�������C" << omegaNodeNumber.size() << ',' << i + 1 << endl;
		for (int j = 0; j < combins.size(); j++)
		{
			for (int p = 0; p < combins[j].size(); p++) cout << combins[j][p] << ' ';
			cout << endl;
		
			loopMain(combins[j], omegaNodeNumber, sampleList, parentNode, tauCList, k);
			
		}
	}

	//  ������ɢ��
	double jDiscrete = Discrete(sampleList[k], k);
	double min = abs(tauCList.begin()->second - jDiscrete);



	vector<int> keyValue;

	for (map<vector<int>, double>::iterator it = tauCList.begin(); it != tauCList.end(); it++)   //Ѱ��һ��ʲô��Сֵ
	{
		
		if (abs(it->second - jDiscrete) <= min)
		{
			min = abs(it->second - jDiscrete);
			keyValue = it->first;
		}
		
	}
	path[k] = keyValue;       
	
	for (int i = 0; i < keyValue.size(); i++)
	{
		if (keyValue[i] >= dim)
		{
			break;
		}
		loopStart(keyValue[i]);
	}
}


void loopMain(const vector<int>&loopNumber, const vector<int>&omegaNodeNumber, const vector<double>&sampleList, const vector<double>&parentNode, map<vector<int>, double>&tauCList, int k)
{
	vector<int>tauLoopNumber;    //  combins[j]��ӦomegaNodeNumber�������
	vector<double>tauDiscreteList;   //  ��Ӧ��tau��ɢ����ֵ
	for (int i = 0; i < loopNumber.size(); i++)
	{
		tauLoopNumber.push_back(omegaNodeNumber[loopNumber[i]]);
		
	}


	cout << sampleList.size() << endl;
	cout << endl;
	for (int i = 0; i < tauLoopNumber.size(); i++)
	{
		tauDiscreteList.push_back(sampleList[tauLoopNumber[i]]);                   //  tauLoopNumber ̫����
	}

	vector<int>tauSumNumberList = getSumSample(tauDiscreteList, tauLoopNumber);



	vector<int>omegaMinusTao = getOmegaMinusTao(tauLoopNumber, omegaNodeNumber);

	
	map<vector<double>, int>omegaMinusTauDict = getProbability(tauSumNumberList, tauDiscreteList, tauLoopNumber, omegaNodeNumber);
	map<double, double>muBeitaDict;
	double sum = 0;
	double sum1 = 0;
	for (int i = 0; i < tauLoopNumber.size(); i++)
	{
		sum1 = parentNode[tauLoopNumber[i]] * tauDiscreteList[i];
	}


	for (map<vector<double>, int>::iterator it = omegaMinusTauDict.begin(); it != omegaMinusTauDict.end(); it++)
	{
		sum = sum1;
		for (int j = 0; j < (it->first).size(); j++){
			sum += (it->first)[j] * parentNode[omegaMinusTao[j]];
		}
	
		muBeitaDict[sum] = (double)((double)omegaMinusTauDict[it->first] / (double)tauSumNumberList.size());
	}
	
	double cvalue = xZero(muBeitaDict, sigma2[k]);

	tauCList[tauLoopNumber] = cvalue;
}


vector<int> getSumSample(const vector<double>&tauDiscreteList, const vector<int>&tauLoopNumber)
{
	
	int row = DiscreX.size() - 2;
	set<vector<double>>tauSumSet;               //  ��ͬ˳���vector<double>��֪���㲻�㲻ͬ�ĳ�Ա
	tauSumSet.insert(tauDiscreteList);
	vector<int>tauSumNumberList;
	for (int i = 0; i < row; i++)
	{
		vector<double>tempList;
		for (int j = 0; j < tauLoopNumber.size(); j++)
		{
			tempList.push_back(DiscreX[i][tauLoopNumber[j]]);
		}
		
		if (tauSumSet.count(tempList) != 0)
		{
			
			tauSumNumberList.push_back(i);
		}
		
	}

	return tauSumNumberList;

}



/*
vector<int> getSumSample(const vector<double>&tauDiscreteList, const vector<int>&tauLoopNumber)
{

	int row = DiscreX.size() - 2;
	for (int i = 0; i < tauDiscreteList.size(); i++)
	{
		cout << tauDiscreteList[i] << ' ';
	}
	cout << endl;
	vector<int>tauSumNumberList;
	for (int i = 0; i < row; i++)
	{
		vector<double>tempList;
		for (int j = 0; j < tauLoopNumber.size(); j++)
		{
			tempList.push_back(DiscreX[i][tauLoopNumber[j]]);
		}

		if (tempList==tauDiscreteList)
		{
			cout << "�Ѿ�ѹ��***************************" << endl;
			tauSumNumberList.push_back(i);
		}
		else { cout << "meiyou fdfsdg*******************" << endl; }
	}

	return tauSumNumberList;

}
*/
vector<int> getOmegaMinusTao(const vector<int>&tauLoopNumber, const vector<int>&omegaNodeNumber)     //ûд��
{
	vector<int>omegaNodeNumberTmp;
	for (int i = 0; i < omegaNodeNumber.size(); i++)
	{
		omegaNodeNumberTmp.push_back(omegaNodeNumber[i]);
	}

	for (int j = 0; j < tauLoopNumber.size(); j++)
	{
		vector<int>::iterator it = find(omegaNodeNumberTmp.begin(), omegaNodeNumberTmp.end(), tauLoopNumber[j]);
		if (it != omegaNodeNumberTmp.end())              //  vector�Ĳ���  �ҵ����ص�������Ӧλ�ã�û�еĻ�����end()
		{
			omegaNodeNumberTmp.erase(it);      //  tauLoopNumber��omegaNodeNumberTmp�еĻ���ɾ��
		}
	}
	return omegaNodeNumberTmp;
}



double Evalf(map<double,double>&muBeitaDict,double sigma2,double x)
{
	
	double f = 0;
	for (map<double, double>::iterator it = muBeitaDict.begin(); it != muBeitaDict.end(); it++)
	{
		f = f - (x - float(it->first)) / ((sqrt(2 * PI*sigma2))*sqrt(sigma2))*exp(-pow(x - float(it->first), 2) / (2 * sigma2))*muBeitaDict[it->first];    //   it->first==key   it->second==value
	}
	return f;
}



double xZero(map<double, double>&muBeitaDict, double sigma2)
{
	
	double f = 0.0;


	vector<double>keyList;

	for (map<double, double>::iterator it = muBeitaDict.begin(); it != muBeitaDict.end(); it++)
	{
		
		keyList.push_back(it->first);
	}

	if (keyList.size() == 0) {
		 return 0.0;
	}

	double maxmiu = keyList[0];
	double minmiu = keyList[0];
	for (int k = 0; k < keyList.size(); k++){
		if (keyList[k] > maxmiu) maxmiu = keyList[k];
		if (keyList[k] < minmiu) minmiu = keyList[k];
	}

	double midmiu = 0.0;

	
	double high = Evalf(muBeitaDict, sigma2, maxmiu);
	double low = Evalf(muBeitaDict, sigma2, minmiu);

	


	int i = 0;
	while (high*low < 0)
	{
		midmiu = (maxmiu + minmiu) / 2;
		if (abs(midmiu - maxmiu) < 0.000000001)
			break;

		double middle = Evalf(muBeitaDict,sigma2,midmiu);
		if (low*middle < 0)
		{
			maxmiu = midmiu;
			high = Evalf(muBeitaDict, sigma2, maxmiu);
			i = i + 1;
		}
		else if (middle*high < 0)
		{
			minmiu = midmiu;
			low = Evalf(muBeitaDict, sigma2, minmiu);
			i = i + 1;
		}
	}

	return midmiu;
}


double Discrete(double number, int col)
{
	int row = DiscreX.size();
	for (int x = 0; x < 10; x++)
	{
		if (DiscreX[row - 1][col] + (x + 1)*(DiscreX[row - 2][col] - DiscreX[row - 1][col]) / 10 >= number)
		{
			number = x + 1;
			break;
		}
	}
	return number;
}

map<vector<double>, int>getProbability(const vector<int>&tauSumNumberList, const vector<double>&tauDiscreteList, const vector<int>&tauLoopNumber, const vector<int>&omegaNodeNumber)
{
	vector<int>omegaMinusTao = getOmegaMinusTao(tauLoopNumber, omegaNodeNumber);
	vector<vector<double>>probabilityData = getProbabilityData(tauSumNumberList);

	set<vector<double>>key;
	map<vector<double>, int>dictory;
	vector<vector<double>>tmplist1;
	for (int i = 0; i < probabilityData.size(); i++)
	{
		vector<double>tmplist2;
		for (int j = 0; j < omegaMinusTao.size(); j++)
		{
			tmplist2.push_back(probabilityData[i][j]);
		}
		tmplist1.push_back(tmplist2);

		if (key.count(tmplist2) == 0){     //not in key
			dictory[tmplist2] = 1;
			key.insert(tmplist2);
		}
		else
		{
			dictory[tmplist2] = dictory[tmplist2] + 1;
		}
	}
	
	return dictory;
}


vector<vector<double>>getProbabilityData(const vector<int>&tauSumNumberList)
{
	
	vector<vector<double>>probabilityData;
	for (int i = 0; i < tauSumNumberList.size(); i++)
	{
		probabilityData.push_back(DiscreX[tauSumNumberList[i]]);      // DiscreX����ɢ���������+���������+�����Сֵ��Ӧ�þ��Ƕ�Ӧpython��dataListNew
	}
	return probabilityData;
}


void showPath()     //��ӡpath
{
	map<int, vector<int>>::iterator it = path.begin();
	for (; it != path.end(); it++)
	{
		cout << it->first << ':';
		for (int i = 0; i < it->second.size(); i++)
		{
			cout << it->second[i] << ' ';
		}
		cout << endl;
	}
}


vector<double> getSigma2()     //�½��õ���������Sigma2
{
	double temp = 0.0;
	for (int i = 0; i < dim; i++)
	{
		temp = Parameter(i);
		sigma2.push_back(temp);
	}

	cout << "the sigma of every node is "<< endl<<'[';
	for (int i = 0; i < sigma2.size(); i++)
	{
		cout << sigma2[i] << ' ';
	}
	cout << ']';

	return sigma2;
}













































