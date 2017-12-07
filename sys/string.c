#include <string.h>
#include <sys/kprintf.h>

int strrev(char *str) {
	int i = 0, j = strlen(str);
	while(i < j) {
		char ch = str[i];
		str[i] = str[j];
		str[j] = ch;
		i++;
		j--;
	}
	return 1;
}

int strcmp(char *str1, char *str2) {
	int i = 0;
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	if(len1 > len2) {
		return 1;
	} else if(len1 < len2) {
		return -1;
	}
	while(str1[i] != '\0' && str1[i] == str2[i]) {
		i++;
	}
	if(str1[i] == '\0') {
		return 0;
	}
	if(str1[i] > str2[i]) {
	  return 1;
	}
	else {
	  return -1;
	}
	//return str1[i] > str2[i];
}

int strsubcmp(char *str1, char *str2, int from, int to) {
	int i = 0;
	int j = from;
	int len1 = strlen(str1);
	//int len2 = strlen(str2);
	int len2 = to - from;
	if(len1 > len2) {
		return 1;
	} else if(len1 < len2) {
		return -1;
	}
	while(str1[i] != '\0' && str1[i] == str2[j] && j < to) {
		i++;
		j++;
	}
	if(str1[i] == '\0') {
		return 0;
	}
	if(str1[i] > str2[j]) {
	  return 1;
	}
	else {
	  return -1;
	}
	//return str1[i] > str2[i];
}

int strcpy(char *str1, char *str2) {
	int i = 0;
	int len = strlen(str2);
	for(i = 0 ; i < len; i++) {
		str1[i] = str2[i];
	}
	str1[i] = '\0';
	return 1;
}

int strsubcpy(char *str1, char *str2, int from, int to) {
	int i;
	for(i = from; i < to; i++) {
		str1[i] = str2[i];
	}
	return 1;
}

int strcat(char *str1, char *str2) {
	int len1 = strlen(str1);
	//int len2 = strlen(str2);
	//str1 = realloc(str1, sizeof(char) * (len1 + len2 + 1));
	strcpy(&str1[len1], str2);
	return 1;
}

int strlen(char *str) {
	int len = 0;
	while(str[len++] != '\0');
	return len - 1;
}

// concatenates the first string to the second string
int strrevcat(char *str1, char *str2) {
	int len2 = strlen(str2);
	//int len2 = strlen(str2);
	//str1 = realloc(str1, sizeof(char) * (len1 + len2 + 1));
	strcpy(&str2[len2], str1);
	return 1;
}
