//테트리스 21.12.05 시작 
//▣ 

#pragma warning(disable:4996)		//getch & kbhit 오류 제거

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include <mmsystem.h>		//playsound & mci용 립

#pragma comment(lib,"winmm.lib")		//playsound & mci용 립
#include <Digitalv.h>		//mci용 헤더

#define L 75		//방향키 정의
#define R 77	
#define U 72
#define D 80

MCI_OPEN_PARMS openBgm;		//bgm 구조체
MCI_PLAY_PARMS playBgm;
MCI_OPEN_PARMS openfallsound;		//블록착지음 구조체
MCI_PLAY_PARMS playfallsound;
MCI_OPEN_PARMS openlinesound;		//줄 삭제음 구조체
MCI_PLAY_PARMS playlinesound;

int dwID;		//dwID는 MCI_OPEN_PARMS이 담고 있는 wDeviceID를 받기 위한 변수다
int dwID1;
int dwID2;

void MainSound(int a)		//bgm 재생
{
	openBgm.lpstrElementName = TEXT("mainbgm.wav"); //파일 오픈
	openBgm.lpstrDeviceType = TEXT("mpegvideo"); //mp3 형식
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openBgm);
	dwID = openBgm.wDeviceID;
	if(a==0)		//재생 신호시 재생
		mciSendCommand(dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)&openBgm);		//음악 반복 재생
	else        //정시 신호시 정지
		mciSendCommand(dwID, MCI_PAUSE, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)&openBgm);		//재생 정지
}

void playsoundline()		//줄 제거 효과음 재생
{
	openlinesound.lpstrElementName = TEXT("line.wav"); //파일 오픈
	openlinesound.lpstrDeviceType = TEXT("mpegvideo"); //mp3 형식
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openlinesound);
	dwID1 = openlinesound.wDeviceID;
	mciSendCommand(dwID1, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL); //음원 재생 위치를 처음으로 초기화
	mciSendCommand(dwID1, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&openlinesound); //음악을 한 번 재생
}

void playsoundfall()		//블록 착지 효과음 재생
{
	openfallsound.lpstrElementName = TEXT("fall.wav"); //파일 오픈
	openfallsound.lpstrDeviceType = TEXT("mpegvideo"); //mp3 형식
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openfallsound);
	dwID2 = openfallsound.wDeviceID;	
	mciSendCommand(dwID2, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL); //음원 재생 위치를 처음으로 초기화
	mciSendCommand(dwID2, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&openfallsound); //음악을 한 번 재생

}

void gotoxy(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void cursorview()
{
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1; //커서 굵기 (1 ~ 100)
	cursorInfo.bVisible = FALSE; //커서 Visible TRUE(보임) FALSE(숨김)
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

int x = 18; //게임 화면의 중앙 x값 
int y = 7; //게임 화면의 가장 윗 줄
int exx = 0;		//미리보기의 이전 x좌표
int exy = 0;		//미리보기의 y좌표
int exacblock[4][4];		//acblock의 이전 모양
int excurrent;		//전 블록이 저장된 위치
int score = 0;		//점수

int yset(void)		//y좌표 계산  
{
	if (y >= 7)
		return y - 7;

	else		//y좌표가 음수일 때 
		return 0;
}

int gameblock[25][12];		//게임화면에 블록이 있으면 1, 없으면 0 

void gameblockset(void)		//게임 경계선 값 모두 1로 세팅 
{
	int i, j;

	for (i = 0; i < 12; i++)
		gameblock[24][i] = 1;

	for (i = 0; i < 24; i++)
	{
		gameblock[i][0] = 1;

		for (j = 0; j < 10; j++)
			gameblock[i][j+1] = 0;

		gameblock[i][11] = 1;
	}
}

int block[7][4][4] = 		//블록 모양 모음 
{
	//I0	
	{
		{0, 0, 0, 0},
		{1, 1, 1, 1},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},

	//T1
	{
		{0, 0, 0, 0},
		{0, 1, 1, 1},
		{0, 0, 1, 0},
		{0, 0, 0, 0}
	},

	//O2
	{
		{0, 0, 0, 0},
		{0, 1, 1, 0},
		{0, 1, 1, 0},
		{0, 0, 0, 0}
	},

	//L3
	{
		{0, 0, 0, 0},
		{0, 1, 1, 1},
		{0, 1, 0, 0},
		{0, 0, 0, 0}
	},

	//J4
	{
		{0, 0, 0, 0},
		{0, 1, 1, 1},
		{0, 0, 0, 1},
		{0, 0, 0, 0}
	},

	//S5
	{
		{0, 0, 0, 0},
		{0, 0, 1, 1},
		{0, 1, 1, 0},
		{0, 0, 0, 0}
	},

	//Z6
	{
		{0, 0, 0, 0},
		{0, 1, 1, 0},
		{0, 0, 1, 1},
		{0, 0, 0, 0}
	}
};

int acblock[4][4];		//시간에서 시드값 끌어오기
int blockset[7];		//사용된 블록의 종류 
int blocksetprint[5][4][4];		//프린트 될 블록들 
int currentnum = 0;		//이번 세트에서 출력된 블록의 수 
int currentblock = 0;		//현재 블록 

int newblockrand(void)		//블록의 종류가 중첩되지 않게 랜덤생성 
{
	int i = 0;

	

	while (1)		//중복이 아닌 블록이 나올 때 까지 
	{
		if (currentnum != 0)		//첫번째 블록이 아닐 경우 
		{
			blockset[currentnum] = rand() % 7;

			for (i = 0; i < currentnum; i++)		//이번 세트에서 중복이 있는지 확인 
			{
				if (blockset[i] == blockset[currentnum])
					break;
			}

			if (i == currentnum)		//없으면 탈출 
				break;
		}

		else		//첫번째 블록일 경우 
		{
			blockset[currentnum] = rand() % 7;
			break;
		}
	}

	i= blockset[currentnum];
	currentnum++;
	if (currentnum == 7)
		currentnum = 0;

	return i;		//현재 순서의 블록 출력 후 currentnum+1 
}

int compareblock(int arr[4][4], int a)		//2차원 배열을 블록들의 첫번째 모양과 비교 
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (arr[i][j] != block[a][i][j])
				return 1;
		}
	}

	return 0;
}

