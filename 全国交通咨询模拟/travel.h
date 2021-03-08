#define _CRT_SECURE_NO_WARNINGS
#include<vector>
#include<iostream>
#include<string>
#include<stdio.h>
#include<map>
#include<fstream>
#include<queue>
using namespace std;
#define cityfile "E:/cityfile.txt"
#define flightfile "E:/flightfile.txt"
#define trainfile "E:/trainfile.txt"
#define infTime Time(35,0,0) 
#define infMoney 1000000
#define infChange 1000000

struct Time
{
	int t[3]; //t[0]:��, t[1]:ʱ, t[2]��
	
	Time(int day = 0, int hour = 0, int minute = 0) { t[0] = day, t[1] = hour, t[2] = minute; }
	bool operator<(Time x) {
		if (t[0] != x.t[0])return t[0] < x.t[0];
		if (t[1] != x.t[1])return t[1] < x.t[1];
		if (t[2] != x.t[2])return t[2] < x.t[2];
	}
	
	Time operator+(Time x) //����"+"
	{
		int t1 = t[0] * 24 * 60 + t[1] * 60 + t[2];
		int t2 = x.t[0] * 24 * 60 + x.t[1] * 60 + x.t[2];
		int result = t1 + t2;
		Time ans;
		ans.t[0] = result / (24 * 60);
		result = result % (24 * 60);
		ans.t[1] = result / (60);
		result = result % 60;
		ans.t[2] = result;
		return ans;
	}

	Time operator-(Time x) {  //����"-"����֤��ļ�С��
		int t1 = t[0] * 24 * 60 + t[1] * 60 + t[2];
		int t2 = x.t[0] * 24 * 60 + x.t[1] * 60 + x.t[2];
		int result = t1 - t2;
		Time ans;
		ans.t[0] = result / (24 * 60);
		result =  result % (24 * 60);
		ans.t[1] = result / (60);
		result = result % 60;
		ans.t[2] = result;
		return ans;
	}
};


Time getTimeDiffer(Time& t1, Time& t2)//t1��֮���ʱ��,t1 - t2�����Ŷȹ��˶೤ʱ��
{
	if (t1 < t2)return Time(1, 0, 0) + t1 - t2;  //����ڶ���5:00 - ǰһ��12:00 = 24 + 5 - 12 = 17
	else return t1 - t2;
}

struct Vehicle
{
	string  identifier;  //��α�� 
	int number; //�м���·��
	vector<pair<string, string> >city;  //pair����һ������ʼ���У�һ���������
	vector<pair<Time, Time> >time;      //pair����һ������ʼʱ�䣬һ���ǵִ�ʱ��
	vector<float>cost;                  //��vector��ԭ����ÿ��·���и��Ե���Ϣ
};

struct Arc {
	Vehicle* way; //������һ����ͨ��·
	int which;//���������Ǹ���ͨ��·����һ��

	Arc()
	{
		way = new Vehicle();
	}
};


struct EdgeInfo
{
	vector<Vehicle*>TravelWays;//������������֮���ʵ��ͨ��ķ�ʽ����������һ������/����
	vector<int>which;//�������һһ��Ӧ�������ǳ���/���������һ��·�Σ�
};

struct Edge
{
	int nextCity;                           //��һ�����е����ڽӱ��е�λ��
	EdgeInfo* info;
	Edge* next;

	Edge()
	{
		info = new EdgeInfo();
	}
};


struct VirtualArc
{
	Vehicle* way;
	int beginArc;  //�����way�ĵ�beginArc��·�ε���ʼ����
	int endArc;    //����way�ĵ�endArc��·�ε��յ����
	float cost;

	VirtualArc()
	{
		way = new Vehicle();
	}

	VirtualArc(Vehicle* _way, int _begin, int _end, float _cost):way(_way),beginArc(_begin),endArc(_end),cost(_cost){}
};

struct Vcity  //���ڽӱ��е��׽ڵ�
{
	string name;
	Edge* flightRoute;
	Edge* trainRoute;

	Vcity(string _name, Edge* _flight = NULL, Edge* _train = NULL)
	{
		name = _name;
		flightRoute = _flight;
		trainRoute = _train;
	}
};


struct PathVirtual
{
	VirtualArc virtualArc;  //����������е�������е���һ��
	float cost;//���ﵱǰ���еĻ���
	Time timeUsed;  //���ﵱǰ���еĻ���
	int change;//������ת����
};

struct mapCity
{
	int number; //������������֮����ͨ������������
	vector<VirtualArc*> virtualArc;  //��������������֮����е������ܵ��������

	mapCity()
	{
		number = infChange;
	}
};
//mapCity mpCity[5][5]
//mpCity[1][2].number
//mpCity[1][2].virtualArc


struct Path
{
	Arc arc; //����before���������֮��İ�λ����е���һ��
	float cost;//���ﵱǰ���еĻ���
	Time timeUsed;  //���ﵱǰ���еĻ���
	int change;//������ת����

};

struct cmpTime //Ϊ��ʵ��dj�㷨����������дС���ѱȽϺ���
{
	bool operator()(Path* a1, Path* a2)
	{
		return !(a1->timeUsed < a2->timeUsed);
	}
};

struct cmpMoney 
{
	bool operator()(Path* a1, Path* a2)
	{
		return a1->cost > a2->cost;
	}
};

struct cmpChange
{
	bool operator()(Path* a1, Path* a2)
	{
		return a1->change > a2->change;
	}
};

