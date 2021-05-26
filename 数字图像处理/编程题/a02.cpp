#include <iostream>
#include <map>
#include <string>

using namespace std;

void encode()
{
    int m, n;
    cin >> m >> n;
    int idleCode = 256;
    map<string, int> dictionary;
    for (int i = 0; i < idleCode; i++)
    {
        dictionary.insert(pair<string, int>(to_string(i), i));
    }
    string P = "";
    string C = "";
    string PC = "";
    bool PC_in_dic = true;
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> C;
            if (P.size())
            {
                PC = P + "-" + C;
            }
            else
            {
                PC = C;
            }

            // 操作
            if (dictionary.count(PC))
            {
                P = PC;
            }
            else
            {
                dictionary.insert(pair<string, int>(PC, idleCode));
                idleCode += 1;
                cout << dictionary[P] << " ";
                P = C;
            }
        }
    }
    cout << dictionary[P] << endl;
}

int main()
{
    encode();
    return 0;
}

/*
4 4
39 39 126 126
39 39 126 126
39 39 126 126
39 39 126 126

39 39 126 126 256 258 260 259 257 126
39 39 126 126 256 258 260 259 257 126
*/
