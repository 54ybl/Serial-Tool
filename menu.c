#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

void set_noncanonical_mode();
void restore_terminal();
void menu_show();
void exe();
void listen();

#define SERIAL_PORT "/dev/ttyUSB0"


/* Menu struct */
struct MenuItem{
    //int optionNum;
    char text[50];
    char cmd[50];
};

/* Menu content & command */
struct MenuItem menu[] = {
    {"Ifconfig", "ifconfig syn0 192.168.1.25"},
    {"Fload", "fload tftp://192.168.1.35/gzrom.bin"},
    {"Load", "load tftp://192.168.1.35/vmlinuz"},
    {"Initrd", "initrd tftp://192.168.1.35/rootfs.cpio.gz"},
    {"Call kernel", "g console=ttyS0,115200"},
    {"Ping", "ping 192.168.1.35"},
    {"Reboot", "reboot"},
    {"Poweroff", "poweroff"},
    {"Break", ""},
};

struct MenuItem kmenu[] = {
    {"Ifconfig", "ifconfig eth0 192.168.1.25"},
    {"Ping", "ping 192.168.1.35"},
    {"Reboot", "reboot"},
    {"Poweroff", "poweroff"},
    {"Break", ""},
};

int main()
{
    set_noncanonical_mode();	//Set non-canonical mode

    system("clear");

    int index = 0;
    int jinx = 0;
    char input;
    int itemCnt = sizeof(menu) / sizeof(menu[0]); //Num of menu items

    menu_show(index, itemCnt, jinx);
    printf("index: %d\n", index);
    while(1) {
	/* Read Enter */
	read(STDIN_FILENO, &input, 1);
	if (input == '\n') {
	    exe(index);
	}

	/* Read q */
	else if (input == 'q') {
	    system("clear");
	    break;
	}

	/* Read Up/Down */
	else if (input == 27) {
	    read(STDIN_FILENO, &input, 1);
	    if (input == 91) {
		read(STDIN_FILENO, &input, 1);

		switch (input) {
		    case 'B':
			//printf("Down\n");
			if (index >= (itemCnt - 1)) {
			    index = 0;
			    break;
			}
			index++;
			break;
		    case 'A':
			//printf("Up\n");
			if (index <= 0) {
			    index = (itemCnt - 1);
			    break;
			}
			index--;
			break;
		    case 'C':
			//printf("Right\n");
			jinx = !jinx;
			if (jinx)
			    itemCnt = sizeof(kmenu) / sizeof(kmenu[0]);     //Num of menu items
			else
			    itemCnt = sizeof(menu) / sizeof(menu[0]); //Num of menu items
			break;
		    case 'D':
			//printf("Left\n");
			jinx = !jinx;
			if (jinx)
			    itemCnt = sizeof(kmenu) / sizeof(kmenu[0]);     //Num of menu items
			else
			    itemCnt = sizeof(menu) / sizeof(menu[0]); //Num of menu items
			break;
		}
		//system("clear");
		menu_show(index, itemCnt, jinx);
		//printf("index: %d\n", index);
	    }
	} else if (input >= '0' && input <= '9'){
	    //printf("num: %d\n",input - '0');
	    index = input - '0' - 1;
	    menu_show(index, itemCnt, jinx);
	} else {
	    printf("%c pressed\n", input);
	}
	printf("index: %d\n", index);
    }
    restore_terminal();

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

void exe(int index, int jinx)
{
    char cmd[128];
    //struct MenuItem *itemSel = &menu[index];
    //snprintf(cmd, sizeof(cmd), "echo \"%s\" > /dev/ttyUSB0",itemSel -> cmd);
    struct MenuItem *selectedMenu;
    if (jinx)
        selectedMenu = kmenu;
    else
        selectedMenu = menu;
    snprintf(cmd, sizeof(cmd), "echo \"%s\" > %s",selectedMenu[index].cmd,SERIAL_PORT);
    printf("cmd: %s\n",cmd);
    system("echo \x03 > /dev/ttyUSB0");
    system(cmd);
}

/*void listen()
  {
  while(1){}
  }*/

void menu_show(int index,int itemCnt, int jinx)
{
    struct MenuItem *selectedMenu;
    if (jinx)
	selectedMenu = kmenu;
    else
	selectedMenu = menu;
    system("clear");
    for (int i = 0; i < itemCnt; ++i) {
	if (i == index) {
	    printf("\033[31m%d.%s <\033[0m\n",i + 1, selectedMenu[i].text);
	} else {
	    printf("%d.%s\n",i + 1, selectedMenu[i].text);
	}
    }
}
