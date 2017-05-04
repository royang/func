#include <stdio.h>

void test(){
	printf("this func is %s\n",__func__);
}

int main(int argc,char** argv){
	test();
	printf("this func is %s\n",__func__);
}