int setcolor(int arr[4][4])		//블록의 모양에 따라 필요한 색 코드 반환 
{
	int i;

	for (i = 0; i < 7; i++)
	{
		if (compareblock(arr, i) == 0)
			break;
	}

	if (i == 0)		//I
		return 11;		//하늘색 

	else if (i == 1)		//T
		return 13;		//핑크색 

	else if (i == 2)		//O
		return 14;		//노란색 

	else if (i == 3)		//L
		return 4;		//다홍색 

	else if (i == 4)		//J
		return 9;		//파랑색 

	else if (i == 5)		//S
		return 10;		//연두색 

	else if (i == 6)		//Z
		return 12;		//빨간색 
}

void scrline(void)		//게임의 전체 태두리 출력  
{
	int i = 0;

	for (i = 0; i < 80; i++)		//첫 줄에 세로좌표 끝까지 그리기
		printf("◈");

	for (i = 0; i < 39; i++)		//두 번째 줄 부터 마지막에서 두 번째 줄 까지는 가장 왼쪽과 가장 오른쪽에만 그리기
	{
		gotoxy(0, i + 1);
		printf("◈");
		gotoxy(158, i);
		printf("◈");
	}

	gotoxy(0, 39);		//마지막 줄로 이동

	for (i = 0; i < 80; i++)		//마지막 줄의 세로좌표 끝까지 그리기
		printf("◈");

	return;
}

void topgameline(void)		//상단 경계선 
{
	int j;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);		//텍스트 색 흰색으로 지정 

	
	gotoxy(10, 9);

	for (j = 0; j < 12; j++)
		printf("--");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), currentblock);		//텍스트 색 다시 원래 블록 색으로 지정 
	return;
}

void gameline(void)		//게임이 이뤄지는 공간의 테두리 출력  
{
	int i = 0;

	for (i = 0; i < 21; i++)		//세로줄 출력 
	{
		gotoxy(10, i + 10);
		printf("◈");
		gotoxy(32, i + 10);
		printf("◈");
	}

	gotoxy(10, 31);

	for (i = 0; i < 12; i++)		//맨 밑 가로줄 출력 
		printf("◈");

	return;
}

void printsq(int arr[4][4])		//블록 출력 
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		gotoxy(x, y + i);

		if (y + i <= 9 && x < 40)
			continue;

		for (j = 0; j < 4; j++)
		{
			if (arr[i][j] == 1)		//블록이 있어야 할 공간이면 출력 
				printf("▣");

			else if (arr[i][j] == 0)		//아니라면 다음 칸으로 이동 
				gotoxy(x + 2 * (j + 1), y + i);
		}
	}
}

void rvprintsq(int arr[4][4])		//블록 잔상 지우기 
{
	int i, j;

	
	for (i = 0; i < 4; i++)
	{
		gotoxy(x, y + i);
		if (y + i <= 9 && x < 40)
			continue;

		for (j = 0; j < 4; j++)
		{
			if (arr[i][j] == 1)		//블록이 있던 칸이면 지우기 
				printf("  ");

			else if (arr[i][j] == 0)		//아니라면 이동 
				gotoxy(x + 2 * (j + 1), y + i);
		}
	}
}

void rvprintguide(int arr[4][4], int a, int b)		//가이드 블록 출력 
{
	int i, j;

	if (excurrent != currentnum)		//놓인 블록이 지워지지 않게
		return;

	for (i = 0; i < 4; i++)
	{
		gotoxy(a, b + i);

		for (j = 0; j < 4; j++)
		{
			if (arr[i][j] == 1)		//블록이 없어야 할 공간이면 출력 
				printf("  ");

			else if (arr[i][j] == 0)		//아니라면 다음 칸으로 이동 
				gotoxy(a + 2 * (j + 1), b + i);
		}
	}
}

void printguide(int arr[4][4], int a)		//가이드 블록 출력 
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		gotoxy(x, a + i);

		for (j = 0; j < 4; j++)
		{
			if (arr[i][j] == 1)		//블록이 있어야 할 공간이면 출력 
				printf("▣");

			else if (arr[i][j] == 0)		//아니라면 다음 칸으로 이동 
				gotoxy(x + 2 * (j + 1), a + i);
		}
	}
}

void blockprintwall(void)		//대기중인 블록 경계선 출력
{
	int i, j;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

	gotoxy(38, 6);
	printf("◈N E  X T◈");	

	for (i = 0; i < 5; i++)
	{
		gotoxy(38, 11 + i * 5);

		for (j = 0; j < 6; j++)
		{
			printf("◈");
		}
	}

	for (i = 0; i < 26; i++)
	{
		gotoxy(38, 6 + i);
		printf("◈");
		gotoxy(48, 6 + i);
		printf("◈");
	}
}

void blockprint(void)		//대기중인 블록 출력 
{
	x = 40;
	y = 7;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), setcolor(blocksetprint[0]));
	printsq(blocksetprint[0]);		//첫번째 대기 

	y = 12;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), setcolor(blocksetprint[1]));
	printsq(blocksetprint[1]);

	y = 17;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), setcolor(blocksetprint[2]));
	printsq(blocksetprint[2]);

	y = 22;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), setcolor(blocksetprint[3]));
	printsq(blocksetprint[3]);

	y = 27;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), setcolor(blocksetprint[4]));
	printsq(blocksetprint[4]);		//다섯번째 대기 
}

