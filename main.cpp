#include<bits/stdc++.h>
using namespace std;
#define PII pair<int,int>
//std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
int text_berth[10];
fstream fs;
namespace transport {
	const int N = 210;
	const int n = 200;
	const int berth_num = 10;
	const int robot_num = 10;
	const int boat_num = 5;
	const int null = -18118515;
	const int change_cnt = 1;
	string ch[N];
	int value_in_map[N][N];
	map<PII, bool>ai_pos;
	struct Berth
	{
		int x, y;
		int transprot_time;
		int loading_speed;
		int tot_value = 0;
		queue<int>items;
		bool st = false;
	}berth[berth_num * 2];
	struct Robot
	{
		int x, y, goods;
		int status;
		int direction = 0;
		int take_item_value = 0;
		int to;
		bool be = false;
		bool change = false;
		vector<PII>copy;
	}robot[robot_num];
	struct Boat
	{
		int status, pos;
		int capacity_now;
		int pre_pos;
	}boat[boat_num];
	int maps[N][N][berth_num * 2];
	int ai_road[N][N][robot_num * 2];
	int pos[4][2] = { {0,1},{0,-1},{-1,0},{1,0} };
	/*0 表示右移一格
	 1 表示左移一格
	 2 表示上移一格
	 3 表示下移一格
	 */
	int boat_capacity;//船的容积，unuse
	int money;
	priority_queue<pair<PII, int>>_[berth_num * 2];
	void bfs_init(PII be, int idx)
	{
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				maps[i][j][idx] = null;
			}
		}
		queue<PII>que;
		que.push(be);
		maps[be.first][be.second][idx] = 0;
		while (que.size()) {
			PII t = que.front();
			que.pop();
			for (int i = 0; i < 4; i++) {
				int xx = t.first + pos[i][0], yy = t.second + pos[i][1];
				if (xx < 0 || xx >= n || yy < 0 || yy >= n) continue;
				if (ch[xx][yy] == '*' || ch[xx][yy] == '#') continue;
				if (maps[xx][yy][idx] != null) continue;
				//if (ch[xx][yy] == 'B' && (xx >= berth[idx].x && xx <= berth[idx].x + 3) && (yy >= berth[idx].y && yy <= berth[idx].y + 3)) {
				//	maps[xx][yy][idx] = 0;
				//	que.push({ xx,yy });
				//}
				//else {
				maps[xx][yy][idx] = maps[t.first][t.second][idx] + 1;
				que.push({ xx,yy });
				//}
			}
		}
	}
	void init()
	{
		for (int i = 0; i < n; i++) {
			cin >> ch[i];
			for (int j = 1; j <= n; j++) {
				if (ch[i][j] == 'A') {
					ch[i][j] = '.';
				}
			}
		}
		for (int i = 0; i < berth_num; i++) {
			int id;
			cin >> id;
			cin >> berth[id].x >> berth[id].y >> berth[id].transprot_time >> berth[id].loading_speed;

			berth[id + 10] = berth[id];
			berth[id + 10].x += 3, berth[id + 10].y += 3;

		}
		for (int i = 0; i < berth_num * 2; i++) {
			bfs_init({ berth[i].x,berth[i].y }, i);
		}
		cin >> boat_capacity;
		string okk;
		cin >> okk;
		if (okk != "OK") {
			exit(0);
		}
		puts("OK");
		fflush(stdout);
	}
	void find_short_berth(PII position, int& p)
	{
		int maxx = 1e9;
		int op = 0;
		for (int i = 0; i < berth_num; i++) {
			//if(ch[position.first][position.second]>=0&&ch[position.first][position.second]<10){
			if (position.first >= berth[i].x && position.first <= berth[i].x + 3 && position.second >= berth[i].y && position.second <= berth[i].y + 3) {
				//maxx = maps[position.first][position.second][i];
				p = ch[position.first][position.second];
				return;
			}
		}
		if (maxx == 1e9)
			p = null;
	}
	int find_short_berth(int i, PII position) {
		for (int j = 0; j <= 3; j++) {
			int xx = position.first + pos[j][0], yy = position.second + pos[j][1];
			if (maps[xx][yy][i] == maps[position.first][position.second][i] - 1) {
				return j;
			}
		}
		return null;
	}
	int mindstance(PII position)
	{
		int minn = 1e9;
		for (int i = 0; i < berth_num; i++) {
			minn = min(minn, maps[position.first][position.second][i]);
		}
		return minn;
	}
	void item_to_berth(PII _item, int ji) {
		//int dis_of_item = mindstance(_item) / change_o;
		PII now = _item;
		if (maps[_item.first][_item.second][ji] != null) {
			while (true) {
				//item_road[now.first][now.second] -= real_value;
				ai_road[now.first][now.second][ji] = 1;
				if (maps[now.first][now.second][ji] == 0) break;
				for (int i = 0; i < 4; i++) {
					int xx = now.first + pos[i][0], yy = now.second + pos[i][1];
					if (maps[xx][yy][ji] == maps[now.first][now.second][ji] - 1) {
						now = { xx,yy };
						break;
					}
				}
			}
		}
	}
	struct Item
	{
		int x, y;
		int id;
		int flag;
	};
	vector<Item>total_item;
	struct Operators
	{
		string op;
		int idx1;
		int idx2;
	};
	vector<Operators>Output;
	int ai_find_road(PII sta, int berth_id) {
		for (int i = 0; i < 4; i++) {
			int xx = sta.first + pos[i][0], yy = sta.second + pos[i][1];
			if (xx < 0 || xx >= n || yy < 0 || yy >= n) continue;
			if (ch[xx][yy] == '#' || ch[xx][yy] == '*') continue;
			if (ai_road[xx][yy][berth_id] == 1) {
				//ai_road[sta.first][sta.second][berth_id] = 2;
				return i;
			}
		}
		return null;
		//return ji;
	}
	int ai_find_road_back(PII sta, int berth_id) {
		for (int i = 0; i < 4; i++) {
			int xx = sta.first + pos[i][0], yy = sta.second + pos[i][1];
			if (xx < 0 || xx >= n || yy < 0 || yy >= n) continue;
			if (ch[xx][yy] == '#' || ch[xx][yy] == '*') continue;
			if (ai_road[xx][yy][berth_id] == 2) {
				//ai_road[sta.first][sta.second][berth_id] = 0;
				return i;
			}
		}
	}
	void debug_show(int idx)
	{
		for (int i = 1; i <= n; i++) {
			for (int j = 1; j <= n; j++) {
				if (ai_road[i][j][idx] == 0)
					fs << ".";
				else if (ai_road[i][j][idx] == 1)
					fs << "+";
				else
					fs << "#";
			}
			fs << "\n";
		}
	}
	bool show_path(int idx, int now_time) {
		while (_[idx].size()) {
			auto w = _[idx].top();
			_[idx].pop();
			w.second = -w.second;
			if (total_item[w.second].flag == true) continue;
			int x = total_item[w.second].x, y = total_item[w.second].y;
			int id = total_item[w.second].id;
			if (now_time + maps[x][y][idx] + 5 <= id + 1000) {
				item_to_berth({ x,y }, idx);
				total_item[w.second].flag = true;
				return true;
			}
		}
		return false;
	}
	int Input()
	{
		int id;
		cin >> id >> money;
		//需要优化
		//debug_show(robot[1].to);
		//item_change(item, id);
		int k;
		cin >> k;
		for (int i = 1; i <= k; i++) {
			int x, y, val;
			cin >> x >> y >> val;
			if (ch[x][y] == '*' || ch[x][y] == '#') continue;
			value_in_map[x][y] = val;
			total_item.push_back({ x,y,id ,false });
			for (int j = 0; j < 2 * berth_num; j++) {
				if (maps[x][y][j] == null) continue;
				_[j].push({ {val * 10 / maps[x][y][j],-maps[x][y][j]},-(int)(total_item.size() - 1) });
			}
			//_[p].push({ max(val * 100 / mindstance({ x,y }),1),total_item.size() - 1 });
			//item_to_berth({ x,y }, val);
		}
		ai_pos.clear();
		for (int i = 0; i < robot_num; i++)
		{
			int sts;
			cin >> robot[i].goods >> robot[i].x >> robot[i].y >> sts;
			ai_pos[{robot[i].x, robot[i].y}] = 1;
			if (id == 1) continue;
			if (1) {
				//copy优先度最高
				if (robot[i].copy.size()) {
					for (int j = 0; j < 4; j++) {
						int xx = robot[i].x + pos[j][0], yy = robot[i].y + pos[j][1];
						if (robot[i].copy.back() == make_pair(xx, yy)) {
							Output.push_back({ "move", i, j });
							break;
						}
					}
					continue;
				}

				if (robot[i].goods == 0) {
					if (robot[i].be == false) {
						int val = find_short_berth(robot[i].to, { robot[i].x,robot[i].y });
						int p = robot[i].to;
						if (val == null) {
							if (show_path(p, id)) {
								robot[i].be = true;
							}
							else continue;
						}
						else {
							Output.push_back({ "move",i,val });
							continue;
						}
					}
					int _operator = ai_find_road({ robot[i].x,robot[i].y }, robot[i].to);
					if (_operator == null) continue;
					Output.push_back({ "move",i,_operator });
				}
				else {
					int _operator = ai_find_road_back({ robot[i].x,robot[i].y }, robot[i].to);
					if (_operator == null) continue;
					Output.push_back({ "move", i, _operator });
				}
			}
			else Output.push_back({ "move", i, 0 });
		}
		if (id == 1) {
			vector<int>st(berth_num);
			for (int i = 0; i < robot_num; i++) {
				int minn = 1e9, ji = -1;
				for (int j = 0; j < berth_num; j++) {
					if (st[j]) continue;
					if (maps[robot[i].x][robot[i].y][j] == null) continue;
					if (maps[robot[i].x][robot[i].y][j] < minn) {
						minn = maps[robot[i].x][robot[i].y][j];
						ji = j;
					}
				}
				if (ji == -1) {
					robot[i].be = true; robot[i].to = 19;
				}
				else {
					st[ji] = true;
					robot[i].to = ji;
					robot[i].be = false;
				}
			}
		}
		for (int i = 0; i < boat_num; i++)
		{
			cin >> boat[i].status >> boat[i].pos;
			if ((id + berth[boat[i].pre_pos].transprot_time) % 3000 == 0) {
				Output.push_back({ "go",i,null });
			}
			if ((id + 500 + 1250) % 3000 == 0) {
				vector<PII>capacitys;
				vector<PII>tot_val;
				for (int i = 0; i < 5; i++) {
					if (boat[i].pos > 5)
						tot_val.push_back({ berth[i].tot_value,i });
					//Output.push_back({ "ship", i, i });
					else
						tot_val.push_back({ berth[i + 5].tot_value,i + 5 });
					capacitys.push_back({ boat[i].capacity_now ,i });
				}
				sort(capacitys.begin(), capacitys.end());
				sort(tot_val.begin(), tot_val.end());
				for (int i = 0; i < 5; i++) {
					Output.push_back({ "ship",capacitys[i].second,tot_val[i].second });
				}
			}
			if (boat[i].status == 1 && boat[i].pos == -1) {
				//if(id<=200)
				boat[i].capacity_now = boat_capacity;
				//if (berth[i].tot_value < berth[i + 5].tot_value)
				Output.push_back({ "ship",i,i });
				//else
					//Output.push_back({ "ship", i, i + 5 });
			}
			else if (boat[i].status == 0 && boat[i].pos != -1) {
				boat[i].pre_pos = boat[i].pos;
				int nums = berth[boat[i].pos].loading_speed;
				while (berth[boat[i].pos].items.size() && nums-- && boat[i].capacity_now) {
					auto t = berth[boat[i].pos].items.front();
					berth[boat[i].pos].items.pop();
					berth[boat[i].pos].tot_value -= t;
					boat[i].capacity_now--;
				}
				//	if ((id + berth[boat[i].pre_pos].transprot_time-1500)%3000==0) {
				//		Output.push_back({ "ship", i, i + 5 });
				//	}
			}
		}
		string okk;
		cin >> okk;
		if (okk != "OK") {
			exit(0);
		}
		return id;
	}
	void cout_operator(int id)
	{
		for (int i = 0; i < Output.size(); i++) {
			if (Output[i].idx2 != null) {
				int num, idx;
				int xx, yy;
				bool ok = false;
				if (Output[i].op == "move") {
					num = Output[i].idx1, idx = Output[i].idx2;
					xx = robot[num].x + pos[idx][0], yy = robot[num].y + pos[idx][1];
					if (ai_pos.count({ xx,yy }) && ai_pos[{xx, yy}] == 1) {
						int ji = null;
						int be = (rand() + i) % 4;
						int cnt = 4;
						while (cnt--) {
							be++;
							be %= 4;
							int _x = robot[num].x + pos[be][0], _y = robot[num].y + pos[be][1];
							if (ch[_x][_y] == '*' || ch[_x][_y] == '#') continue;
							if (ai_pos.count({ _x,_y }) && ai_pos[{_x, _y}] == 1) continue;
							ji = be;
							ok = true;
							break;
						}
						if (ji == null) continue;
						Output[i].idx2 = ji;
					}
				}
				printf("%s %d %d\n", Output[i].op.c_str(), Output[i].idx1, Output[i].idx2);
				xx = robot[num].x + pos[Output[i].idx2][0], yy = robot[num].y + pos[Output[i].idx2][1];
				if (Output[i].op == "move") {//添加一个避让功能
					if (ok == true) {
						robot[num].copy.push_back({ robot[num].x,robot[num].y });
					}
					else {
						if (robot[num].copy.size()) robot[num].copy.pop_back();
						else if (ai_road[xx][yy][robot[num].to] == 1) {
							ai_road[robot[num].x][robot[num].y][robot[num].to] = 2;
							int _operator = ai_find_road({ xx,yy }, robot[num].to);
							if (_operator == null) {
								printf("get %d\n", num);
								robot[num].take_item_value = value_in_map[xx][yy];

							}
						}
						else if (ai_road[xx][yy][robot[num].to] == 2) {
							ai_road[robot[num].x][robot[num].y][robot[num].to] = 0;
							if (maps[xx][yy][robot[num].to] == 0) {
								printf("pull %d\n", num);
								robot[num].be = false;
								int p;
								p = robot[num].to;
								berth[p].items.push(robot[num].take_item_value);
								berth[p].tot_value += robot[num].take_item_value;
								text_berth[p] += robot[num].take_item_value;
							}
						}
					}
					ai_pos[{xx, yy}] = 1;
					ai_pos[{robot[num].x, robot[num].y}] = 0;
				}
			}
			else
				printf("%s %d\n", Output[i].op.c_str(), Output[i].idx1);
		}
		Output.clear();
		puts("OK");
		fflush(stdout);
	}
}
using namespace transport;
signed main()
{
	fs.open("temp.txt", ios::out);
	ios_base::sync_with_stdio(0); cin.tie(0);
	init();
	for (int fps = 1; fps <= 15000; fps++) {
		int id = Input();
		cout_operator(id);
		fs << id << "--------------------------------\n";
		for (int i = 0; i < 10; i++) {
			fs << text_berth[i] << ' ';
		}
		fs << '\n';
		for (int i = 0; i < 10; i++) {
			fs << berth[i].tot_value << ' ';
		}
		fs << "\n";
	}

}
