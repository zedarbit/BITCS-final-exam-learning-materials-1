
#include <stdio.h>
#include <string.h>
#include <vector>
#include <queue>

using namespace std;

struct Node
{
	int w;
	int h;
	int g;
	vector<int> nexts;
};

int dp[505] = { 0 };
int n;
struct Node a[505] = { 0 };
int t;

void deal()
{
	memset(dp, 0, sizeof(dp));
	memset(a, 0, sizeof(a));
	scanf("%d", &n);
	for (int i = 0; i < n; i++)
	{
		scanf("%d%d%d", &a[i].w, &a[i].h, &a[i].g);
		int m;
		scanf("%d", &m);
		for (int j = 0; j < m; j++)
		{
			int x;
			scanf("%d", &x);
			a[i].nexts.push_back(x - 1);
		}
	}

	queue<int> q;
	for (int i = 0; i < n; i++)
	{
		if (dp[i] < a[i].h + a[i].g)
		{
			dp[i] = a[i].h + a[i].g;
			q.push(i);
		}
	}

	while (!q.empty())
	{
		int x = q.front();
		q.pop();
		for (int i = 0; i < a[x].nexts.size(); i++)
		{
			int to = a[x].nexts[i];
			if (a[to].w < a[x].w)
			{
				if (dp[to] < dp[x] + a[to].h)
				{
					dp[to] = dp[x] + a[to].h;
					q.push(to);
				}
			}
		}
	}
	int res = 0;
	for (int i = 0; i < n; i++)
	{
		if (res < dp[i])
		{
			res = dp[i];
		}
	}
	printf("%d\n", res);
}

int main()
{
	scanf("%d", &t);
	while (t--)
	{
		deal();
	}
}

/*

1
2
5 6 7 1 2
6 6 8 1 1



*/