void rvblockprint(void)		//대기중인 잔상 제거 
{
	x = 40;
	y = 7;
	rvprintsq(blocksetprint[0]);		//첫번째 대기 

	y = 12;
	rvprintsq(blocksetprint[1]);

	y = 17;
	rvprintsq(blocksetprint[2]);

	y = 22;
	rvprintsq(blocksetprint[3]);

	y = 27;
	rvprintsq(blocksetprint[4]);		//다섯번째 대기 
}

int guidecheckdown(void)		//가이드 블록 최대 하강 좌표 계산
{
	int i, j;

	for (i = 3; i >= 0; i--)
	{
		for (j = 0; j < 4; j++)
		{
			if (acblock[i][j] != 0 && gameblock[exy - 7 + 1 + i][(x - 10) / 2 + j] != 0)		//간섭시 1 반환 
				return 1;
		}
	}

	return 0;
}

void guide(void)		//가이드 블록 출력
{
	int i, j;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

	rvprintguide(exacblock, exx, exy);		//가이드 블록 잔상 제거

	exy = y;		

	while (guidecheckdown() == 0)		//가이드 블록이 내려갈 수 있는 최대 값까지 이동
		exy++;

	exx = x;		//exx값 갱신
	printguide(acblock, exy);		//가이드 블록 출력

	for (i = 0; i < 4; i++)		//현재 가이드 블록의 모양 저장
	{
		for (j = 0; j < 4; j++)
			exacblock[i][j] = acblock[i][j];
	}
	excurrent = currentnum;		//현재 가이드 블록의 코드 위치 저장
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), currentblock);		//텍스트 색 현재 블록으로 지정 
}

void changeac(int num)		//대기중인 블록 한칸씩 앞으로 당기기 
{
	int i, j;

	rvblockprint();		//대기중인 블록 잔상 제거 
	blockprintwall();	//대기중인 블록 경계선 출력	

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			acblock[i][j] = blocksetprint[0][i][j];
			blocksetprint[0][i][j] = blocksetprint[1][i][j];
			blocksetprint[1][i][j] = blocksetprint[2][i][j];
			blocksetprint[2][i][j] = blocksetprint[3][i][j];
			blocksetprint[3][i][j] = blocksetprint[4][i][j];
			blocksetprint[4][i][j] = block[num][i][j];
		}
	}

	blockprint();		//대기중인 블록 출력 
}

void setgameblock(void)		//블록 고정 
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (acblock[i][j] == 1)
				gameblock[yset() + i][(x - 10) / 2 + j] = currentblock;		//현재 블록의 코드값으로 저장
		}
	}
}

int blockcheckmoveL(void)		//좌로 이동시 간섭체크 
{
	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (acblock[j][i] != 0 && gameblock[yset() + j][(x - 10) / 2 - 1 + i] != 0)		//간섭시 1 반환 
				return 1;
		}
	}

	return 0;
}

int blockcheckmoveR(void)		//우로 이동시 간섭체크 
{
	int i, j;

	for (i = 3; i >= 0; i--)
	{
		for (j = 0; j < 4; j++)
		{
			if (acblock[j][i] != 0 && gameblock[yset() + j][(x - 10) / 2 + 1 + i] != 0)		//간섭시 1 반환 
				return 1;
		}
	}

	return 0;
}

int blockcheckdown(void)		//하강시 간섭체크 
{
	int i, j;

	for (i = 3; i >= 0; i--)
	{
		for (j = 0; j < 4; j++)
		{
			if (acblock[i][j] != 0 && gameblock[yset() + 1 + i][(x - 10) / 2 + j] != 0)		//간섭시 1 반환 
				return 1;
		}
	}

	return 0;
}

int blockcheckturnL(void)		//회전시 좌측 간섭체크 
{
	int j;

	for (j = 0; j < 3; j++)
	{
		if (acblock[j][1] != 0 && gameblock[yset() + j][(x - 10) / 2 + 1] != 0)
		{
			if (j == 0)
				return 1;

			else if (j == 1)
				return 1;

			else if (j == 2)
				return 1;
		}
	}
	return 0;
}

int blockcheckturnR(void)		//회전시 우측 간섭체크 
{
	int j;

	for (j = 0; j < 3; j++)
	{
		if (acblock[j][3] != 0 && gameblock[yset() + j][(x - 10) / 2 + 3] != 0)
		{
			if (j == 0)
				return 2;

			else if (j == 1)
				return 2;

			else if (j == 2)
				return 2;
		}
	}
	return 0;
}

int blockcheckturnU(void)		//회전시 상단 간섭체크 
{
	int i;

	for (i = 1; i < 4; i++)
	{
		if (acblock[0][i] != 0 && gameblock[yset()][(x - 10) / 2 + i] != 0)
		{
			if (i == 1)
				return 3;

			else if (i == 2)
				return 3;

			else if (i == 3)
				return 3;
		}
	}
	return 0;
}

int blockcheckturnD(void)		//회전시 하단 간섭체크 
{
	int i;

	for (i = 1; i < 4; i++)
	{
		if (acblock[2][i] != 0 && gameblock[yset() + 2][(x - 10) / 2 + i] != 0)
		{
			if (i == 1)
				return 4;

			else if (i == 2)
				return 4;

			else if (i == 3)
				return 4;
		}
	}
	return 0;
}

