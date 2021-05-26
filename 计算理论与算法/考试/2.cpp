
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <map>

using namespace std;

struct Node
{
	int deadline;
	int penalty;
};

struct Node ary[10005] = {0};

int n;

bool deal(Node x, Node y)
{
	if (x.penalty == y.penalty)
		return x.deadline > y.deadline;
	return x.penalty > y.penalty;
}

int main()
{
	scanf("%d", &n);
	for (int i = 0; i < n; i++)
	{
		scanf("%d", &ary[i].deadline);
	}
	for (int i = 0; i < n; i++)
	{
		scanf("%d", &ary[i].penalty);
	}
	sort(ary, ary + n, deal);

	int result = 0;
	map<int, int> mymap;
	for (int i = 0; i < n; i++)
	{
		int ok = 0;
		for (int j = ary[i].deadline; j > 0; j--)
		{
			if (mymap.find(j) == mymap.end())
			{
				mymap[j] = 1;
				ok = 1;
				break;
			}
		}
		if (ok == 0)
		{
			result += ary[i].penalty;
		}
	}
	printf("%d\n", result);
}

/*

3
2 2 2
6 7 8


*/
