#include<stdio.h>
#include <string.h>

int main() {
	char snum1[200] = { '\0' };
	char snum2[200] = { '\0' };
	scanf("%s", snum1);
	scanf("%s", snum2);

	const int len1 = strlen(snum1);
	const int len2 = strlen(snum2);

	int num1[200] = { 0 };
	int num2[200] = { 0 };

	for (int i = 0, j = len1 - 1; i < len1; i++, j--) {
		num1[i] = snum1[j] - '0';
	}
	for (int i = 0, j = len2 - 1; i < len2; i++,j--) {
		num2[i] = snum2[j] - '0';
	}

	int ans[210] = { 0 };
	for (int i = 0; i < len1; i++) {
		for (int j = 0; j < len2; j++) {
			ans[i + j] += num1[i] * num2[j];
		}
	}

	int len = len1 + len2 - 2;
	for (int i = 0; i < len; i++) {
		int carry = ans[i] / 10;
		ans[i] %= 10;
		ans[i + 1] += carry;
		
		if (i == len - 1 && carry != 0) {
			len++;
		}
	}

	for (int i = len; i >= 0; i--) {
		printf("%d", ans[i]);
	}
	printf("\n");
}