struct cmpVirtualChange
{
	bool operator()(PathVirtual* a1, PathVirtual* a2)
	{
		return a1->change > a2->change;
	}
};

vector<Vcity*>Graphy;  //������е�ͼ
vector<Vehicle*>flightInfo;  //�洢�ź�����Ϣ������
vector<Vehicle*>trainInfo;  //�洢�Ż���Ϣ������
map<string, int>mp;

void infoInputFile(vector<Vehicle*>&inf, ifstream &infile)  //�������������Ӧ����������붫��
{
	string id;
	while (infile >> id)
	{
		Vehicle* newVehicle = new Vehicle();
		newVehicle->identifier = id;
		infile >> newVehicle->number;
		for (int i = 0; i < newVehicle->number; i++)
		{
			string city1, city2;
			infile >> city1 >> city2;
			newVehicle->city.push_back(pair<string, string>(city1, city2));
			int t[4];
			for (int i = 0; i < 4; i++)infile >> t[i];
			newVehicle->time.push_back(pair<Time, Time>(Time(0, t[0], t[1]), Time(0, t[2], t[3])));
			float cost;
			infile >> cost;
			newVehicle->cost.push_back(cost);
		}
		inf.push_back(newVehicle);
	}
}

void infoInputUser(int n,vector<Vehicle*>& inf) //���û����뽻ͨ��·����Ϣ
{
	while (n--!=0)
	{
		string id;
		cin >> id;
		Vehicle* newVehicle = new Vehicle();
		newVehicle->identifier = id;
		cin >> newVehicle->number;
		for (int i = 0; i < newVehicle->number; i++)
		{
			string city1, city2;
			cin >> city1 >> city2;
			newVehicle->city.push_back(pair<string, string>(city1, city2));
			int t[4];//11:00
			string ti;
			cin >> ti;
			sscanf(ti.c_str(), "%d:%d", &t[0], &t[1]);
			cin >> ti;
			sscanf(ti.c_str(), "%d:%d", &t[2], &t[3]);
			newVehicle->time.push_back(pair<Time, Time>(Time(0, t[0], t[1]), Time(0, t[2], t[3])));
			float cost;
			cin >> cost;
			newVehicle->cost.push_back(cost);
		}
		inf.push_back(newVehicle);
	}
}

void insertFlight(Vehicle* flightVeh)  //���뺽��
{
	for (int j = 0; j < flightVeh->number; j++)  //�����·��
	{
		string scity = flightVeh->city[j].first, ecity = flightVeh->city[j].second;
		int s = mp[scity], e = mp[ecity];
		int judge = true;
		Edge* edge = Graphy[s]->flightRoute;
		while (edge != NULL && judge)
		{
			if (edge->nextCity == e)//��������������֮���Ѿ��б�
			{
				judge = false;
				break;
			}
			edge = edge->next;
		}
		if (judge) //���ǻ�û�бߣ���Ҫ�´��ߵ���Ϣ
		{
			edge = new Edge();
			edge->nextCity = e;
		}//��ʱ����������֮�仹û�б�
		edge->info->TravelWays.push_back(flightVeh);
		edge->info->which.push_back(j);
		if (judge)
		{
			edge->next = Graphy[s]->flightRoute;
			Graphy[s]->flightRoute = edge;
		}
	}
}


void insertTrain(Vehicle* trainVeh)
{
	for (int j = 0; j < trainVeh->number; j++)
	{
		string scity = trainVeh->city[j].first, ecity = trainVeh->city[j].second;
		int s = mp[scity], e = mp[ecity];
		bool judge = true;
		Edge* edge = Graphy[s]->trainRoute;
		while (edge != NULL && judge)
		{
			if (edge->nextCity == e)//��������������֮���Ѿ��б�
			{
				judge = false;
				break;
			}
			edge = edge->next;
		}
		if (judge)
		{
			edge = new Edge();
			edge->nextCity = e;
		}//��ʱ����������֮�仹û�б�
		edge->info->TravelWays.push_back(trainVeh);
		edge->info->which.push_back(j);
		if (judge)
		{
			//���´��ı߼��뵽ͼ�н�ȥ
			edge->next = Graphy[s]->trainRoute;
			Graphy[s]->trainRoute = edge;
		}
	}
}

void addCity()
{
	cout << "�����������ĳ������ƣ�" << endl;
	string newCityName;
	cin >> newCityName;
	bool cityExisted;
	if (mp.find(newCityName) != mp.end())cityExisted = true;
	else cityExisted = false;

	while (cityExisted)
	{
		cout << "������ĳ����Ѵ��ڣ���ȷ���Ƿ�������ȷ����������������룬������\"Y/y\",����������\"N/n\"" << endl;
		char judgec;
		cin >> judgec;
		if (judgec != 'Y' && judgec != 'y')
		{
			cout << "��ȡ������!" << endl;
			return;
		}
		cout << "�����������ĳ������ƣ�" << endl;
		cin >> newCityName;
		if (mp.find(newCityName) == mp.end())cityExisted = false;
	}

	Vcity* newCity = new Vcity(newCityName);
	Graphy.push_back(newCity);
	mp[newCityName] = Graphy.size() - 1;
	cout << "��ӳɹ���" << endl;
}

