
#include "includes.h"

#define TASK_STK_SIZE 512

void keyhandle(void*);
void Player(void*);
void gamestatus(void*);
void Move(INT16S key);

OS_EVENT* keySem;
OS_EVENT* gameSem;
INT8U turn;
INT8U datax, datay;
INT16S datakey;
INT8U board[3][3];

typedef enum GAME_RESULT {
	PLAYER_A_WINS,
	PLAYER_B_WINS,
	DRAW,
	INCOMPLETE
} GameResult;

typedef struct PLAYER_DATA {
	INT8U id;
	OS_EVENT* sem;
} PlayerData;

PlayerData plr_data[2];
OS_STK keyboard_stk[TASK_STK_SIZE];
OS_STK gamestatus_stk[TASK_STK_SIZE];
OS_STK player_stk[2][TASK_STK_SIZE];

GameResult getResult();

int main(void) {
	PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);
	// Initialize uC/OS-II
	OSInit();
	datakey = 0;
	keySem = OSSemCreate(0);
	INT8U m, n;
	for (m=0; m<3; m++){
		for (n=0; n<3; n++){
			board[m][n] = 0;
		}
	}
	datax = 0;
	datay = 0;
	turn = 2;
	gameSem = OSSemCreate(1);
	for (int i=0; i<2; i++) {
		plr_data[i].sem = OSSemCreate(0);
		plr_data[i].id = 1 + i;
	}

	OSTaskCreate(keyhandle, NULL, &keyboard_stk[TASK_STK_SIZE - 1], 0);
	OSTaskCreate(gamestatus, NULL, &gamestatus_stk[TASK_STK_SIZE - 1], 1);
	for (int i=0; i<2; i++) {
		OSTaskCreate(Player, &plr_data[i], &player_stk[i][TASK_STK_SIZE - 1], 2+i);
	}

	OSStart();
	return 0;
}

void keyhandle(void *data) {
	// Prevent compiler warning
	data = data;
	INT16S key;
	for (;;) {
		// If a key has been pressed
		if (PC_GetKey(&key) == TRUE) {
			// And it's the ESCAPE key
			if (key == 0x1B) {
				// End program
				PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);
				exit(0);
			} else {		
				OSSemAccept(keySem);
				datakey = key;
				OSSemPost(keySem);
			}
		}
		OSTimeDlyHMSM(0, 0, 0, 100);
	}
}

void gamestatus(void *data) {
	data = data;
	INT8U err, first = TRUE;

	for (;;) {
		OSSemPend(gameSem, 0, &err);
		if (first) {
			first = FALSE;
		} else {
			board[datax][datay] = turn;
		}
		GameResult res = getResult();
		if (res != INCOMPLETE) {
			// print result
			if (res == PLAYER_A_WINS) {
				PC_DispStr(9, 20, "X wins.	", DISP_FGND_WHITE + DISP_BGND_BLACK);
			} else if (res == PLAYER_B_WINS) {
				PC_DispStr(9, 20, "O wins.	", DISP_FGND_WHITE + DISP_BGND_BLACK);
			} else {
				PC_DispStr(9, 20, "draw.		", DISP_FGND_WHITE + DISP_BGND_BLACK);
			}
			// infinite halt
			while (1) {
				OSTimeDly(1000);
			}
		}
		if (turn == 1) {
			turn = 2;
		} else {
			turn = 1;
		}
		OSSemPost(plr_data[turn - 1].sem);
	}
}

void Player(void *data) {
	PlayerData* player = data;
	INT8U err, end;
	INT16S key;

	for (;;) {
		OSSemPend(player->sem, 0, &err);
		end = FALSE;
		while (!end) {						
				INT8U x, y, c, color, buf[100];
				for (x=0; x<7; x++){
					for (y=0; y<7; y++){
						color = DISP_FGND_WHITE + DISP_BGND_BLACK;
						if (x%2 && y%2) {
							if (board[x/2][y/2] == 1) {
								c = 'X';
							} else if (board[x/2][y/2] == 2) {
								c = 'O';
							} else {
								c = ' ';
							}
							if (x/2 == datax && y/2 == datay) {
								color = DISP_FGND_WHITE + DISP_BGND_BLUE;
								if (c == ' ') {
									color = DISP_FGND_CYAN + DISP_BGND_BLUE;
									c = turn == 1 ? 'X': 'O';
								}
							}
						} else if (x%2) {
							c = '-';
						} else if (y%2) {
							c = '|';
						} else {
							c = '+';
						}
						PC_DispChar(40 + x, 7 + y, c, color);
					}
				}
				PC_DispStr(20, 1, "[Space] move.", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(20, 2, "[Enter] select.", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(20, 3, "[Esc] quit.", DISP_FGND_WHITE + DISP_BGND_BLACK);
				PC_DispStr(20, 4, buf, DISP_FGND_WHITE + DISP_BGND_BLACK);
				
			OSSemPend(keySem, 0, &err);
			key=datakey;
			Move(key);
			end = ((key == 0x0d) && (board[datax][datay] == 0));
		}
		OSSemPost(gameSem);
	}
}
/*
**************************************************************************
* NON-TASK FUNCTIONS
**************************************************************************
*/

void Move(INT16S key){
	if(key == 0x20) {
		if(datax<2){
			datax++;
		}else if(datax==2 && datay!=2){
			datay++;
			datax=0;
		}else if(datax==2 && datay==2){
			datax=0;
			datay=0;
		}
	}	
}

GameResult getResult() {
	int x, y, sum;
	for(x = 0; x < 3; x++) {
		sum = 0;
		for (y = 0; y < 3; y++) {
			sum += (board[x][y] == 2) ? -1 : board[x][y];
		}
		if (sum == 3 || sum == -3){
			return sum > 0 ? PLAYER_A_WINS : PLAYER_B_WINS;
		}
	}
	for(x = 0; x < 3; x++) {
		sum = 0;
		for (y = 0; y < 3; y++) {
			sum += (board[y][x] == 2) ? -1 : board[y][x];
		}
		if (sum == 3 || sum == -3){
			return sum > 0 ? PLAYER_A_WINS : PLAYER_B_WINS;
		}
	}
	for(x = 0; x < 3; x+=2) {
		sum = 0;
		for (y = 0; y < 3; y++) {
			sum += (board[y][x+y-x*y] == 2) ? -1 : board[y][x+y-x*y];
		}
		if (sum == 3 || sum == -3){
			return sum > 0 ? PLAYER_A_WINS : PLAYER_B_WINS;
		}
	}
	sum = 0;
	for(x = 0; x < 3; x++) {
		for (y = 0; y < 3; y++) {
			sum += (board[x][y] != 0) ? 1 : 0;
		}
	}
	return sum < 9 ? INCOMPLETE : DRAW;
}