int blockcheckturn(void)		//회전시 간섭체크 
{
	if (acblock[3][2] != 0 && gameblock[yset() + 3][(x - 10) / 2 + 2] != 0)		//I자블록의 (2, 3) 좌표 간섭 체크 
	{
		y -= 1;

		if (blockcheckturnU() == 3)		//이동한 좌표가 위쪽 간섭 시 이동한 좌표 초기화, 회전 불가 신호 
		{
			y += 1;
			return 1;
		}

		else
		{
			if (blockcheckturnD() == 4)		//이동한 좌표가 아랫쪽 간섭 시 한 칸 더 이동 
			{
				y -= 1;

				if (blockcheckturnU() == 3)		//다시 이동한 좌표가 위쪽 간섭시 이동한 좌표 초기화, 회전 불가 신호  
				{
					y += 2;
					return 1;
				}

				else
					return 0;
			}

			else
				return 0;
		}
	}

	else if ((acblock[1][0] != 0 && gameblock[yset() + 1][(x - 10) / 2] != 0) && x >= 10)		//I자블록의 (0, 1)간섭 체크 
	{
		x += 2;

		if (blockcheckturnR() == 2)		//이동한 좌표가 오른쪽 간섭시 이동한 좌표 초기화, 회전 불가 신호 
		{
			x -= 2;
			return 1;
		}

		else
			return 0;
	}

	else if (blockcheckturnD() == 4)		//하단 간섭 체크 
	{
		if (acblock[3][2] == 1)		//I자 블록 간섭시 
		{
			y -= 2;

			if (blockcheckturnU() == 3 || (acblock[1][2] == 1 && gameblock[yset() + 1][(x - 10) / 2 + 2] == 1))		//위쪽, 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 2;
				return 1;
			}

			else
				return 0;
		}

		y -= 1;

		if (blockcheckturnU() != 3)		//이동한 좌표 상단 간섭 안 할 때  
		{
			if (blockcheckturnL() == 1)		//좌측 간섭체크, 간섭시 좌표이동 
			{
				x += 2;

				if (blockcheckturnD() == 4)		//이동한 좌표 하단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x -= 2;
					y += 1;
					return 1;
				}


				else if (blockcheckturnL() == 1)		//이동한 좌표 좌측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x -= 2;
					y += 1;
					return 1;
				}

				else if (blockcheckturnR() == 2)		//이동한 좌표 우측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x -= 2;
					y += 1;
					return 1;
				}

				else if (blockcheckturnU() == 3)		//이동한 좌표 상단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x -= 2;
					y += 1;
					return 1;
				}

				else if (acblock[1][2] == 1 && gameblock[yset() + 1][(x - 10) / 2 + 2] == 1)		//이동한 좌표 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x -= 2;
					y += 1;
					return 1;
				}

				else
					return 0;
			}

			else if (blockcheckturnR() == 2)		//오른쪽 간섭 체크, 간섭시 좌표이동 
			{
				x -= 2;

				if (blockcheckturnD() == 4)				//이동한 좌표 하단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x += 2;
					y += 1;
					return 1;
				}


				else if (blockcheckturnL() == 1)		//이동한 좌표 좌측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x += 2;
					y += 1;
					return 1;
				}

				else if (blockcheckturnR() == 2)		//이동한 좌표 우측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x += 2;
					y += 1;
					return 1;
				}

				else if (blockcheckturnU() == 3)		//이동한 좌표 상단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x += 2;
					y += 1;
					return 1;
				}

				else if (acblock[1][2] == 1 && gameblock[yset() + 1][(x - 10) / 2 + 2] == 1)		//이동한 좌표 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x += 2;
					y += 1;
					return 1;
				}

				else
					return 0;
			}
		}

		else		//상단 간섭시 이동한 좌표 초기화, 회전 불가 신호 
		{
			y += 1;
			return 1;
		}
	}

	else if (blockcheckturnL() == 1)		//좌측 간섭 체크 후 간섭시 좌표이동 
	{
		if (acblock[1][0] == 1)		//I자 블록 간섭일 시 좌표이동 
		{
			x += 4;

			if (blockcheckturnR() == 2 || (acblock[1][2] == 1 && gameblock[yset() + 1][(x - 10) / 2 + 2] == 1))		//이동한 좌표 우측 간섭 체크, 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				x -= 4;
				return 1;
			}

			else
				return 0;
		}

		x += 2;

		if (blockcheckturnR() == 2)		//이동한 좌표 우측 간섭 체크, 간섭시 좌표 이동 
		{
			x -= 2;
			y -= 1;

			if (blockcheckturnD() == 4)		//이동한 좌표 하단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}


			else if (blockcheckturnL() == 1)		//이동한 좌표 좌측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}

			else if (blockcheckturnR() == 2)		//이동한 좌표 우측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}

			else if (blockcheckturnU() == 3)		//이동한 좌표 상단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}

			else if (acblock[1][2] != 0 && gameblock[yset() + 1][(x - 10) / 2 + 2] != 0)		//이동한 좌표 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}

			else
				return 0;
		}

		else if (acblock[1][2] != 0 && gameblock[yset() + 1][(x - 10) / 2 + 2] != 0)		//이동한 좌표 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
		{
			y += 1;
			return 1;
		}

		else
			return 0;
	}

	else if (blockcheckturnR() == 2)		//우측 간섭 체크, 간섭시 좌표이동 
	{
		x -= 2;

		if (blockcheckturnL() == 1)		//이동한 좌표 좌측 간섭체크, 간섭시 좌표이동 
		{
			x += 2;
			y -= 1;

			if (blockcheckturnD() == 4)		//이동한 좌표 하단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}


			else if (blockcheckturnL() == 1)		//이동한 좌표 좌측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}

			else if (blockcheckturnR() == 2)		//이동한 좌표 우측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}

			else if (blockcheckturnU() == 3)		//이동한 좌표 상단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}

			else if (acblock[1][2] != 0 && gameblock[yset() + 1][(x - 10) / 2 + 2] != 0)		//이동한 좌표 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
			{
				y += 1;
				return 1;
			}

			else
				return 0;
		}

		else if (acblock[1][2] != 0 && gameblock[yset() + 1][(x - 10) / 2 + 2] != 0)		//이동한 좌표 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
		{
			y += 1;
			return 1;
		}

		else
			return 0;
	}

	else if (blockcheckturnU() == 3)		//상단 간섭 체크, 간섭시 좌표이동 
	{
		y += 1;

		if (blockcheckturnD() != 4)		//이동한 좌표 하단 간섭 안 할 때 
		{
			if (blockcheckturnL() == 1)		//이동한 좌표 좌측 간섭 체크, 간섭시 좌표이동 
			{
				x += 2;

				if (blockcheckturnD() == 4)		//이동한 좌표 하단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x -= 2;
					y -= 1;
					return 1;
				}


				else if (blockcheckturnL() == 1)		//이동한 좌표 좌측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x -= 2;
					y -= 1;
					return 1;
				}

				else if (blockcheckturnR() == 2)		//이동한 좌표 우측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x -= 2;
					y -= 1;
					return 1;
				}

				else if (blockcheckturnU() == 3)		//이동한 좌표 상단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x -= 2;
					y -= 1;
					return 1;
				}

				else if (acblock[1][2] != 0 && gameblock[yset() + 1][(x - 10) / 2 + 2] != 0)		//이동한 좌표 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					y += 1;
					return 1;
				}

				else
					return 0;
			}

			if (blockcheckturnR() == 2)		//이동한 좌표 우측 간섭체크, 간섭시 좌표이동 
			{
				x -= 2;

				if (blockcheckturnD() == 4)		//이동한 좌표 하단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x += 2;
					y -= 1;
					return 1;
				}


				else if (blockcheckturnL() == 1)		//이동한 좌표 좌측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x += 2;
					y -= 1;
					return 1;
				}

				else if (blockcheckturnR() == 2)		//이동한 좌표 우측 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x += 2;
					y -= 1;
					return 1;
				}

				else if (blockcheckturnU() == 3)		//이동한 좌표 상단 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					x += 2;
					y -= 1;
					return 1;
				}

				else if (acblock[1][2] != 0 && gameblock[yset() + 1][(x - 10) / 2 + 2] != 0)		//이동한 좌표 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
				{
					y += 1;
					return 1;
				}

				else
					return 0;
			}
		}

		else		//하단 간섭 시 이동한 좌표 초기화, 회전 불가 신호 
		{
			y -= 1;
			return 1;
		}
	}

	else if (acblock[1][2] != 0 && gameblock[yset() + 1][(x - 10) / 2 + 2] != 0)		//이동한 좌표 중앙 간섭 체크, 간섭시 이동한 좌표 초기화, 회전 불가 신호 
	{
		y += 1;
		return 1;
	}

	else
		return 0;

}