void initGraphy()
{
	int i;
	cout << "                          ----------------------------------------------------------" << endl;
	cout << "                                           ��ӭ������ͨģ���ѯϵͳ" << endl<<endl;
	cout << "                                            ��ѡ���ʼ����ͨͼ��ʽ" << endl;
	cout << "                                                   (1)�ļ�  " << endl;
	cout << "                                                   (2)����  " << endl;
	cout << "                          -----------------------------------------------------------" << endl;
	cin >> i;
	if (i == 1)
	{
		ifstream in(cityfile,ios::in);
		string name;
		while (in >> name)
		{
			Vcity* newcity = new Vcity(name);
			Graphy.push_back(newcity);
		}
		in.close();
		for (int i = 0; i < Graphy.size(); i++)
			mp[Graphy[i]->name] = i;
		//Ϊ�˼���ÿ���ҳ��е�ʱ�䣬��map�ṹ

		ifstream in1(flightfile, ios::in);
		infoInputFile(flightInfo, in1);
		in1.close();

		ifstream in2(trainfile, ios::in);
		infoInputFile(trainInfo, in2);
		in2.close();
	}
	else
	{
		cout << "������������ĳ��и�����" << endl;
		int nu;
		cin >> nu;
		cout << "������������ƣ�" << endl;
		string cname;
		for (int i = 0; i < nu; i++)
		{
			cin >> cname;
			Vcity* newcity = new Vcity(cname);
			Graphy.push_back(newcity);
			mp[cname] = i;
		}

		cout << "������������ķɻ����ߵ�������" << endl;
		cin >> nu;
		cout << "�밴�շɻ������ţ��ɻ�ͣ�����Σ�ÿ����ʼ���к��յ���У�ÿ����ʼʱ����յ�ʱ�䣬ÿ�μ۸��˳�����룺" << endl;
		cout << "����1254shandong 1 �ൺ �Ͼ� 12:50 14:50 280 , ��ʾ���Ϊ\"1254shandong\"�ķɻ�ͣ��һ�Σ�12��50�ִ��ൺ������14��50�ֵ����Ͼ����۸�Ϊ280Ԫ" << endl;
		cout << "-------------------------------------------" << endl;
		infoInputUser(nu, flightInfo);

		cout << "-------------------------------------------" << endl;
		cout << "������������Ļ𳵺��ߵ�������" << endl;
		cin >> nu;
		cout << "�밴�ջ𳵰����ţ���ͣ�����Σ�ÿ����ʼ���к��յ���У�ÿ����ʼʱ����յ�ʱ�䣬ÿ�μ۸��˳�����룺" << endl;
		cout << "����"<<endl;
		cout << "1254huoche 2" <<endl;
		cout << "�ൺ �Ͼ� 12:50 14:50 280 " << endl;
		cout << "�Ͼ� ���� 15:20 16:50 130 " << endl;
		cout<<", ��ʾ���Ϊ\"1254huoche\"�Ļ�ͣ�����Σ�12��50�ִ��ൺ������14��50�ֵ����Ͼ����۸�Ϊ280Ԫ��15��20�ִ��Ͼ�������16��50�ֵ������ݣ��۸�Ϊ130Ԫ" << endl;
		cout << "-------------------------------------------" << endl;
		infoInputUser(nu, trainInfo);
		cout << endl << endl;
	}

	//����ʼ����Ϣȫ�������뺽����ǻ������к󣬽������ÿһ���ֱ�ִ�в������
	for (int i = 0; i < flightInfo.size(); i++)
		insertFlight(flightInfo[i]);
	for (int i = 0; i < trainInfo.size(); i++)
		insertTrain(trainInfo[i]);
}

void showMenu()
{
	cout << "                          ----------------------------------------------------------" << endl;
	cout << "                                           ��ӭ������ͨģ���ѯϵͳ" << endl << endl;
	cout << "                                                ��ѡ�����Ĳ���" << endl<<endl;
	cout << "                                                (1)��ӳ���  " << endl;
	cout << "                                                (2)ɾ������  " << endl;
	cout << "                                                (3)��ӻ𳵳���  " << endl;
	cout << "                                                (4)ɾ���𳵳���  " << endl;
	cout << "                                                (5)��Ӻ�����  " << endl;
	cout << "                                                (6)ɾ��������  " << endl;
	cout << "                                                (7)��ѯ·��  " << endl;
	cout << "                                                (0)�˳�ϵͳ  " << endl;
	cout << "                          -----------------------------------------------------------" << endl;
}

void eraseEdge(int beforeCnumber, int endCnumber, Vehicle*veh,int judge)//ɾ��ͼ�д�beforeCnumber��endCnumber��veh��β���,judgeΪ1����𳵣�Ϊ0����ɻ�
{
	Edge* before = NULL;
	Edge* thisNode;
	if(judge == 1)thisNode = Graphy[beforeCnumber]->trainRoute;
	else thisNode = Graphy[beforeCnumber]->flightRoute;
	while (thisNode != NULL && thisNode->nextCity != endCnumber )//�ҵ��˴���ʼ���е�ɾ������֮��ı�
	{
		before = thisNode;
		thisNode = thisNode->next;
	}

	int whichHu = -1;//�Ǳ�������ĵ�����
	for (int p = 0; p < thisNode->info->TravelWays.size(); p++)
	{
		if (thisNode->info->TravelWays[p]->identifier == veh->identifier)
		{
			whichHu = p; break;
		}
	}
	thisNode->info->TravelWays.erase(thisNode->info->TravelWays.begin() + whichHu); //�ӱ������Ľ�ͨ��Ϣ��ɾ��
	thisNode->info->which.erase(thisNode->info->which.begin() + whichHu);
	if (thisNode->info->TravelWays.size() == 0)//�����һ��������ɾ�ˣ����ͼ�����ʾ�ı�ɾ��
	{
		Edge* temp = thisNode;
		if (before == NULL)
		{
			if (judge == 1)Graphy[beforeCnumber]->trainRoute = thisNode->next;
			else Graphy[beforeCnumber]->flightRoute = thisNode->next;
		}
		else
		{
			before->next = thisNode->next;
		}
		delete temp;
	}
}

