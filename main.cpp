#include "core.h"

int main(int argc, char** argv) {
	Node_st* xml = Parse("F:\\Documents and Settings\\baojing_zhou\\桌面\\test.xml");
	
	fflush(stdin);
	printf("press ENTER to exit...\n");
	getchar();

	return 0;
}