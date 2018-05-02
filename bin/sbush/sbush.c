#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
  // puts("sbush> ");
  // return 0;
	// while(1);

	// const char *str = "I am in Sbush";
	// int i = 123;
	printf("sbush:");

	// printf("%s %s %s", str, str, str);
	// printf("%d", i);
	char *s = '\0';
	getcwd(s, 100);
	while(1) {
		s = gets(s);
		printf("hi\n");
		printf(s);
		// if(strcmp(s, "ls")) {
		// 	execvpe(s, argv, envp);
		// }
		// printf(s);
	}
	// __asm__ __volatile__ ("int $0x80");
	while(1);
	return 0;
}