bool eraseCity(string cname)
{
	if (mp.find(cname) == mp.end())
	{
		cout << "�˳��в����ڣ������Ƿ��������" << endl;
		return false;
	}
	int cnumber = mp[cname];
	//�����������һ����·�а�����������г����ģ��������һ���ǵ����������

	//�ȰѴ�������г�����ͨ��·���ǲ���ȫ��ɾ��
	//�ⲿ����ɾ������ͼ����Ķ���
	Edge* node = Graphy[cnumber]->trainRoute;
	while (node != NULL)
	{
		int y = node->info->TravelWays.size();//Ϊʲô�������y��ԭ���ǣ������Ǵ�ͼ��ɾ��һ�����ߵĲ���ʱ������ڵ㱾����Ǹ�Ҳ�ض��ᱻɾ��������travelway��С���
		Edge* nextnode = node->next;//��Ϊ�����ǽ�����������Ϣȫ��ɾ����ʱ�������߻ᱻɾ�������Ҳ���޴�̸����֮���node->next
		while(y--!=0)//��ĳ�������м�������
		{
			Vehicle* veh = node->info->TravelWays[0];//�����������ϣ�ÿһ�ӱض���������0����ɾ��
			int startWhich = node->info->which[0];//�ں����ϵĵڼ��������������
			if (startWhich != 0)startWhich--;//���ðѵ�������е�������·��ɾ��,���Ҫǰ��һλ
			for (int op = startWhich; op < veh->city.size(); op++)
			{
				string startCity = veh->city[op].first;
				string endCity = veh->city[op].second;
				int beforeCnumber = mp[startCity];
				int endCnumber = mp[endCity];
				eraseEdge(beforeCnumber, endCnumber, veh, 1);
			}


			//�ⲿ����ɾ��������Ϣ������Ķ���
			if (startWhich == 0)//������α�ȡ��
			{
				for (int u = 0; u < trainInfo.size(); u++)
				{
					if (trainInfo[u] == veh)
					{
						trainInfo.erase(trainInfo.begin() + u);
						delete veh;
						break;
					}
				}
			}
			else
			{
				//�����λ��֮���·����Ϣȫ��ɾ��
				while (veh->number != startWhich)//  �������startWhich��1����ô���ʣ�µľ͸�ֻ��һ������λ��0
				{
					veh->city.pop_back();
					veh->cost.pop_back();
					veh->time.pop_back();
					veh->number--;
				}
			}
		}
		node = nextnode;
	}

	//���ڴ������һվ��������е�
	for (int i = 0; i < trainInfo.size(); i++)
	{
		if (trainInfo[i]->city[trainInfo[i]->number - 1].second == cname)
		{
			eraseEdge(mp[trainInfo[i]->city[trainInfo[i]->number - 1].first], mp[cname], trainInfo[i], 1);
			if (trainInfo[i]->number == 1)//����һվ��ֱ��ɾ��
			{
				delete trainInfo[i];
				trainInfo.erase(trainInfo.begin() + i);
				i--;//�����λ��ԭ���ĸ�ɾ�˵Ļ�����ô��������һ���Ľ��ᵽ���λ����
			}
			else  //�����һվ�ĸ�ɾ��
			{
				trainInfo[i]->city.pop_back();
				trainInfo[i]->cost.pop_back();
				trainInfo[i]->time.pop_back();
				trainInfo[i]->number--;
			}
		}
	}

	//�����Ƿɻ�
	//����ɾ����������г����ķɻ�
	node = Graphy[cnumber]->flightRoute;
	while (node != NULL)
	{
		int y = node->info->TravelWays.size();
		Edge* nextnode = node->next;
		while (y-- != 0)//���⼸�����߶��㶨
		{
			Vehicle* veh = node->info->TravelWays[0];//�����������ϣ�ÿһ�ӱض���������0����ɾ��
			string startCity = veh->city[0].first;//�ɻ�ֻͣ��һ��
			string endCity = veh->city[0].second;
			int beforeCnumber = mp[startCity];
			int endCnumber = mp[endCity];
			eraseEdge(beforeCnumber, endCnumber, veh, 0);

			for (int u = 0; u < flightInfo.size(); u++)
			{
				if (flightInfo[u] == veh)
				{
					flightInfo.erase(flightInfo.begin() + u);
					delete veh;
					break;
				}
			}
		}
		node = nextnode;
	}

	//���ڴ���ɵ�������еĺ���
	for (int i = 0; i < flightInfo.size(); i++)
	{
		if (flightInfo[i]->city[0].second == cname)
		{
			eraseEdge(mp[flightInfo[i]->city[0].first], mp[cname], flightInfo[i], 0);
			delete flightInfo[i];
			flightInfo.erase(flightInfo.begin() + i);
			i--;//�����λ��ԭ���ĸ�ɾ�˵Ļ�����ô��������һ���Ľ��ᵽ���λ����
		}
	}

	mp.erase(cname);
	delete Graphy[cnumber];//�ѱ�ʾ���еĵ��ͼ��ɾ��
	Graphy.erase(Graphy.begin() + cnumber);
	for (int i = cnumber; i < Graphy.size(); i++)
		mp[Graphy[i]->name] = i;
	return true;
}

