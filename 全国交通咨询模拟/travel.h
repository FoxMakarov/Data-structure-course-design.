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
	int t[3]; //t[0]:天, t[1]:时, t[2]分
	
	Time(int day = 0, int hour = 0, int minute = 0) { t[0] = day, t[1] = hour, t[2] = minute; }
	bool operator<(Time x) {
		if (t[0] != x.t[0])return t[0] < x.t[0];
		if (t[1] != x.t[1])return t[1] < x.t[1];
		if (t[2] != x.t[2])return t[2] < x.t[2];
	}
	
	Time operator+(Time x) //重载"+"
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

	Time operator-(Time x) {  //重载"-"，保证大的减小的
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


Time getTimeDiffer(Time& t1, Time& t2)//t1是之后的时间,t1 - t2代表着度过了多长时间
{
	if (t1 < t2)return Time(1, 0, 0) + t1 - t2;  //例如第二天5:00 - 前一天12:00 = 24 + 5 - 12 = 17
	else return t1 - t2;
}

struct Vehicle
{
	string  identifier;  //班次编号 
	int number; //有几个路段
	vector<pair<string, string> >city;  //pair里面一个是起始城市，一个到达城市
	vector<pair<Time, Time> >time;      //pair里面一个是起始时间，一个是抵达时间
	vector<float>cost;                  //用vector的原因是每个路段有各自的信息
};

struct Arc {
	Vehicle* way; //代表哪一个交通线路
	int which;//代表是是那个交通线路的哪一段

	Arc()
	{
		way = new Vehicle();
	}
};


struct EdgeInfo
{
	vector<Vehicle*>TravelWays;//代表两个城市之间的实现通达的方式，代表是哪一个车次/航班
	vector<int>which;//与上面的一一对应，代表是车次/航班里的哪一个路段，
};

struct Edge
{
	int nextCity;                           //下一个城市的在邻接表中的位置
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
	int beginArc;  //代表从way的第beginArc个路段的起始城市
	int endArc;    //代表到way的第endArc个路段的终点城市
	float cost;

	VirtualArc()
	{
		way = new Vehicle();
	}

	VirtualArc(Vehicle* _way, int _begin, int _end, float _cost):way(_way),beginArc(_begin),endArc(_end),cost(_cost){}
};

struct Vcity  //在邻接表中的首节点
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
	VirtualArc virtualArc;  //代表到这个城市的虚拟边中的哪一个
	float cost;//到达当前城市的花费
	Time timeUsed;  //到达当前城市的花费
	int change;//代表中转次数
};

struct mapCity
{
	int number; //代表两座城市之间能通过几辆车到达
	vector<VirtualArc*> virtualArc;  //代表到这两个城市之间城市的所有能到的虚拟边

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
	Arc arc; //代表before到这个城市之间的班次弧线中的哪一个
	float cost;//到达当前城市的花费
	Time timeUsed;  //到达当前城市的花费
	int change;//代表中转次数

};

struct cmpTime //为了实现dj算法，而用来重写小根堆比较函数
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

vector<Vcity*>Graphy;  //代表城市的图
vector<Vehicle*>flightInfo;  //存储着航班信息的数组
vector<Vehicle*>trainInfo;  //存储着火车信息的数组
map<string, int>mp;

void infoInputFile(vector<Vehicle*>&inf, ifstream &infile)  //采用输入流向对应的数组里读入东西
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

void infoInputUser(int n,vector<Vehicle*>& inf) //由用户读入交通线路的消息
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

void insertFlight(Vehicle* flightVeh)  //插入航班
{
	for (int j = 0; j < flightVeh->number; j++)  //航班的路段
	{
		string scity = flightVeh->city[j].first, ecity = flightVeh->city[j].second;
		int s = mp[scity], e = mp[ecity];
		int judge = true;
		Edge* edge = Graphy[s]->flightRoute;
		while (edge != NULL && judge)
		{
			if (edge->nextCity == e)//代表这两个城市之间已经有边
			{
				judge = false;
				break;
			}
			edge = edge->next;
		}
		if (judge) //若是还没有边，就要新创边的信息
		{
			edge = new Edge();
			edge->nextCity = e;
		}//此时这两个城市之间还没有边
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
			if (edge->nextCity == e)//代表这两个城市之间已经有边
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
		}//此时这两个城市之间还没有边
		edge->info->TravelWays.push_back(trainVeh);
		edge->info->which.push_back(j);
		if (judge)
		{
			//将新创的边加入到图中进去
			edge->next = Graphy[s]->trainRoute;
			Graphy[s]->trainRoute = edge;
		}
	}
}

