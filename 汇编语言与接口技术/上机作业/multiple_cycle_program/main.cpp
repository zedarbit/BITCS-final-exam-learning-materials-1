#include <stdio.h>
int main() {
	int N = 2;
	bool flag = false;
	for (int i = 0; i * i < N; i++) {
		for (int j = 0; j * j < N; j++) {
			for (int z = 0; z * z < N; z++) {
				for (int y = 0; y * y < N; y++) {
					int ans = i * i + j * j + z * z + y * y;
					if (ans == N) {
						printf("%d^2 + %d^2  + %d^2 + %d^2 = %d\n", i, j, z, y, N);
						flag = true;
					}
				}
			}
		}
	}
	if (flag) {
		printf("No solution.\n");
	}
}