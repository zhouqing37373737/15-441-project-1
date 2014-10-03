#include "common.h"
#include <stdlib.h>
#include <string.h>

char *str_tolower(char *str){
	int i;
	for(i=0;str[i]!='\0';i++){
		if(str[i]<='Z'&&str[i]>='A'){
			str[i]=str[i]+'a'-'A';
		}
	}

	return str;
}

char *str_truncate(char *str){
	
	int i;
	i=0;
	while(*str==' '){
		str++;
	}

	while(str[i]!='\0'){
		i++;
	}

	i--;

	while(i>0 && str[i]==' '){
		i--;
	}

	str[i]='\0';
	return str;
}

int str_loosecompare(char *str1,char *str2){
	char *tmp1;
	char *tmp2;
	int result;

	tmp1=(char *)malloc(strlen(str1)+1);
	tmp2=(char *)malloc(strlen(str2)+1);
	strcpy(tmp1,str1);
	strcpy(tmp2,str2);

	tmp1=str_tolower(str_truncate(tmp1));
	tmp2=str_tolower(str_truncate(tmp2));

	result=strcmp(tmp1,tmp2);

	free(tmp1);
	free(tmp2);
	return result;


}
