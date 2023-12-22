#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

#define max_index 3

void set_noncanonical_mode();
void restore_terminal();
void menu_show();
void highlight();
void exe();

/*void menu(int index)
  {

  }

  int highlight()
  {

  }*/

struct MenuItem{
    int optionNum;
    char text[50];
    char cmd[50];
};

struct MenuItem menu[] = {
    {0, "Ifconfig", "ifconfig syn0 192.168.1.25"},
    {1, "Fload", "fload tftp://192.168.1.35/gzrom.bin"},
    {2, "Reboot", "reboot"},
};

int main()
{
    set_noncanonical_mode();

    system("clear");

    int index = 0;
    char input;
    int itemCnt = sizeof(menu) / sizeof(menu[0]);

    menu_show(index, itemCnt);
    while(1){
	read(STDIN_FILENO, &input, 1);
	if(input == '\n')
	    exe(index);

	read(STDIN_FILENO, &input, 1);
	if(input == 'q'){
	    system("clear");
	    break;
	}
	else if(input == 27){
	    read(STDIN_FILENO, &input, 1);

	    if(input == 91){
		read(STDIN_FILENO, &input, 1);

		switch(input){
		    case 'B':
			//printf("Up\n");
			if(index >= 2){
			    index = 0;
			    break;
			}
			index++;
			break;
		    case 'A':
			//printf("Down\n");
			if(index <= 0){
			    index = 2;
			    break;
			}
			index--;
			break;
		    case 'C':
			//printf("Right\n");
			break;
		    case 'D':
			//printf("Left\n");
			break;
		}
		//system("clear");
		menu_show(index, itemCnt);
		//highlight(index);
		printf("index: %d\n", index);
	    }
	}else{
	    printf("%c pressed\n", input);
	}

    }
    restore_terminal();

    printf("\033[31m1. ifconfig\033[0m\n");
    printf("2. fload\n");
    printf("3. reboot\n");
    return 0;
}

void set_noncanonical_mode()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void restore_terminal()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void highlight(int index)
{
    int i = index;
    char *text = menu[i].text;

    printf("text: %s\n",text);
}

void exe(int index)
{
    char cmd[128];
    struct MenuItem *itemSel = &menu[index];
    snprintf(cmd, sizeof(cmd), "echo \"%s\" > /dev/ttyUSB0",itemSel -> cmd);
    printf("cmd :%s\n",cmd);
    system(cmd);
}

void menu_show(int index,int itemCnt)
{
    system("clear");
    for (int i = 0; i < itemCnt; ++i){
	if(i == index){
	    printf("\033[31m%d,%s\033[0m\n",menu[i].optionNum,menu[i].text);
	}else{
	    printf("%d,%s\n",menu[i].optionNum,menu[i].text);
	}
    }
}
