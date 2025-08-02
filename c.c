#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define SIZE 15
#define MAX_MOVES 225

// 记录每一步
typedef struct {
    int row;
    int col;
    char player; // 'X' or 'O'
} Move;

char board[SIZE][SIZE];
Move moveStack[MAX_MOVES];
int moveCount = 0;

// 初始化棋盘
void initBoard() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = '+';
    moveCount = 0;
}

// 打印棋盘
void printBoard() {
    printf("    ");
    for (int j = 0; j < SIZE; j++) printf("%2d ", j);
    printf("\n");

    for (int i = 0; i < SIZE; i++) {
        printf("%2d ", i);
        for (int j = 0; j < SIZE; j++)
            printf("%2c ", board[i][j]);
        printf("\n");
    }
}

// 判断是否合法落子
bool isValidMove(int row, int col) {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE && board[row][col] == '+';
}

// 判断是否胜利
bool checkWin(int row, int col, char player) {
    int directions[4][2] = {{0,1},{1,0},{1,1},{1,-1}};
    for (int d = 0; d < 4; d++) {
        int count = 1;
        for (int i = 1; i < 5; i++) {
            int r = row + i * directions[d][0];
            int c = col + i * directions[d][1];
            if (r < 0 || r >= SIZE || c < 0 || c >= SIZE || board[r][c] != player) break;
            count++;
        }
        for (int i = 1; i < 5; i++) {
            int r = row - i * directions[d][0];
            int c = col - i * directions[d][1];
            if (r < 0 || r >= SIZE || c < 0 || c >= SIZE || board[r][c] != player) break;
            count++;
        }
        if (count >= 5) return true;
    }
    return false;
}

// 检查是否平局
bool isBoardFull() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == '+') return false;
    return true;
}

// 撤销一步
bool undoMove() {
    if (moveCount == 0) return false;
    Move last = moveStack[--moveCount];
    board[last.row][last.col] = '+';
    return true;
}

// AI 评分函数
int scorePosition(int row, int col, char player) {
    int score = 0;
    char opponent = (player == 'X') ? 'O' : 'X';
    int directions[4][2] = {{0,1},{1,0},{1,1},{1,-1}};

    for (int d = 0; d < 4; d++) {
        int count_player = 1, count_opponent = 1;

        for (int i = 1; i < 5; i++) {
            int r = row + i * directions[d][0];
            int c = col + i * directions[d][1];
            if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) break;
            if (board[r][c] == player) count_player++;
            else break;
        }
        for (int i = 1; i < 5; i++) {
            int r = row - i * directions[d][0];
            int c = col - i * directions[d][1];
            if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) break;
            if (board[r][c] == player) count_player++;
            else break;
        }

        for (int i = 1; i < 5; i++) {
            int r = row + i * directions[d][0];
            int c = col + i * directions[d][1];
            if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) break;
            if (board[r][c] == opponent) count_opponent++;
            else break;
        }
        for (int i = 1; i < 5; i++) {
            int r = row - i * directions[d][0];
            int c = col - i * directions[d][1];
            if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) break;
            if (board[r][c] == opponent) count_opponent++;
            else break;
        }

        if (count_player >= 5) score += 10000;
        else if (count_player == 4) score += 1000;
        else if (count_player == 3) score += 100;
        else if (count_player == 2) score += 10;

        if (count_opponent >= 5) score += 9000;
        else if (count_opponent == 4) score += 900;
        else if (count_opponent == 3) score += 90;
        else if (count_opponent == 2) score += 9;
    }

    int center = SIZE / 2;
    score += 15 - (abs(center - row) + abs(center - col));
    return score;
}

// AI 决策
void computerMove(int* row, int* col, int difficulty) {
    int maxScore = -1;
    int bestRow = -1, bestCol = -1;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == '+') {
                int score = scorePosition(i, j, 'O');
                if (difficulty == 3)
                    score += (int)(scorePosition(i, j, 'X') * 1.2);

                if (score > maxScore) {
                    maxScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }

    *row = bestRow;
    *col = bestCol;
    printf("computer move: %d %d (score %d)\n", *row, *col, maxScore);
}

