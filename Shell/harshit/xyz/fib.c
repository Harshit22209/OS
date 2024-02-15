/*
 * No changes are allowed in this file
 */
 #include<stdio.h>
int fib(int n) {
  if(n<2) return n;
  else return fib(n-1)+fib(n-2);
}

int main(int argc, char** argv) {
	char  *a=argv[1];
	printf("Hello world");
	int n=0;
	while(*a!='\0'){
		 n=n*10+(*a-'0');
		 a++;
		 printf("%d %s",n,a);
		 
	}
	int val = fib(n);
	printf("result: %d\n",val);
}