bool eraseVehcle(string id, int decide)
{
	if (decide == 1)//��
	{
		for (int i = 0; i < trainInfo.size(); i++)
		{
			if (id == trainInfo[i]->identifier)//�ӻ���Ϣ�����ҵ��������
			{
				for (int j = 0; j < trainInfo[i]->number; j++)
				{
					int startCnumber = mp[trainInfo[i]->city[j].first];
					int endCnumber = mp[trainInfo[i]->city[j].second];
					eraseEdge(startCnumber, endCnumber, trainInfo[i], 1);
					//��ÿһ��·��ɾ����
				}
				trainInfo.erase(trainInfo.begin() + i);
				//����Ϣ������ɾ��
				return true;
			}
		}
		cout << "δ�ҵ��ñ�ŵĻ𳵣�" << endl;
		return false;
	}
	else
	{
		for (int i = 0; i < flightInfo.size(); i++)
		{
			if (id == flightInfo[i]->identifier)
			{
				for (int j = 0; j < flightInfo[i]->number; j++)
				{
					int startCnumber = mp[flightInfo[i]->city[j].first];
					int endCnumber = mp[flightInfo[i]->city[j].second];
					eraseEdge(startCnumber, endCnumber, flightInfo[i], 0);
				}
				flightInfo.erase(flightInfo.begin() + i);
				return true;
			}
		}
		cout << "δ�ҵ��ñ�ŵĺ��࣡" << endl;
		return false;
	}
}

void save()
{
	freopen(cityfile, "w", stdout);
	int length = Graphy.size();
	for (int i = 0; i < length; i++)
		cout << Graphy[i]->name << endl;
	fclose(stdout);

	freopen(flightfile, "w", stdout);
	length = flightInfo.size();
	for (int i = 0; i < length; i++)
	{
		cout << flightInfo[i]->identifier << " " << flightInfo[i]->number << endl;
		for (int j = 0; j < flightInfo[i]->number; j++)
		{
			cout << flightInfo[i]->city[j].first << " " << flightInfo[i]->city[j].second << " ";
			cout << flightInfo[i]->time[j].first.t[1] << " " << flightInfo[i]->time[j].first.t[2] << " ";
			cout << flightInfo[i]->time[j].second.t[1] << " " << flightInfo[i]->time[j].second.t[2] << " ";
			cout << flightInfo[i]->cost[j];
		}
		cout << endl;
	}
	fclose(stdout);

	freopen(trainfile, "w", stdout);
	length = trainInfo.size();
	for (int i = 0; i < length; i++)
	{
		cout << trainInfo[i]->identifier << " " << trainInfo[i]->number << endl;
		for (int j = 0; j < trainInfo[i]->number; j++)
		{
			cout << trainInfo[i]->city[j].first << " " << trainInfo[i]->city[j].second << " ";
			cout << trainInfo[i]->time[j].first.t[1] << " " << trainInfo[i]->time[j].first.t[2] << " ";
			cout << trainInfo[i]->time[j].second.t[1] << " " << trainInfo[i]->time[j].second.t[2] << " ";
			cout << trainInfo[i]->cost[j];
			cout << endl;
		}
		cout << endl;
	}
	fclose(stdout);

}

void print(vector<Path>& path, int now)
{
	if (path[now].arc.way->city[path[now].arc.which].first == "superBegin")return;//������ʼ����
	print(path, mp[path[now].arc.way->city[path[now].arc.which].first]);
	
	string sh = "", sm = "", eh = "", em = "";
	if (path[now].arc.way->time[path[now].arc.which].first.t[1] < 10)sh = "0" + to_string(path[now].arc.way->time[path[now].arc.which].first.t[1]);
	else sh = to_string(path[now].arc.way->time[path[now].arc.which].first.t[1]);

	if(path[now].arc.way->time[path[now].arc.which].first.t[2]<10)sm = "0" + to_string(path[now].arc.way->time[path[now].arc.which].first.t[2]);
	else sm = to_string(path[now].arc.way->time[path[now].arc.which].first.t[2]);

	if(path[now].arc.way->time[path[now].arc.which].second.t[1]<10)eh = "0" + to_string(path[now].arc.way->time[path[now].arc.which].second.t[1]);
	else eh = to_string(path[now].arc.way->time[path[now].arc.which].second.t[1]);

	if (path[now].arc.way->time[path[now].arc.which].second.t[2] < 10)em = "0" + to_string(path[now].arc.way->time[path[now].arc.which].second.t[2]);
	else em = to_string(path[now].arc.way->time[path[now].arc.which].second.t[2]);
	

	cout << sh << ":"<< sm  << " - " << eh << ":"<< em 
		<< " | " << path[now].arc.way->identifier << " | " << path[now].arc.way->city[path[now].arc.which].first << " - "
		<< path[now].arc.way->city[path[now].arc.which].second << " | " << path[now].arc.way->cost[path[now].arc.which] <<"RMB"<< endl;
}