void calcturnL(void)		//반시계방향 회전 연산 
{
	int n = 0;

	n = acblock[2][3];		//회전축 기준 꼭짓점 회전 
	acblock[2][3] = acblock[2][1];
	acblock[2][1] = acblock[0][1];
	acblock[0][1] = acblock[0][3];
	acblock[0][3] = n;

	n = acblock[1][3];		//회전축 기준 모서리 회전 
	acblock[1][3] = acblock[2][2];
	acblock[2][2] = acblock[1][1];
	acblock[1][1] = acblock[0][2];
	acblock[0][2] = n;

	n = acblock[1][0];		//일자블록만 포함되는 부분 회전 
	acblock[1][0] = acblock[3][2];
	acblock[3][2] = n;
}

void calcturnR(void)		//시계방향 회전 연산 
{
	int n = 0;

	n = acblock[0][3];		//회전축 기준 꼭짓점 회전 
	acblock[0][3] = acblock[0][1];
	acblock[0][1] = acblock[2][1];
	acblock[2][1] = acblock[2][3];
	acblock[2][3] = n;

	n = acblock[0][2];		//회전축 기준 모서리 회전 
	acblock[0][2] = acblock[1][1];
	acblock[1][1] = acblock[2][2];
	acblock[2][2] = acblock[1][3];
	acblock[1][3] = n;

	n = acblock[1][0];		//일자블록만 포함되는 부분 회전 
	acblock[1][0] = acblock[3][2];
	acblock[3][2] = n;
}

void turnL(void)		//반시계방향 회전 
{
	if (acblock[1][2] == 1 && acblock[2][1] == 1 && acblock[2][2] == 1 && acblock[1][1] == 1)		//정사각형 블록일시 생략 
		return;

	rvprintsq(acblock);		//잔상제거 

	calcturnL();		//회전연산 

	if (blockcheckturn() == 1)		//회전 불가 신호시 다시 원상복구  
		calcturnR();

	guide();		//가이드 블록 출력
	printsq(acblock);		//블록 출력 
}

void turnR(void)		//시계방향 회전 
{
	if (acblock[1][2] == 1 && acblock[2][1] == 1 && acblock[2][2] == 1 && acblock[1][1] == 1)		//정사각형 블록일시 생략 
		return;


	rvprintsq(acblock);		//잔상제거 

	calcturnR();		//회전연산 

	if (blockcheckturn() == 1)		//회전 불가 신호시 다시 원상복구  
		calcturnL();

	guide();		//가이드 블록 출력
	printsq(acblock);		//블록 출력 
}

void moveL(void)		//좌로 이동 
{
	if (blockcheckmoveL() != 1)		//간섭 없을 시 
	{
		rvprintsq(acblock);		//잔상 제거 
		x -= 2;		//좌표 이동 
		guide();		//가이드 블록 출력
		printsq(acblock);		//블록 출력 
	}
}

void moveR(void)		//우로 이동 
{
	if (blockcheckmoveR() != 1)		//간섭 없을 시 
	{
		rvprintsq(acblock);		//잔상 제거
		
		x += 2;		//좌표 이동 
		guide();		//가이드 블록 출력
		printsq(acblock);		//블록 출력 
	}
}

int moveD(void)		//하단으로 이동 
{
	if (blockcheckdown() != 1)		//간섭 없을 시
	{
		rvprintsq(acblock);		//잔상 제거
		y += 1;		//하단으로 이동
		printsq(acblock);		//블록 출력  
		score += 1;		//점수 1점 추가
		return 0;
	}

	else        //간섭 시
		return 1;
}

void godown(int a)		//하드드랍 
{
	rvprintsq(acblock);
	while (blockcheckdown() == 0)		//하단이동이 방해받을 때 까지 하단으로 이동
	{
		y++;
		if(a==0)
			score += 2;		//강제 하드드랍이 아닐 시 이동한 y좌표당 점수 2점
	}
		
	printsq(acblock);
	return;
}

