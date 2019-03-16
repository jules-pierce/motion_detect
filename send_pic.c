#include <stdio.h>
#include <stdlib.h>

int main(){
	int i;
	FILE *indes;
	char string1[500], string2[500];

	system("./imagesnap tmp_image.jpg >& /dev/null ");
	system("qlmanage -p tmp_image.jpg &> /dev/null       & ");

	printf("Enter an integer to continue:  \n");
	scanf("%d", &i);

	// Kill the qlmanage program when it is time to exit, and delete the
	// image we created
	system("ps aux | grep qlmanage | grep -v grep &> tmp_data.txt");
	indes = fopen("tmp_data.txt", "r");
	fscanf(indes, "%s %s", string1, string2);
	fclose(indes);
	system("rm -f tmp_data.txt");

	sprintf(string1, "kill -9 %s", string2);
	system(string1);
	system("rm -f tmp_image.jpg");
}