bool djLeastTime(int sc, int ec, Time beginTime, int way)//��Dijkstra�㷨˼����ô����sc���е��յ�sc����֮��ľ���,beginTime�����ο�Ԥ��ĳ���ʱ�䣬
{                                                        //way��ʾ�����Ľ�ͨ�������ĸ������way��0����ʾ�Ƿɻ���way��1��ʾ��
	int citynumber = Graphy.size();
	vector<Path>path(citynumber);
	vector<bool>arrived(citynumber, false);
	for (int i = 0; i < citynumber; i++)
	{
		path[i].arc.way->city.push_back(pair<string,string>("superBegin",Graphy[i]->name));//��ʼ������ÿ�����е�ǰһ�����ж��Ǿ���ȡ�����ĳ�����ʼ
		path[i].arc.which = 0;
		path[i].timeUsed = infTime;
	}

	path[sc].arc.way->time.push_back(pair<Time,Time>(infTime,beginTime));
	path[sc].timeUsed = Time(0, 0, 0);
	priority_queue<Path*, vector<Path*>, cmpTime>q;
	q.push(&path[sc]);
	while (!q.empty())
	{
		Path* temp = q.top();
		q.pop();
		int thiscity = mp[temp->arc.way->city[temp->arc.which].second];
		if (thiscity == ec)break;//����Ŀ�����
		if (arrived[thiscity])continue;
		arrived[thiscity] = true;
		Edge* node;
		if (way == 1)node = Graphy[thiscity]->trainRoute;
		else node = Graphy[thiscity]->flightRoute;

		while (node != NULL)
		{
			if (arrived[node->nextCity]) 
			{
				node = node->next;
			}
			else
			{
				int arcNum = node->info->TravelWays.size(); //����������֮���м�������
				int which = -1;//����һ���������ʱ��Ļ������
				Time temptime = infTime;
				for (int u = 0; u < arcNum; u++)
				{
					Time tem = (getTimeDiffer(node->info->TravelWays[u]->time[node->info->which[u]].first, temp->arc.way->time[temp->arc.which].second)
						+ getTimeDiffer(node->info->TravelWays[u]->time[node->info->which[u]].second, node->info->TravelWays[u]->time[node->info->which[u]].first)
						);
					if ( tem < temptime)//Խ�絽��Խ��
					{
						temptime = tem;
						which = u;
					}
				}
				if (which != -1) {
					
					Time used = temp->timeUsed + temptime;

					if (used < path[node->nextCity].timeUsed)//�ɳ�
					{
						path[node->nextCity].arc.way = node->info->TravelWays[which];
						path[node->nextCity].arc.which = node->info->which[which];
						path[node->nextCity].timeUsed = used;
						Path* t = new Path(path[node->nextCity]);
						q.push(t);
					}
				}
				node = node->next;
			}
		}
	}

	if (path[ec].arc.way->city[path[ec].arc.which].first == "superBegin")//����û�е���
		return false;
	else //������·
	{
		Time result = path[ec].timeUsed;
		cout << "�ӳ���" << Graphy[sc]->name << "�ĺ��ҿ�ʼ�����յ������" << Graphy[ec]->name << "�����Ҫ" << result.t[0] << "��" << result.t[1] << "ʱ" << result.t[2] << "��" << endl;
		cout << "����·���ǣ� ����ʱ�� - ����ʱ�� | ��α�� | �������� - ������� | Ʊ��" << endl;
		print(path, ec);
		return true;
	}
}


bool djLeastMoney(int sc, int ec, Time beginTime, int way)
{
	int citynumber = Graphy.size();
	vector<Path>path(citynumber);
	vector<bool>arrived(citynumber, false);
	for (int i = 0; i < citynumber; i++)
	{
		path[i].arc.way->city.push_back(pair<string, string>("superBegin", Graphy[i]->name));//��ʼ������ÿ�����е�ǰһ�����ж��Ǿ���ȡ�����ĳ�����ʼ
		path[i].arc.which = 0;
		path[i].timeUsed.t[0] = 0;
		path[i].timeUsed.t[1] = 0;
		path[i].timeUsed.t[2] = 0;
		path[i].cost = infMoney;
	}

	path[sc].cost = 0;
	path[sc].arc.way->time.push_back(pair<Time, Time>(infTime, beginTime));
	priority_queue<Path*, vector<Path*>, cmpMoney>q;
	q.push(&path[sc]);


	
	while (!q.empty())
	{
		Path* temp = q.top();
		q.pop();
		int thiscity = mp[temp->arc.way->city[temp->arc.which].second];
		if (thiscity == ec)break;//����Ŀ�����
		if (arrived[thiscity])continue;
		arrived[thiscity] = true;
		Edge* node;
		if (way == 1)node = Graphy[thiscity]->trainRoute;
		else node = Graphy[thiscity]->flightRoute;

		while (node != NULL)
		{
			if (arrived[node->nextCity])
			{
				node = node->next;
			}
			else
			{
				int arcNum = node->info->TravelWays.size(); //����������֮���м�������
				int which = -1;//����һ���������ٻ��ѵĻ������
				float money = infMoney;
				for (int u = 0; u < arcNum; u++)
				{
					if (node->info->TravelWays[u]->cost[node->info->which[u]] < money)//ǮԽ��Խ��
					{
						money = node->info->TravelWays[u]->cost[node->info->which[u]];
						which = u;
					}
				}
				if (which != -1) {
					float newCost = temp->cost + money;
					if (newCost < path[node->nextCity].cost)//�ɳ�
					{
						path[node->nextCity].arc.way = node->info->TravelWays[which];
						path[node->nextCity].arc.which = node->info->which[which];
						path[node->nextCity].cost = newCost;
						path[node->nextCity].timeUsed = temp->timeUsed
							+ getTimeDiffer(path[node->nextCity].arc.way->time[path[node->nextCity].arc.which].first, temp->arc.way->time[temp->arc.which].second)
							+ getTimeDiffer(path[node->nextCity].arc.way->time[path[node->nextCity].arc.which].second, path[node->nextCity].arc.way->time[path[node->nextCity].arc.which].first);
						Path* t = new Path(path[node->nextCity]);
						q.push(t);
					}
				}
				node = node->next;
			}
		}
	}
		
		if (path[ec].arc.way->city[path[ec].arc.which].first == "superBegin")//����û�е���
			return false;
		else //������·
		{
			cout << "�ӳ���" << Graphy[sc]->name << "�ĺ��ҿ�ʼ�����յ������" << Graphy[ec]->name << "������Ҫ" << path[ec].cost << "RMB" << endl;
			cout << "����ʱ��Ϊ" << path[ec].timeUsed.t[0] << "��" << path[ec].timeUsed.t[1] << "ʱ" << path[ec].timeUsed.t[2] << "��" << endl;

			cout << "����·���ǣ� ����ʱ�� - ����ʱ�� | ��α�� | �������� - ������� | Ʊ��" << endl;
			print(path, ec);
		}
		return true;
}