int kb(void)		//키보드 입력시 
{
	int k = 0;

	k = _getch();

	if (k == 224)
	{
		k = _getch();

		if (k == 75)		//왼쪽 방향키 
		{
			moveL();
			return 2;
		}


		else if (k == 77)		//오른쪽 방향키 
		{
			moveR();
			return 2;
		}

		else if (k == 72)		//위쪽 방향키 
		{
			turnR();
			return 2;
		}

		else if (k == 80)		//아래쪽 방향키 
		{
			if (moveD() == 1)
				return 1;

			else
				return 3;		//1초 자동하강 대기시간 초기화 하라고 신호 
		}
		
		else
			return 0;
	}

	else if (k == 115)		//키보드 S 
	{
		turnL();
		return 2;
	}

	else if (k == 100)		//키보드 D 
	{
		turnR();
		return 2;
	}
	else if (k == 32)		//스페이스 바
	{
		godown(0);
		gotoxy(10, 32);
		return 1;
	}
	else
		return 0;
}

void moveac(int a)		//활성화된 블록의 좌표의 이동 
{
	int i;
	x = 18;		//블록이 출력되기 시작할 x좌표
	y = 7;		//블록이 출력되기 시작할  y좌표
	int buff = 0;
	clock_t start = 0;
	clock_t end=0;
	float duration = 0;

	topgameline();		//상단의 블록 경계선 출력
	guide();		//가이드 블록 출력
	start =NULL;
	while (1)	// 무한반복
	{
		for (i = 0; i < a; i++)	//a*10 밀리세컨드 = 블록이 한칸씩 내려오는 시간
		{
			if (start != NULL)		//땅에 닿고 1초 카운트가 진행중 일 때
			{
				end = clock();		//현재시각
				duration = (float)(end - start);		//카운트 계산

				if (duration >= 1000)		//1초 경과시
				{
					godown(1);		//강제 하드드랍 후 고정
					return;		
				}
			}

			if (_kbhit())	//키보드 입력이 있다면
			{
				buff = kb();

				if (buff == 3)		//방해받지 않은 하단 이동
				{
					if (start != NULL)		//땅에 닿고 1초 카운트가 진행중 일 때
					{
						start = NULL;		//카운트 초기화
						end = NULL;
					}
						
					i = 0;        //밑으로 이동할 때 지연시간 초기화(0.1초 지났을 때 밑으로 하강시 다시 0.3초를 기다려야 1칸 하강)
				}

				else if (buff == 1)		//방해받은 하단 이동시 고정
					return;
			}
			Sleep(10);
		}

		buff = moveD();		//a*10밀리세컨드 마다 한 칸씩 아래로 이동

		if (buff == 1)		//방해받은 이동일 때
		{
			if(start == NULL)		//카운트 시작
				start = clock();
		}
		else        //방해받지 않은 이동일 때
		{
			if (start != NULL)		//카운트 초기화
			{
				start = NULL();
				end = NULL();
			}
		}
	}
}

void printgameblock(void)		//게임화면 갱신 
{
	int i, j;

	for (i = 4; i < 24; i++)
	{
		gotoxy(12, 7 + i);
		for (j = 1; j < 11; j++)
		{
			if (gameblock[i][j] == 0)
				printf("  ");

			else if (gameblock[i][j] == 11)		//I
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), gameblock[i][j]);
				printf("▣");
			}

			else if (gameblock[i][j] == 13)		//T
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), gameblock[i][j]);
				printf("▣");
			}

			else if (gameblock[i][j] == 14)		//O
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), gameblock[i][j]);
				printf("▣");
			}

			else if (gameblock[i][j] == 4)		//L
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), gameblock[i][j]);
				printf("▣");
			}

			else if (gameblock[i][j] == 9)		//J
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), gameblock[i][j]);
				printf("▣");
			}

			else if (gameblock[i][j] == 10)		//S
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), gameblock[i][j]);
				printf("▣");
			}

			else if (gameblock[i][j] == 12)		//Z
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), gameblock[i][j]);
				printf("▣");
			}
		}
	}
}

void finishedgameblock(int a)		//줄이 완성되었을 때 
{
	int i, j, k=0;

	for (i = a; i > 0; i--)
	{
		for (j = 1; j < 11; j++)
		{
			gameblock[i][j] = gameblock[i - 1][j];		//완성된 줄의 윗줄부터 하나씩 아랫줄로 이동 
			k++;
		}
		printgameblock();		//게임 화면 갱신
	}
	if(k == 0)
		return;
	else if (k == 1)		//한 줄 클리어시 100점
		score += 100;
	else if (k == 2)		//더블 시 300점
		score += 300;
	else if (k == 3)		//트리플 시 500점
		score += 500;
	else                    //테트리스 시 800점
		score += 800;
}

int scangameover(void)		//게임오버됐는지 체크 
{
	int i = 0;

	for (i = 1; i < 11; i++)
	{
		if (gameblock[3][i] != 0)		//오버됐을시 1 반환
			return 1;
	}
}

void scangameblock(void)		//완성된 줄이 있는지 검사 
{
	int i, j;

	for (i = 23; i > 3; i--)
	{
		for (j = 1; j < 11; j++)
		{
			if (gameblock[i][j] == 0)		//완성이 되지 않은 부분이 있다면 다음 줄로 
				break;
		}

		if (j == 11)		//완성이 되었다면
		{
			playsoundline();		//줄 제거 효과음 재생
			finishedgameblock(i++);		// 한 줄씩 당기기 
		}
	}
	return;
}

