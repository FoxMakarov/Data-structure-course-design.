#include"travel.h"

int main()
{
	initGraphy();

	while (true)
	{
		showMenu();
		int i;
		cin >> i;
		switch (i)
		{
		case 0:
			cout << "                          -----------------------------------------------------------" << endl;
			cout << "                                                    欢迎再次使用    " << endl;
			cout << "                          -----------------------------------------------------------" << endl;
			save();
			return 0;
			break;
		case 1:
			addCity();
			break;
		case 2:
		{
			cout << "请输入您想从交通图中删除的城市的名称：" << endl;
			string cname;
			cin >> cname;
			if(eraseCity(cname))cout<<"删除成功！"<<endl << endl;
			break;
		}
		case 3:
		{
			int nu;
			cout << "请输入想输入的火车航线的条数:" << endl;
			cin >> nu;
			cout << "请按照火车班次序号，火车停靠几次，每段起始城市和终点城市，每段起始时间和终点时间，每段价格的顺序输入：" << endl;
			cout << "例：" << endl;
			cout << "1254huoche 2" << endl;
			cout << "青岛 南京 12:50 14:50 280 " << endl;
			cout << "南京 扬州 15:20 16:50 130 " << endl;
			cout << ", 表示班次为\"1254huoche\"的火车停靠两次，12点50分从青岛出发，14点50分到达南京，价格为280元，15点20分从南京出发，16点50分到达扬州，价格为130元" << endl;
			cout << "-------------------------------------------" << endl;
			infoInputUser(nu, trainInfo);
			cout << "-------------------------------------------" << endl;
			int newAllNumber = trainInfo.size();
			for (int t = 1; t <= nu; t++)
				insertTrain(trainInfo[newAllNumber - t]);
			cout << "添加成功！" << endl << endl;
			break;
		}
		case 4:
		{
			cout << "请输入您想删除的火车车次编号:" << endl;
			string trainId;
			cin >> trainId;
			if(eraseVehcle(trainId, 1))cout<<"删除成功！"<<endl;
			break;
		}
		case 5:
		{
			int nu;
			cout << "请输入想输入的飞机航线的条数:" << endl;
			cin >> nu;
			cout << "请按照飞机班次序号，飞机停靠几次，每段起始城市和终点城市，每段起始时间和终点时间，每段价格的顺序输入：" << endl;
			cout << "例：1254shandong 1 青岛 南京 12:50 14:50 280 , 表示班次为\"1254shandong\"的飞机停靠一次，12点50分从青岛出发，14点50分到达南京，价格为280元" << endl;
			cout << "-------------------------------------------" << endl;
			infoInputUser(nu, flightInfo);
			cout << "-------------------------------------------" << endl;
			int newAllNumber = flightInfo.size();
			for (int t = 1; t <= nu; t++)
				insertFlight(flightInfo[newAllNumber - t]);
			cout << "添加成功！" << endl << endl;
			break;
		}
		case 6:
		{
			cout << "请输入您想删除的航班编号:" << endl;
			string flightId;
			cin >> flightId;
			if (eraseVehcle(flightId, 0))cout << "删除成功！" << endl << endl;
			break;
		}
		case 7:
		{
			string scity, ecity;
			Time beg(0,0,0);
			cout << "请输入您的出发城市：" << endl;
			cin >> scity;
			while (mp.find(scity) == mp.end())
			{
				cout << "您输入的城市不存在，请重新输入：" << endl;
				cin >> scity;
			}
			cout << "请输入您的终点城市：" << endl;
			cin >> ecity;
			while (mp.find(ecity) == mp.end() || scity == ecity)
			{
				if (mp.find(ecity) == mp.end()) {
					cout << "您输入的城市不存在，请重新输入：" << endl;
					cin >> ecity;
				}
				else
				{
					cout << "您的出发城市和终点城市一致，请重新输入:"<<endl;
					cin >> ecity;
				}
			}
			string btime;
			cout << "请输入您的出发时间,如11:00:" << endl;
			cin >> btime;
			sscanf(btime.c_str(), "%d:%d", &beg.t[1], &beg.t[2]);

			cout << "请输入您的乘坐工具：飞机(0) , 火车（1）" << endl;
			int k;
			cin >> k;
			while (!(k == 0 || k == 1))
			{
				cout << "请按照题目要求输入！" << endl;
				cin >> k;
			}

			cout << "请选择您的最优决策：最短时间(1) 、最小花费(2) 、最少中转(3)" << endl;
			int po;
			cin >> po;
			while (!(po == 1 || po == 2 || po == 3))
			{
				cout << "请按照题目要求输入！" << endl;
				cin >> po;
			}

			bool succeed;

			if (po == 1)succeed = djLeastTime(mp[scity], mp[ecity], beg, k);
			if (po == 2)succeed = djLeastMoney(mp[scity], mp[ecity], beg, k);
			if (po == 3)
			{
				if (k == 0)succeed = djMinTransferFlight(mp[scity], mp[ecity], beg);
				else succeed = djMinTransferTrain(mp[scity], mp[ecity], beg);
			}
			if (!succeed)cout << "抱歉，未能找到符合要求的路径。" << endl;
		}
		default:
			break;
		}
	}
	
}