// 玩家 vs 玩家
void playPvP() {
    initBoard();
    char players[2] = {'X', 'O'};
    int currentPlayer = 0;

    while (true) {
        printBoard();
        printf("player %c's turn (or type 'r' to undo): ", players[currentPlayer]);

        char input[16];
        scanf("%s", input);

        if (strcmp(input, "r") == 0) {
            if (undoMove()) {
                currentPlayer = 1 - currentPlayer;
                printf("Undo successful.\n");
            } else {
                printf("Nothing to undo.\n");
            }
            continue;
        }

        int row = atoi(input);
        int col;
        if (scanf("%d", &col) != 1 || !isValidMove(row, col)) {
            printf("Invalid move.\n");
            while (getchar() != '\n');
            continue;
        }

        board[row][col] = players[currentPlayer];
        moveStack[moveCount++] = (Move){row, col, players[currentPlayer]};

        if (checkWin(row, col, players[currentPlayer])) {
            printBoard();
            printf("player %c won!\n", players[currentPlayer]);
            break;
        }
        if (isBoardFull()) {
            printBoard();
            printf("Draw! The board is full.\n");
            break;
        }

        currentPlayer = 1 - currentPlayer;
    }
}

// 玩家 vs 电脑
void playPvAI(int difficulty) {
    initBoard();

    while (true) {
        printBoard();
        printf("your turn (or type 'r' to undo): ");

        char input[16];
        scanf("%s", input);

        if (strcmp(input, "r") == 0) {
            if (moveCount >= 2) {
                undoMove(); // 撤销电脑
                undoMove(); // 撤销玩家
                printf("Undo successful.\n");
            } else {
                printf("Nothing to undo.\n");
            }
            continue;
        }

        int row = atoi(input), col;
        if (scanf("%d", &col) != 1 || !isValidMove(row, col)) {
            printf("Invalid move.\n");
            while (getchar() != '\n');
            continue;
        }

        board[row][col] = 'X';
        moveStack[moveCount++] = (Move){row, col, 'X'};

        if (checkWin(row, col, 'X')) {
            printBoard();
            printf("you win!\n");
            break;
        }
        if (isBoardFull()) {
            printBoard();
            printf("Draw! The board is full.\n");
            break;
        }

        // AI 落子
        if (difficulty == 1) {
            do {
                row = rand() % SIZE;
                col = rand() % SIZE;
            } while (!isValidMove(row, col));
            printf("computer move: %d %d\n", row, col);
        } else {
            computerMove(&row, &col, difficulty);
        }

        board[row][col] = 'O';
        moveStack[moveCount++] = (Move){row, col, 'O'};

        if (checkWin(row, col, 'O')) {
            printBoard();
            printf("you lose!\n");
            break;
        }
        if (isBoardFull()) {
            printBoard();
            printf("Draw! The board is full.\n");
            break;
        }
    }
}

// 主菜单
int main() {
    int mode, difficulty;
    srand(time(NULL));

    printf("=== Welcome to Gomoku ===\n");
    printf("Select mode:\n");
    printf("1. Player vs Player\n");
    printf("2. Player vs Computer\n");
    printf("Enter 1 or 2: ");

    if (scanf("%d", &mode) != 1 || (mode != 1 && mode != 2)) {
        printf("Invalid input. Exiting.\n");
        return 1;
    }

    if (mode == 1) {
        playPvP();
    } else {
        printf("Select AI difficulty:\n");
        printf("1. Easy (random)\n");
        printf("2. Medium (scoring)\n");
        printf("3. Hard (scoring + defense)\n");
        printf("Enter 1-3: ");
        if (scanf("%d", &difficulty) != 1 || difficulty < 1 || difficulty > 3) {
            printf("Invalid input. Defaulting to Medium.\n");
            difficulty = 2;
        }
        playPvAI(difficulty);
    }

    return 0;
}