void addCity()
{
	cout << "请输入新增的城市名称：" << endl;
	string newCityName;
	cin >> newCityName;
	bool cityExisted;
	if (mp.find(newCityName) != mp.end())cityExisted = true;
	else cityExisted = false;

	while (cityExisted)
	{
		cout << "您输入的城市已存在，请确认是否输入正确。若是您想继续输入，请输入\"Y/y\",否则请输入\"N/n\"" << endl;
		char judgec;
		cin >> judgec;
		if (judgec != 'Y' && judgec != 'y')
		{
			cout << "已取消操作!" << endl;
			return;
		}
		cout << "请输入新增的城市名称：" << endl;
		cin >> newCityName;
		if (mp.find(newCityName) == mp.end())cityExisted = false;
	}

	Vcity* newCity = new Vcity(newCityName);
	Graphy.push_back(newCity);
	mp[newCityName] = Graphy.size() - 1;
	cout << "添加成功！" << endl;
}

void initGraphy()
{
	int i;
	cout << "                          ----------------------------------------------------------" << endl;
	cout << "                                           欢迎来到交通模拟查询系统" << endl<<endl;
	cout << "                                            请选择初始化交通图方式" << endl;
	cout << "                                                   (1)文件  " << endl;
	cout << "                                                   (2)键盘  " << endl;
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
		//为了减少每次找城市的时间，用map结构

		ifstream in1(flightfile, ios::in);
		infoInputFile(flightInfo, in1);
		in1.close();

		ifstream in2(trainfile, ios::in);
		infoInputFile(trainInfo, in2);
		in2.close();
	}
	else
	{
		cout << "请输入想输入的城市个数：" << endl;
		int nu;
		cin >> nu;
		cout << "请输入城市名称：" << endl;
		string cname;
		for (int i = 0; i < nu; i++)
		{
			cin >> cname;
			Vcity* newcity = new Vcity(cname);
			Graphy.push_back(newcity);
			mp[cname] = i;
		}

		cout << "请输入想输入的飞机航线的条数：" << endl;
		cin >> nu;
		cout << "请按照飞机班次序号，飞机停靠几次，每段起始城市和终点城市，每段起始时间和终点时间，每段价格的顺序输入：" << endl;
		cout << "例：1254shandong 1 青岛 南京 12:50 14:50 280 , 表示班次为\"1254shandong\"的飞机停靠一次，12点50分从青岛出发，14点50分到达南京，价格为280元" << endl;
		cout << "-------------------------------------------" << endl;
		infoInputUser(nu, flightInfo);

		cout << "-------------------------------------------" << endl;
		cout << "请输入想输入的火车航线的条数：" << endl;
		cin >> nu;
		cout << "请按照火车班次序号，火车停靠几次，每段起始城市和终点城市，每段起始时间和终点时间，每段价格的顺序输入：" << endl;
		cout << "例："<<endl;
		cout << "1254huoche 2" <<endl;
		cout << "青岛 南京 12:50 14:50 280 " << endl;
		cout << "南京 扬州 15:20 16:50 130 " << endl;
		cout<<", 表示班次为\"1254huoche\"的火车停靠两次，12点50分从青岛出发，14点50分到达南京，价格为280元，15点20分从南京出发，16点50分到达扬州，价格为130元" << endl;
		cout << "-------------------------------------------" << endl;
		infoInputUser(nu, trainInfo);
		cout << endl << endl;
	}

	//当初始的信息全部被存入航班或是火车数组中后，将里面的每一个分别执行插入操作
	for (int i = 0; i < flightInfo.size(); i++)
		insertFlight(flightInfo[i]);
	for (int i = 0; i < trainInfo.size(); i++)
		insertTrain(trainInfo[i]);
}

void showMenu()
{
	cout << "                          ----------------------------------------------------------" << endl;
	cout << "                                           欢迎来到交通模拟查询系统" << endl << endl;
	cout << "                                                请选择您的操作" << endl<<endl;
	cout << "                                                (1)添加城市  " << endl;
	cout << "                                                (2)删除城市  " << endl;
	cout << "                                                (3)添加火车车次  " << endl;
	cout << "                                                (4)删除火车车次  " << endl;
	cout << "                                                (5)添加航班班次  " << endl;
	cout << "                                                (6)删除航班班次  " << endl;
	cout << "                                                (7)查询路线  " << endl;
	cout << "                                                (0)退出系统  " << endl;
	cout << "                          -----------------------------------------------------------" << endl;
}

