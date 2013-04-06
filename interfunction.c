#include <stdio.h>

void a(int x,int y) {
	static int i;
	i= x+y;
	return;
}

int b() {
	static int i;
	return i;
}

int main() {
	a(4,5);
	printf("4+5=%d\n",b());
	return 0;
}