void printT(int x, int y)		//T 출력
{
	gotoxy(x, y);   printf("▣▣▣▣▣▣");
	gotoxy(x, y+1); printf("    ▣▣    ");
	gotoxy(x, y+2); printf("    ▣▣    ");
	gotoxy(x, y+3); printf("    ▣▣    ");
	gotoxy(x, y+4); printf("    ▣▣    ");
	gotoxy(x, y+5); printf("    ▣▣    ");
	gotoxy(x, y+6); printf("    ▣▣    ");
	gotoxy(x, y+7); printf("    ▣▣    ");
	gotoxy(x, y+8); printf("    ▣▣    ");
}

void printE(int x, int y)		//E 출력
{
	gotoxy(x, y);     printf("▣▣▣▣▣▣");
	gotoxy(x, y + 1); printf("▣▣        ");
	gotoxy(x, y + 2); printf("▣▣        ");
	gotoxy(x, y + 3); printf("▣▣        ");
	gotoxy(x, y + 4); printf("▣▣▣▣▣  ");
	gotoxy(x, y + 5); printf("▣▣        ");
	gotoxy(x, y + 6); printf("▣▣        ");
	gotoxy(x, y + 7); printf("▣▣        ");
	gotoxy(x, y + 8); printf("▣▣▣▣▣▣");
}

void printR(int x, int y)		//R 출력
{
	gotoxy(x, y);     printf("▣▣▣▣    ");
	gotoxy(x, y + 1); printf("▣▣  ▣▣  ");
	gotoxy(x, y + 2); printf("▣▣   ▣▣ ");
	gotoxy(x, y + 3); printf("▣▣  ▣▣  ");
	gotoxy(x, y + 4); printf("▣▣▣▣    ");
	gotoxy(x, y + 5); printf("▣▣ ▣▣   ");
	gotoxy(x, y + 6); printf("▣▣  ▣▣  ");
	gotoxy(x, y + 7); printf("▣▣   ▣▣ ");
	gotoxy(x, y + 8); printf("▣▣    ▣▣");
}

void printI(int x, int y)		//I 출력
{
	gotoxy(x, y);     printf("▣▣▣▣▣▣");
	gotoxy(x, y + 1); printf("    ▣▣    ");
	gotoxy(x, y + 2); printf("    ▣▣    ");
	gotoxy(x, y + 3); printf("    ▣▣    ");
	gotoxy(x, y + 4); printf("    ▣▣    ");
	gotoxy(x, y + 5); printf("    ▣▣    ");
	gotoxy(x, y + 6); printf("    ▣▣    ");
	gotoxy(x, y + 7); printf("    ▣▣    ");
	gotoxy(x, y + 8); printf("▣▣▣▣▣▣");
}

void printS(int x, int y)		//S 출력
{
	gotoxy(x, y);     printf("  ▣▣▣▣▣");
	gotoxy(x, y + 1); printf(" ▣▣▣▣   ");
	gotoxy(x, y + 2); printf("▣▣▣      ");
	gotoxy(x, y + 3); printf(" ▣▣▣     ");
	gotoxy(x, y + 4); printf("   ▣▣▣   ");
	gotoxy(x, y + 5); printf("     ▣▣▣ ");
	gotoxy(x, y + 6); printf("      ▣▣▣");
	gotoxy(x, y + 7); printf("   ▣▣▣▣ ");
	gotoxy(x, y + 8); printf("▣▣▣▣▣  ");
}

int firstSC(void)		//선택화면 출력
{
	int input = 0;
	int CursorS = 26;
	int num = 0;
	int i = 0;

	gotoxy(74, 26);
	printf("Beginner");
	gotoxy(74, 28);
	printf("Amateur");
	gotoxy(74, 30);
	printf("Expert");
	gotoxy(74, 32);
	printf("Exit");
	gotoxy(71, 26);
	printf(">>");

	while (1)
	{
		for (i = 0; i < 10; i++)
		{
			if (kbhit())
			{
				int input = getch();

				if (input == 13)		//엔터키 입력
				{
					if (CursorS == 26)
					{
						PlaySound(TEXT("start.wav"), 0, SND_ASYNC);		//선택 효과음

						for (i = 0; i < 5; i++)		//선택시 반짝거림 
						{
							gotoxy(74, 26);
							printf("        ");
							Sleep(100);
							gotoxy(74, 26);
							printf("Beginner");
							Sleep(100);
						}
						system("cls");
						return 1;		//비기너 선택시 1반환
					}

					else if (CursorS == 28)
					{
						PlaySound(TEXT("start.wav"), 0, SND_ASYNC);

						for (i = 0; i < 5; i++)		//선택시 반짝거림 
						{
							gotoxy(74, 28);
							printf("       ");
							Sleep(100);
							gotoxy(74, 28);
							printf("Amateur");
							Sleep(100);
						}
						system("cls");
						return 2;		//아마추어 선택시 2반환
					}

					else if (CursorS == 30)
					{
						PlaySound(TEXT("start.wav"), 0, SND_ASYNC);

						for (i = 0; i < 5; i++)		//선택시 반짝거림 
						{
							gotoxy(74, 30);
							printf("      ");
							Sleep(100);
							gotoxy(74, 30);
							printf("Expert");
							Sleep(100);
						}
						system("cls");
						return 3;		//엑스퍼트 선택시 3반환
					}

					else        //종료 선택시 0반환
					{
						return 0;
					}
				}

				else if (input == 224)
				{
					input = getch();

					switch (input)
					{
					case U:		//방향키 위
					{
						gotoxy(71, CursorS);
						printf("  ");
						if (CursorS != 26)
							CursorS = CursorS - 2;
						else
							CursorS = 32;
						break;
					}

					case D:		//방향키 아래
					{
						gotoxy(71, CursorS);
						printf("  ");
						if (CursorS != 32)
							CursorS = CursorS + 2;
						else
							CursorS = 26;
						break;
					}
					}
				}
			}
			Sleep(10);
		}

		if (num == 0)
		{
			gotoxy(71, CursorS);
			printf(">>");
			num = 1;
		}

		else
		{
			gotoxy(71, CursorS);
			printf("  ");
			num = 0;
		}
	}
}

