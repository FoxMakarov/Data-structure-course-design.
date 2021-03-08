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
			cout << "                                                    ��ӭ�ٴ�ʹ��    " << endl;
			cout << "                          -----------------------------------------------------------" << endl;
			save();
			return 0;
			break;
		case 1:
			addCity();
			break;
		case 2:
		{
			cout << "����������ӽ�ͨͼ��ɾ���ĳ��е����ƣ�" << endl;
			string cname;
			cin >> cname;
			if(eraseCity(cname))cout<<"ɾ���ɹ���"<<endl << endl;
			break;
		}
		case 3:
		{
			int nu;
			cout << "������������Ļ𳵺��ߵ�����:" << endl;
			cin >> nu;
			cout << "�밴�ջ𳵰����ţ���ͣ�����Σ�ÿ����ʼ���к��յ���У�ÿ����ʼʱ����յ�ʱ�䣬ÿ�μ۸��˳�����룺" << endl;
			cout << "����" << endl;
			cout << "1254huoche 2" << endl;
			cout << "�ൺ �Ͼ� 12:50 14:50 280 " << endl;
			cout << "�Ͼ� ���� 15:20 16:50 130 " << endl;
			cout << ", ��ʾ���Ϊ\"1254huoche\"�Ļ�ͣ�����Σ�12��50�ִ��ൺ������14��50�ֵ����Ͼ����۸�Ϊ280Ԫ��15��20�ִ��Ͼ�������16��50�ֵ������ݣ��۸�Ϊ130Ԫ" << endl;
			cout << "-------------------------------------------" << endl;
			infoInputUser(nu, trainInfo);
			cout << "-------------------------------------------" << endl;
			int newAllNumber = trainInfo.size();
			for (int t = 1; t <= nu; t++)
				insertTrain(trainInfo[newAllNumber - t]);
			cout << "��ӳɹ���" << endl << endl;
			break;
		}
		case 4:
		{
			cout << "����������ɾ���Ļ𳵳��α��:" << endl;
			string trainId;
			cin >> trainId;
			if(eraseVehcle(trainId, 1))cout<<"ɾ���ɹ���"<<endl;
			break;
		}
		case 5:
		{
			int nu;
			cout << "������������ķɻ����ߵ�����:" << endl;
			cin >> nu;
			cout << "�밴�շɻ������ţ��ɻ�ͣ�����Σ�ÿ����ʼ���к��յ���У�ÿ����ʼʱ����յ�ʱ�䣬ÿ�μ۸��˳�����룺" << endl;
			cout << "����1254shandong 1 �ൺ �Ͼ� 12:50 14:50 280 , ��ʾ���Ϊ\"1254shandong\"�ķɻ�ͣ��һ�Σ�12��50�ִ��ൺ������14��50�ֵ����Ͼ����۸�Ϊ280Ԫ" << endl;
			cout << "-------------------------------------------" << endl;
			infoInputUser(nu, flightInfo);
			cout << "-------------------------------------------" << endl;
			int newAllNumber = flightInfo.size();
			for (int t = 1; t <= nu; t++)
				insertFlight(flightInfo[newAllNumber - t]);
			cout << "��ӳɹ���" << endl << endl;
			break;
		}
		case 6:
		{
			cout << "����������ɾ���ĺ�����:" << endl;
			string flightId;
			cin >> flightId;
			if (eraseVehcle(flightId, 0))cout << "ɾ���ɹ���" << endl << endl;
			break;
		}
		case 7:
		{
			string scity, ecity;
			Time beg(0,0,0);
			cout << "���������ĳ������У�" << endl;
			cin >> scity;
			while (mp.find(scity) == mp.end())
			{
				cout << "������ĳ��в����ڣ����������룺" << endl;
				cin >> scity;
			}
			cout << "�����������յ���У�" << endl;
			cin >> ecity;
			while (mp.find(ecity) == mp.end() || scity == ecity)
			{
				if (mp.find(ecity) == mp.end()) {
					cout << "������ĳ��в����ڣ����������룺" << endl;
					cin >> ecity;
				}
				else
				{
					cout << "���ĳ������к��յ����һ�£�����������:"<<endl;
					cin >> ecity;
				}
			}
			string btime;
			cout << "���������ĳ���ʱ��,��11:00:" << endl;
			cin >> btime;
			sscanf(btime.c_str(), "%d:%d", &beg.t[1], &beg.t[2]);

			cout << "���������ĳ������ߣ��ɻ�(0) , �𳵣�1��" << endl;
			int k;
			cin >> k;
			while (!(k == 0 || k == 1))
			{
				cout << "�밴����ĿҪ�����룡" << endl;
				cin >> k;
			}

			cout << "��ѡ���������ž��ߣ����ʱ��(1) ����С����(2) ��������ת(3)" << endl;
			int po;
			cin >> po;
			while (!(po == 1 || po == 2 || po == 3))
			{
				cout << "�밴����ĿҪ�����룡" << endl;
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
			if (!succeed)cout << "��Ǹ��δ���ҵ�����Ҫ���·����" << endl;
		}
		default:
			break;
		}
	}
	
}