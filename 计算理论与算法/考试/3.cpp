
#include <stdio.h>
#include <queue>
#include <stdlib.h>
#include <algorithm>

using namespace std;

int ary[25][25] = {0};
int dp[2000005] = {0};
int n, m;

int main()
{
	scanf("%d%d", &n, &m);
	for (int i = 0; i < m; i++)
	{
		int x, y;
		scanf("%d%d", &x, &y);
		ary[x][y] = 1;
		ary[y][x] = 1;
	}

	int res = 0;
	queue<int> q;
	q.push(0);
	while (!q.empty())
	{
		int from = q.front();
		q.pop();
		for (int i = 0; i < n; i++)
		{
			if ((from & (1 << i)) == 0)
			{
				int ok = 1;
				for (int j = 0; j < n; j++)
				{
					if ((from & (1 << j)))
					{
						if (ary[i][j] == 1)
						{
							ok = 0;
							break;
						}
					}
				}
				if (ok)
				{
					if (dp[from ^ (1 << i)] < dp[from] + 1)
					{
						dp[from ^ (1 << i)] = dp[from] + 1;
						q.push(from ^ (1 << i));
						if (res < dp[from ^ (1 << i)])
						{
							res = dp[from ^ (1 << i)];
						}
					}
				}
			}
		}
	}
	printf("%d\n", res);
}

/*

7 10
1 2
1 4
2 4
2 3
2 5
2 6
3 5
3 6
4 5
5 6


*/