string to_Time(int n)
{
	string te = to_string(n);
	if (te.size() < 2)te = '0' + te;
	return te;
}

void initial(vector<vector<mapCity> >&mpCity)
{
	for (int i = 0; i < trainInfo.size(); i++)
	{
		for (int p = 0; p < trainInfo[i]->number; p++)
		{
			int s = mp[trainInfo[i]->city[p].first];  //��ĳһ����ʼ����
			float cost = 0;
			for (int q = p; q < trainInfo[i]->number; q++)
			{
				int t = mp[trainInfo[i]->city[q].second];
				cost += trainInfo[i]->cost[q];
				mpCity[s][t].number = 1;  //�����һ�������ܵ�
				mpCity[s][t].virtualArc.push_back(new VirtualArc(trainInfo[i], p, q, cost));
			}
		}
	}
}


void printVirtualArc(vector<PathVirtual>& path, int now, int sc)
{
	if (now == sc)return;
	printVirtualArc(path, mp[path[now].virtualArc.way->city[path[now].virtualArc.beginArc].first], sc);

	for (int i = path[now].virtualArc.beginArc; i <= path[now].virtualArc.endArc; i++)
	{
		string sh = to_Time(path[now].virtualArc.way->time[i].first.t[1]);
		string sm = to_Time(path[now].virtualArc.way->time[i].first.t[0]);
		string eh = to_Time(path[now].virtualArc.way->time[i].second.t[1]);
		string em = to_Time(path[now].virtualArc.way->time[i].second.t[0]);

		cout << sh << ":" << sm << " - " << eh << ":" << em
			<< " | " << path[now].virtualArc.way->identifier << " | " << path[now].virtualArc.way->city[i].first << " - "
			<< path[now].virtualArc.way->city[i].second << " | " << path[now].virtualArc.way->cost[i] << "RMB" << endl;
	}
}

bool djMinTransferTrain(int sc, int ec, Time beginTime)
{
	int citynumber = Graphy.size();
	vector<vector<mapCity> >mpCity(citynumber, vector<mapCity>(citynumber));
	for (int i = 0; i < citynumber; i++)
		mpCity[i][i].number = 0;
	initial(mpCity);

	vector<PathVirtual>path(citynumber);
	vector<bool>arrived(citynumber, false);

	for (int i = 0; i < citynumber; i++)
	{
		path[i].change = infChange;
		path[i].timeUsed = Time(0, 0, 0);
		path[i].virtualArc.way->city.push_back(pair<string, string>("superBegin", Graphy[i]->name));//��ʼ������ÿ�����е�ǰһ�����ж��Ǿ���ȡ�����ĳ�����ʼ
		path[i].virtualArc.beginArc = 0;
		path[i].virtualArc.endArc = 0;
	}

	path[sc].cost = 0;
	path[sc].virtualArc.way->time.push_back(pair<Time, Time>(infTime, beginTime));
	path[sc].change = 1;
	priority_queue<PathVirtual*, vector<PathVirtual*>, cmpVirtualChange>q;
	q.push(&path[sc]);

	while (q.size() != 0)
	{
		PathVirtual* temp = q.top();
		q.pop();
		int thiscity = mp[temp->virtualArc.way->city[temp->virtualArc.endArc].second];
		if (arrived[thiscity])continue;
		arrived[thiscity] = true;

		Time arriveThisCityTime = temp->virtualArc.way->time[temp->virtualArc.endArc].second;
		
		for (int i = 0; i < citynumber; i++)
		{
			if (!arrived[i] && path[i].change >= path[thiscity].change + mpCity[thiscity][i].number)//�ɳڲ���
			{
				//���ڿ�ʼѡ��һ����ʱ��̵Ļ�
				int arcNum = mpCity[thiscity][i].virtualArc.size();
				int whichHu = -1;
				Time tempTime = infTime;

				for (int p = 0; p < arcNum; p++)
				{
					Time tem = getTimeDiffer(mpCity[thiscity][i].virtualArc[p]->way->time[mpCity[thiscity][i].virtualArc[p]->beginArc].first, arriveThisCityTime) +
						getTimeDiffer(mpCity[thiscity][i].virtualArc[p]->way->time[mpCity[thiscity][i].virtualArc[p]->endArc].second, mpCity[thiscity][i].virtualArc[p]->way->time[mpCity[thiscity][i].virtualArc[p]->beginArc].first);
				
					if (tem < tempTime)
					{
						whichHu = p;
						tempTime = tem;
					}
				}

				Time used = path[thiscity].timeUsed + tempTime;
				if (path[i].change == path[thiscity].change + mpCity[thiscity][i].number)  //������ת���������������ж����ķѵ�ʱ�䣬����ķѵ�ʱ���µ�·���٣���ô��path[i]�ı�
				{
					if (!(used < path[i].timeUsed))continue;
				}
				path[i].change = path[thiscity].change + mpCity[thiscity][i].number;
				path[i].timeUsed = used;
				path[i].virtualArc.way = mpCity[thiscity][i].virtualArc[whichHu]->way;
				path[i].virtualArc.beginArc = mpCity[thiscity][i].virtualArc[whichHu]->beginArc;
				path[i].virtualArc.endArc = mpCity[thiscity][i].virtualArc[whichHu]->endArc;
				path[i].virtualArc.cost = mpCity[thiscity][i].virtualArc[whichHu]->cost;
				path[i].cost = path[thiscity].cost + mpCity[thiscity][i].virtualArc[whichHu]->cost;

				PathVirtual* t = new PathVirtual(path[i]);
				q.push(t);
			}
		}
	}

	if (path[ec].change == infChange)return false;
	else
	{
		Time result = path[ec].timeUsed;
		cout << "�ӳ���" << Graphy[sc]->name << "���յ������" << Graphy[ec]->name << "������Ҫ��ת" << path[ec].change - 2 << "��" << endl;
		cout << "��Ҫ" << result.t[0] << "��" << result.t[1] << "ʱ" << result.t[2] << "��" << endl;
		cout << "��Ҫ����" << path[ec].cost << "RMB" << endl;
		cout << "����·���ǣ� ����ʱ�� - ����ʱ�� | ��α�� | �������� - ������� | Ʊ��" << endl;
		printVirtualArc(path, ec, sc);
		return true;
	}
}