int start(void)		//스타트 화면 
{
	srand(time(NULL));		//시간에서 시드값 끌어오기
	system("mode con cols=160 lines=40");		//콘솔창 크기 지정 
	cursorview();		//커서 숨기기 
	system("title Tetris by namaek");		//타이틀 명 변경 

	int i = 0;

	system("cls");		//화면 초기화 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);		//색상 지정
	printT(39, 6);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);		//알파벳 출력
	printE(53, 6);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	printT(67, 6);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	printR(81, 6);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	printI(95, 6);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
	printS(109, 6);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

	gotoxy(50, 34);
	printf("회전 : S, D, 방향키 상 / 이동 : 방향키 좌, 우 ,하 / 하드드랍 : 스페이스 바");
	gotoxy(117, 16);
	printf("made by namaek");

	return(firstSC());
}

void gameover(void)
{
	system("cls");		//화면 초기화 

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);		//텍스트 색 지정
	PlaySound(TEXT("gameover.wav"), 0, SND_ASYNC);		//게임오버 효과음 재생

	gotoxy(56, 6);      printf("  ▣▣▣      ▣▣▣    ▣▣      ▣▣  ▣▣▣▣");		//게임 오버 문구 출력
	gotoxy(56, 6 + 1);  printf("▣      ▣  ▣      ▣  ▣  ▣  ▣  ▣  ▣      ");
	gotoxy(56, 6 + 2);  printf("▣          ▣      ▣  ▣  ▣  ▣  ▣  ▣      ");
	gotoxy(56, 6 + 3);  printf("▣      ▣  ▣▣▣▣▣  ▣    ▣    ▣  ▣▣▣  ");
	gotoxy(56, 6 + 4);  printf("▣    ▣▣  ▣      ▣  ▣          ▣  ▣      ");
	gotoxy(56, 6 + 5);  printf("▣      ▣  ▣      ▣  ▣          ▣  ▣      ");
	gotoxy(56, 6 + 6);  printf("  ▣▣▣    ▣      ▣  ▣          ▣  ▣▣▣▣");
	gotoxy(56, 6 + 7);  printf("                                                ");
	gotoxy(56, 6 + 8);  printf("    ▣▣▣    ▣      ▣  ▣▣▣▣  ▣▣▣▣  ");
	gotoxy(56, 6 + 9);  printf("  ▣      ▣  ▣      ▣  ▣        ▣      ▣");
	gotoxy(56, 6 + 10); printf("  ▣      ▣  ▣      ▣  ▣        ▣      ▣");
	gotoxy(56, 6 + 11); printf("  ▣      ▣  ▣      ▣  ▣▣▣    ▣▣▣▣  ");
	gotoxy(56, 6 + 12); printf("  ▣      ▣  ▣      ▣  ▣        ▣      ▣");
	gotoxy(56, 6 + 13); printf("  ▣      ▣    ▣  ▣    ▣        ▣      ▣");
	gotoxy(56, 6 + 14); printf("    ▣▣▣        ▣      ▣▣▣▣  ▣      ▣");

	gotoxy(70, 25);
	printf("YOUR SCORE : %d", score);		//최종 점수 출력
	
	Sleep(2000);
	gotoxy(65, 30);
	printf("Press any key to go to menu");
	
	while (kbhit()) getch();		//버퍼 초기화
	getch();		//후 아무 키 입력시 메인화면으로 복귀
}

int main(void)
{
	int speed;
	int num;

	while (1)		//게임 종료시 메인화면으로 복귀
	{
		clock_t starttime = 0;
		clock_t endtime = 0;
		int speednum = 0;
		score = 0;

		num = start();
		if (num == 1)		//비기너
			speed = 30;

		else if (num == 2)		//아마추어
			speed = 20;

		else if (num == 3)		//엑스퍼트
			speed = 10;

		else        //종료
			return 0;
	
		MainSound(0);		//bgm 재생

		scrline();		//게임 경계선 출력
		gameline();		//테트리스가 이루어지는 경계선 출력
		gameblockset();		//쌓인블록 좌표 초기화
		
		changeac(newblockrand());		//처음 대기 블록 5개와 첫 활성화 될 블록 지정 
		changeac(newblockrand());
		changeac(newblockrand());
		changeac(newblockrand());
		changeac(newblockrand());
		changeac(newblockrand());

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);		//점수 색깔 지정 
		gotoxy(10, 7);
		printf("SCORE : %d", score);		//점수 전광판

		starttime = clock();		//게임을 시작할 때의 시간 기록
		while (1)
		{
			currentblock = setcolor(acblock);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), currentblock);		//블록 색깔 지정 
			moveac(speed);
			playsoundfall();
		
			setgameblock();
			scangameblock();

			gotoxy(10, 7);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);		//블록 색깔 지정 
			printf("SCORE : %d", score);
			
			endtime = clock();		//현재 시간 기록
			if (endtime-starttime>=60000 && speednum==0)		//게임 시작 1분째에 속도 변화
			{
				if (num == 1)
					speed -= 10;

				else if (num == 2)
					speed -= 7;

				else if (num == 3)
					speed -= 3;

				speednum++;
			}
			else if (endtime - starttime >= 120000 && speednum==1)		//게임 시작 2분째에 속도 변화
			{
				if (num == 1)
					speed -= 10;

				else if (num == 2)
					speed -= 7;

				else if (num == 3)
					speed -= 3;

				speednum++;
			}
			else if (endtime - starttime >= 180000 && speednum == 2)		//게임 시작 3분째에 속도 변화
			{
				if (num == 1)
					speed -= 7;

				else if (num == 2)
					speed -= 4;

				else if (num == 3)
					speed -= 3;

				speednum++;
			}

			if (scangameover() == 1)		//가장 윗줄에 블록이 있는지 확인
				break;

			changeac(newblockrand());
		}

		MainSound(1);		//bgm 정지
		gameover();		//게임오버 화면
	}
	
	Sleep(5000);
	return 0;
}