#include <stdlib.h>

void _start(void) {
  // call main() and exit() here
	int argc = 1;
	char *argv[1] = {NULL};
	char *envp[1] = {NULL};
	main(argc, argv, envp);
	// exit(0);
}