bool djMinTransferFlight(int sc, int ec, Time beginTime)   //������ɻ�
{
	int citynumber = Graphy.size();
	vector<Path>path(citynumber);
	vector<bool>arrived(citynumber, false);
	for (int i = 0; i < citynumber; i++)
	{
		path[i].arc.way->city.push_back(pair<string, string>("superBegin", Graphy[i]->name));//��ʼ������ÿ�����е�ǰһ�����ж��Ǿ���ȡ�����ĳ�����ʼ
		path[i].arc.which = 0;
		path[i].timeUsed.t[0] = 0;
		path[i].timeUsed.t[1] = 0;
		path[i].timeUsed.t[2] = 0;
		path[i].change = infChange;
	}

	path[sc].cost = 0;
	path[sc].arc.way->time.push_back(pair<Time, Time>(infTime, beginTime));
	path[sc].change = 0;
	priority_queue<Path*, vector<Path*>, cmpChange>q;
	q.push(&path[sc]);

	while (!q.empty())
	{
		Path* temp = q.top();
		q.pop();
		int thiscity = mp[temp->arc.way->city[temp->arc.which].second];
		if (thiscity == ec)break;//����Ŀ�����
		if (arrived[thiscity])continue;
		arrived[thiscity] = true;
		Edge* node;
		node = Graphy[thiscity]->flightRoute;

		while (node != NULL)
		{
			int arcNum = node->info->TravelWays.size(); //����������֮���м�������
			int nextcity = node->nextCity;
			if (path[nextcity].change < (temp->change + 1))//������ʵ�����Ѿ��ڽ����ɳ��ж���
			{
				node = node->next;
				continue;
			}

			int which = -1;//����һ���������ʱ��Ļ������
			Time temptime = infTime;
			for (int u = 0; u < arcNum; u++)
			{
				// ���ο�ʼ��ʱ�� - ����������е�ʱ�� + ���ν�����ʱ�� - ���ο�ʼ��ʱ��

				Time tem = (getTimeDiffer(node->info->TravelWays[u]->time[node->info->which[u]].first, temp->arc.way->time[temp->arc.which].second)
					+ getTimeDiffer(node->info->TravelWays[u]->time[node->info->which[u]].second, node->info->TravelWays[u]->time[node->info->which[u]].first)
					);
				if (tem < temptime)//Խ�絽��Խ��
				{
					temptime = tem;
					which = u;
				}
			}
			if (which != -1) {

				Time used = temp->timeUsed + temptime;

				path[node->nextCity].arc.way = node->info->TravelWays[which];
				path[node->nextCity].arc.which = node->info->which[which];
				path[node->nextCity].timeUsed = used;
				path[node->nextCity].change = temp->change + 1;
				path[node->nextCity].cost = temp->cost + node->info->TravelWays[which]->cost[node->info->which[which]];
				Path* t = new Path(path[node->nextCity]);
				q.push(t);

			}
			node = node->next;
		}
	}


	if (path[ec].arc.way->city[path[ec].arc.which].first == "superBegin")//����û�е���
		return false;
	else //������·
	{
		Time result = path[ec].timeUsed;
		cout << "�ӳ���" << Graphy[sc]->name << "���յ������" << Graphy[ec]->name << "������Ҫ��ת" << path[ec].change - 1 << "��" << endl;
		cout << "��Ҫ" << result.t[0] << "��" << result.t[1] << "ʱ" << result.t[2] << "��" << endl;
		cout <<"��Ҫ����"<< path[ec].cost << "RMB" << endl;
		cout << "����·���ǣ� ����ʱ�� - ����ʱ�� | ��α�� | �������� - ������� | Ʊ��" << endl;
		print(path, ec);
		return true;
	}
}   