void eraseEdge(int beforeCnumber, int endCnumber, Vehicle*veh,int judge)//删除图中从beforeCnumber到endCnumber的veh班次部分,judge为1代表火车，为0代表飞机
{
	Edge* before = NULL;
	Edge* thisNode;
	if(judge == 1)thisNode = Graphy[beforeCnumber]->trainRoute;
	else thisNode = Graphy[beforeCnumber]->flightRoute;
	while (thisNode != NULL && thisNode->nextCity != endCnumber )//找到了从起始城市到删除城市之间的边
	{
		before = thisNode;
		thisNode = thisNode->next;
	}

	int whichHu = -1;//是边里面的哪道弧线
	for (int p = 0; p < thisNode->info->TravelWays.size(); p++)
	{
		if (thisNode->info->TravelWays[p]->identifier == veh->identifier)
		{
			whichHu = p; break;
		}
	}
	thisNode->info->TravelWays.erase(thisNode->info->TravelWays.begin() + whichHu); //从边里面存的交通信息中删除
	thisNode->info->which.erase(thisNode->info->which.begin() + whichHu);
	if (thisNode->info->TravelWays.size() == 0)//如果就一条弧还被删了，则把图里面表示的边删除
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
		cout << "此城市不存在！请检查是否输入错误" << endl;
		return false;
	}
	int cnumber = mp[cname];
	//有两种情况，一是线路中包含从这个城市出发的，二是最后一段是到达这个城市

	//先把从这个城市出发交通线路的那部分全部删除
	//这部分是删除城市图里面的东西
	Edge* node = Graphy[cnumber]->trainRoute;
	while (node != NULL)
	{
		int y = node->info->TravelWays.size();//为什么设立这个y的原因是，当我们从图中删除一条航线的部分时，这个节点本身的那个也必定会被删除，所以travelway大小会变
		Edge* nextnode = node->next;//因为当我们将这条边上信息全部删除的时候，这条边会被删除，因此也就无从谈起这之后的node->next
		while(y--!=0)//到某个城市有几道航线
		{
			Vehicle* veh = node->info->TravelWays[0];//在这条航线上，每一从必定会把这个第0个给删除
			int startWhich = node->info->which[0];//在航线上的第几个到了这个城市
			if (startWhich != 0)startWhich--;//还得把到这个城市的那条线路给删了,因此要前移一位
			for (int op = startWhich; op < veh->city.size(); op++)
			{
				string startCity = veh->city[op].first;
				string endCity = veh->city[op].second;
				int beforeCnumber = mp[startCity];
				int endCnumber = mp[endCity];
				eraseEdge(beforeCnumber, endCnumber, veh, 1);
			}


			//这部分是删除车次信息数组里的东西
			if (startWhich == 0)//整个班次被取消
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
				//把这个位置之后的路段信息全部删掉
				while (veh->number != startWhich)//  例如如果startWhich是1，那么最后剩下的就该只有一个，是位置0
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

	//现在处理最后一站到这个城市的
	for (int i = 0; i < trainInfo.size(); i++)
	{
		if (trainInfo[i]->city[trainInfo[i]->number - 1].second == cname)
		{
			eraseEdge(mp[trainInfo[i]->city[trainInfo[i]->number - 1].first], mp[cname], trainInfo[i], 1);
			if (trainInfo[i]->number == 1)//就这一站，直接删除
			{
				delete trainInfo[i];
				trainInfo.erase(trainInfo.begin() + i);
				i--;//把这个位置原本的给删了的话，那么本该在下一个的将会到这个位置上
			}
			else  //把最后一站的给删除
			{
				trainInfo[i]->city.pop_back();
				trainInfo[i]->cost.pop_back();
				trainInfo[i]->time.pop_back();
				trainInfo[i]->number--;
			}
		}
	}

	//现在是飞机
	//首先删除从这个城市出发的飞机
	node = Graphy[cnumber]->flightRoute;
	while (node != NULL)
	{
		int y = node->info->TravelWays.size();
		Edge* nextnode = node->next;
		while (y-- != 0)//把这几道航线都搞定
		{
			Vehicle* veh = node->info->TravelWays[0];//在这条航线上，每一从必定会把这个第0个给删除
			string startCity = veh->city[0].first;//飞机只停靠一次
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

	//现在处理飞到这个城市的航线
	for (int i = 0; i < flightInfo.size(); i++)
	{
		if (flightInfo[i]->city[0].second == cname)
		{
			eraseEdge(mp[flightInfo[i]->city[0].first], mp[cname], flightInfo[i], 0);
			delete flightInfo[i];
			flightInfo.erase(flightInfo.begin() + i);
			i--;//把这个位置原本的给删了的话，那么本该在下一个的将会到这个位置上
		}
	}

	mp.erase(cname);
	delete Graphy[cnumber];//把表示城市的点从图中删除
	Graphy.erase(Graphy.begin() + cnumber);
	for (int i = cnumber; i < Graphy.size(); i++)
		mp[Graphy[i]->name] = i;
	return true;
}

bool eraseVehcle(string id, int decide)
{
	if (decide == 1)//火车
	{
		for (int i = 0; i < trainInfo.size(); i++)
		{
			if (id == trainInfo[i]->identifier)//从火车信息表中找到这个车次
			{
				for (int j = 0; j < trainInfo[i]->number; j++)
				{
					int startCnumber = mp[trainInfo[i]->city[j].first];
					int endCnumber = mp[trainInfo[i]->city[j].second];
					eraseEdge(startCnumber, endCnumber, trainInfo[i], 1);
					//对每一个路段删除边
				}
				trainInfo.erase(trainInfo.begin() + i);
				//从信息数组中删除
				return true;
			}
		}
		cout << "未找到该编号的火车！" << endl;
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
		cout << "未找到该编号的航班！" << endl;
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
	if (path[now].arc.way->city[path[now].arc.which].first == "superBegin")return;//到了起始城市
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

bool djLeastTime(int sc, int ec, Time beginTime, int way)//用Dijkstra算法思想求得从起点sc城市到终点sc城市之间的距离,beginTime代表游客预想的出发时间，
{                                                        //way表示乘坐的交通工具是哪个，如果way是0，表示是飞机，way是1表示火车
	int citynumber = Graphy.size();
	vector<Path>path(citynumber);
	vector<bool>arrived(citynumber, false);
	for (int i = 0; i < citynumber; i++)
	{
		path[i].arc.way->city.push_back(pair<string,string>("superBegin",Graphy[i]->name));//初始化到达每个城市的前一个城市都是绝对取不到的超级开始
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
		if (thiscity == ec)break;//到了目标城市
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
				int arcNum = node->info->TravelWays.size(); //这两座城市之间有几道弧线
				int which = -1;//到另一座城市最短时间的弧线序号
				Time temptime = infTime;
				for (int u = 0; u < arcNum; u++)
				{
					Time tem = (getTimeDiffer(node->info->TravelWays[u]->time[node->info->which[u]].first, temp->arc.way->time[temp->arc.which].second)
						+ getTimeDiffer(node->info->TravelWays[u]->time[node->info->which[u]].second, node->info->TravelWays[u]->time[node->info->which[u]].first)
						);
					if ( tem < temptime)//越早到达越好
					{
						temptime = tem;
						which = u;
					}
				}
				if (which != -1) {
					
					Time used = temp->timeUsed + temptime;

					if (used < path[node->nextCity].timeUsed)//松弛
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

	if (path[ec].arc.way->city[path[ec].arc.which].first == "superBegin")//代表还没有到过
		return false;
	else //代表有路
	{
		Time result = path[ec].timeUsed;
		cout << "从城市" << Graphy[sc]->name << "的候车室开始到最终到达城市" << Graphy[ec]->name << "最快需要" << result.t[0] << "天" << result.t[1] << "时" << result.t[2] << "分" << endl;
		cout << "旅行路线是： 出发时间 - 到达时间 | 班次编号 | 出发城市 - 到达城市 | 票价" << endl;
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
		path[i].arc.way->city.push_back(pair<string, string>("superBegin", Graphy[i]->name));//初始化到达每个城市的前一个城市都是绝对取不到的超级开始
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
		if (thiscity == ec)break;//到了目标城市
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
				int arcNum = node->info->TravelWays.size(); //这两座城市之间有几道弧线
				int which = -1;//到另一座城市最少花费的弧线序号
				float money = infMoney;
				for (int u = 0; u < arcNum; u++)
				{
					if (node->info->TravelWays[u]->cost[node->info->which[u]] < money)//钱越少越好
					{
						money = node->info->TravelWays[u]->cost[node->info->which[u]];
						which = u;
					}
				}
				if (which != -1) {
					float newCost = temp->cost + money;
					if (newCost < path[node->nextCity].cost)//松弛
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
		
		if (path[ec].arc.way->city[path[ec].arc.which].first == "superBegin")//代表还没有到过
			return false;
		else //代表有路
		{
			cout << "从城市" << Graphy[sc]->name << "的候车室开始到最终到达城市" << Graphy[ec]->name << "最少需要" << path[ec].cost << "RMB" << endl;
			cout << "所需时间为" << path[ec].timeUsed.t[0] << "日" << path[ec].timeUsed.t[1] << "时" << path[ec].timeUsed.t[2] << "分" << endl;

			cout << "旅行路线是： 出发时间 - 到达时间 | 班次编号 | 出发城市 - 到达城市 | 票价" << endl;
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
			int s = mp[trainInfo[i]->city[p].first];  //从某一个起始城市
			float cost = 0;
			for (int q = p; q < trainInfo[i]->number; q++)
			{
				int t = mp[trainInfo[i]->city[q].second];
				cost += trainInfo[i]->cost[q];
				mpCity[s][t].number = 1;  //代表乘一辆车就能到
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
		path[i].virtualArc.way->city.push_back(pair<string, string>("superBegin", Graphy[i]->name));//初始化到达每个城市的前一个城市都是绝对取不到的超级开始
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
			if (!arrived[i] && path[i].change >= path[thiscity].change + mpCity[thiscity][i].number)//松弛操作
			{
				//现在开始选择一条耗时最短的弧
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
				if (path[i].change == path[thiscity].change + mpCity[thiscity][i].number)  //对于中转次数不变的情况，判断所耗费的时间，如果耗费的时间新的路径少，那么则将path[i]改变
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
		cout << "从城市" << Graphy[sc]->name << "最终到达城市" << Graphy[ec]->name << "最少需要中转" << path[ec].change - 2 << "次" << endl;
		cout << "需要" << result.t[0] << "天" << result.t[1] << "时" << result.t[2] << "分" << endl;
		cout << "需要花费" << path[ec].cost << "RMB" << endl;
		cout << "旅行路线是： 出发时间 - 到达时间 | 班次编号 | 出发城市 - 到达城市 | 票价" << endl;
		printVirtualArc(path, ec, sc);
		return true;
	}
}


bool djMinTransferFlight(int sc, int ec, Time beginTime)   //这个给飞机
{
	int citynumber = Graphy.size();
	vector<Path>path(citynumber);
	vector<bool>arrived(citynumber, false);
	for (int i = 0; i < citynumber; i++)
	{
		path[i].arc.way->city.push_back(pair<string, string>("superBegin", Graphy[i]->name));//初始化到达每个城市的前一个城市都是绝对取不到的超级开始
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
		if (thiscity == ec)break;//到了目标城市
		if (arrived[thiscity])continue;
		arrived[thiscity] = true;
		Edge* node;
		node = Graphy[thiscity]->flightRoute;

		while (node != NULL)
		{
			int arcNum = node->info->TravelWays.size(); //这两座城市之间有几道弧线
			int nextcity = node->nextCity;
			if (path[nextcity].change < (temp->change + 1))//这里其实就是已经在进行松弛判断了
			{
				node = node->next;
				continue;
			}

			int which = -1;//到另一座城市最短时间的弧线序号
			Time temptime = infTime;
			for (int u = 0; u < arcNum; u++)
			{
				// 车次开始的时间 - 到达这个城市的时间 + 车次结束的时间 - 车次开始的时间

				Time tem = (getTimeDiffer(node->info->TravelWays[u]->time[node->info->which[u]].first, temp->arc.way->time[temp->arc.which].second)
					+ getTimeDiffer(node->info->TravelWays[u]->time[node->info->which[u]].second, node->info->TravelWays[u]->time[node->info->which[u]].first)
					);
				if (tem < temptime)//越早到达越好
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


	if (path[ec].arc.way->city[path[ec].arc.which].first == "superBegin")//代表还没有到过
		return false;
	else //代表有路
	{
		Time result = path[ec].timeUsed;
		cout << "从城市" << Graphy[sc]->name << "最终到达城市" << Graphy[ec]->name << "最少需要中转" << path[ec].change - 1 << "次" << endl;
		cout << "需要" << result.t[0] << "天" << result.t[1] << "时" << result.t[2] << "分" << endl;
		cout <<"需要花费"<< path[ec].cost << "RMB" << endl;
		cout << "旅行路线是： 出发时间 - 到达时间 | 班次编号 | 出发城市 - 到达城市 | 票价" << endl;
		print(path, ec);
		return true;
	}
}   