#include <SFML/Graphics.hpp>
#include <time.h>
#include "Connector.hpp"
#include <vector>
#include <algorithm>
#include <random>
using namespace sf;

int size = 56;
Vector2f offset(28, 28);

Sprite f[32]; //figures
std::string position = "";

int board[8][8] =
{ -1,-2,-3,-5,-4,-3,-2,-1,
 -6,-6,-6,-6,-6,-6,-6,-6,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  6, 6, 6, 6, 6, 6, 6, 6,
  1, 2, 3, 5, 4, 3, 2, 1 };

std::string toChessNote(Vector2f p)
{
	std::string s = "";
	s += char(p.x / size + 97);
	s += char(7 - p.y / size + 49);
	return s;
}

Vector2f toCoord(char a, char b)
{
	int x = int(a) - 97;
	int y = 7 - int(b) + 49;
	return Vector2f(x * size, y * size);
}

void move(std::string str)
{
	Vector2f oldPos = toCoord(str[0], str[1]);
	Vector2f newPos = toCoord(str[2], str[3]);

	for (int i = 0; i < 32; i++)
		if (f[i].getPosition() == newPos) f[i].setPosition(-100, -100);

	for (int i = 0; i < 32; i++)
		if (f[i].getPosition() == oldPos) f[i].setPosition(newPos);

	//castling       //if the king didn't move
	if (str == "e1g1") if (position.find("e1") == -1) move("h1f1");
	if (str == "e8g8") if (position.find("e8") == -1) move("h8f8");
	if (str == "e1c1") if (position.find("e1") == -1) move("a1d1");
	if (str == "e8c8") if (position.find("e8") == -1) move("a8d8");
}

void loadPosition()
{
	int k = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			int n = board[i][j];
			if (!n) continue;
			int x = abs(n) - 1;
			int y = n > 0 ? 1 : 0;
			f[k].setTextureRect(IntRect(size * x, size * y, size, size));
			f[k].setPosition(size * j, size * i);
			k++;
		}

	for (int i = 0; i < position.length(); i += 5) {
        move(position.substr(i, 4));
	}
}

//void shuffleNonPawns() {
//	std::vector<int> whitePieces;
//	std::vector<std::pair<int, int>> whitePositions;
//
//	std::vector<int> blackPieces;
//	std::vector<std::pair<int, int>> blackPositions;
//
//	// 收集非兵种棋子的位置和类型
//	for (int i = 0; i < 8; i++) {
//		for (int j = 0; j < 8; j++) {
//			int piece = board[i][j];
//			if (piece == 0 || abs(piece) == 6) continue; // 跳过空格和兵
//
//			if (piece > 0) {
//				whitePieces.push_back(piece);
//				whitePositions.push_back({ i, j });
//			}
//			else {
//				blackPieces.push_back(piece);
//				blackPositions.push_back({ i, j });
//			}
//		}
//	}
//
//	// 创建随机引擎
//	std::random_device rd;
//	std::mt19937 g(rd());
//
//	// 打乱位置
//	std::shuffle(whitePositions.begin(), whitePositions.end(), g);
//	std::shuffle(blackPositions.begin(), blackPositions.end(), g);
//
//	// 清除原来位置
//	for (auto& pos : whitePositions) board[pos.first][pos.second] = 0;
//	for (auto& pos : blackPositions) board[pos.first][pos.second] = 0;
//
//	// 随机重新放置
//	for (size_t i = 0; i < whitePieces.size(); ++i)
//		board[whitePositions[i].first][whitePositions[i].second] = whitePieces[i];
//
//	for (size_t i = 0; i < blackPieces.size(); ++i)
//		board[blackPositions[i].first][blackPositions[i].second] = blackPieces[i];
//}

void shuffleNonPawns() {
	std::vector<int> whitePieces;
	std::vector<int> whiteColumns;

	// 收集白方第7行的非兵棋子及其列号
	for (int j = 0; j < 8; ++j) {
		int piece = board[7][j];
		if (piece != 0 && abs(piece) != 6) {
			whitePieces.push_back(piece);
			whiteColumns.push_back(j);
			board[7][j] = 0; // 清空原位置
		}
	}

	// 清空黑方第0行的非兵
	for (int j = 0; j < 8; ++j) {
		if (abs(board[0][j]) != 6)
			board[0][j] = 0;
	}

	// 创建随机引擎并打乱列顺序
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(whiteColumns.begin(), whiteColumns.end(), g);

	// 将白方棋子按打乱列放置，并在黑方对应列放置对称棋子
	for (size_t i = 0; i < whitePieces.size(); ++i) {
		int col = whiteColumns[i];
		board[7][col] = whitePieces[i];         // 白方
		board[0][col] = -whitePieces[i];        // 黑方对称（纵向）
	}
}

bool isValidMove(Vector2f oldPos, Vector2f newPos, int board[8][8], bool isWhiteTurn) {
	int x1 = oldPos.x / size;
	int y1 = oldPos.y / size;
	int x2 = newPos.x / size;
	int y2 = newPos.y / size;

	int piece = board[y1][x1];
	int target = board[y2][x2];

	// 判断是否是当前轮次的棋子（白方走正数，黑方走负数）
	if ((isWhiteTurn && piece < 0) || (!isWhiteTurn && piece > 0))
		return false;

	// 防止走出边界
	if (x2 < 0 || x2 > 7 || y2 < 0 || y2 > 7)
		return false;

	// 兵的移动逻辑
	if (abs(piece) == 6) {
		int dir = (piece > 0) ? -1 : 1; // 白兵向上，黑兵向下
		int startRow = (piece > 0) ? 6 : 1;

		// 单步前进
		if (x1 == x2 && y2 - y1 == dir && target == 0)
			return true;
        
		// 吃子（斜走）
		else if (abs(x2 - x1) == 1 && y2 - y1 == dir && target != 0 && (piece * target < 0)) {
            return true;
		}

		// 双步前进（第一步）
		if (x1 == x2 && y1 == startRow && y2 - y1 == 2 * dir && target == 0) {
			int midY = y1 + dir;
			if (board[midY][x1] == 0) return true;
		}

		return false;
	}

	// 马的移动逻辑
    if (abs(piece) == 2) {  // 马
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);

        // 判断L形（走 2 格 1 格 或 1 格 2 格）
        if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
            // 确保目标位置没有自己的一方棋子
            if (target * piece > 0)
                return false;
            return true;
        }
        return false;
    }

	// 车的移动逻辑
	if (abs(piece) == 1) {  // 车
		// 车只能沿水平或垂直方向移动
		if (x1 == x2) {  // 垂直方向
			int step = (y2 > y1) ? 1 : -1;
			for (int y = y1 + step; y != y2; y += step) {
				if (board[y][x1] != 0)  // 途中有棋子阻挡
					return false;
			}
			// 确保目标位置是空的或是敌方棋子
			if (target * piece > 0)
				return false;
			return true;
		}

		if (y1 == y2) {  // 水平方向
			int step = (x2 > x1) ? 1 : -1;
			for (int x = x1 + step; x != x2; x += step) {
				if (board[y1][x] != 0)  // 途中有棋子阻挡
					return false;
			}
			// 确保目标位置是空的或是敌方棋子
			if (target * piece > 0)
				return false;
			return true;
		}
		return false;  // 不是水平或垂直方向的移动
	}

	// 象的移动逻辑
	if (abs(piece) == 3) {  // 象
		// 象只能沿对角线移动
		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);

		if (dx == dy) {  // 对角线移动
			int stepX = (x2 > x1) ? 1 : -1;
			int stepY = (y2 > y1) ? 1 : -1;

			// 检查途中是否有棋子阻挡
			int x = x1 + stepX;
			int y = y1 + stepY;
			while (x != x2 && y != y2) {
				if (board[y][x] != 0)  // 途中有棋子阻挡
					return false;
				x += stepX;
				y += stepY;
			}

			// 确保目标位置是空的或是敌方棋子
			if (target * piece > 0)
				return false;
			return true;
		}
		return false;  // 不是沿对角线的移动
	}

	// 王的移动逻辑
	if (abs(piece) == 4) {
		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);

		// 王只能走一格
		if ((dx <= 1 && dy <= 1) && (dx + dy != 0)) {
			// 确保目标不是己方单位
			if (target * piece > 0)
				return false;

			return true;
		}

		return false;
	}

	// 后（皇后）的移动逻辑
	if (abs(piece) == 5) {
		int dx = abs(x2 - x1);
		int dy = abs(y2 - y1);

		// 横、竖 或 对角线方向
		if ((x1 == x2) || (y1 == y2) || (dx == dy)) {
			int stepX = (x2 == x1) ? 0 : ((x2 > x1) ? 1 : -1);
			int stepY = (y2 == y1) ? 0 : ((y2 > y1) ? 1 : -1);

			int x = x1 + stepX;
			int y = y1 + stepY;

			while (x != x2 || y != y2) {
				if (board[y][x] != 0)
					return false; // 路上有阻挡
				x += stepX;
				y += stepY;
			}

			// 确保目标不是己方单位
			if (target * piece > 0)
				return false;

			return true;
		}

		return false;
	}

	// TODO：添加其他棋子的判断逻辑
	return true; // 默认允许其他类型
}

int checkWinner() {
	bool whiteKingAlive = false;
	bool blackKingAlive = false;

	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j) {
			if (board[i][j] == 4) whiteKingAlive = true;
			if (board[i][j] == -4) blackKingAlive = true;
		}

	if (!whiteKingAlive) return -1; // 黑胜
	if (!blackKingAlive) return 1;  // 白胜
	return 0; // 尚未分胜负
}

void resetBoard() {
	int initialBoard[8][8] =
	{ -1,-2,-3,-5,-4,-3,-2,-1,
      -6,-6,-6,-6,-6,-6,-6,-6,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      6, 6, 6, 6, 6, 6, 6, 6,
      1, 2, 3, 5, 4, 3, 2, 1 };

	for (int i = 0; i < 8; ++i)
		for (int j = 0; j < 8; ++j)
			board[i][j] = initialBoard[i][j];
}

int main(){

	bool isWhiteTurn = true;
	int mode = 0;


	RenderWindow window(VideoMode(704, 504), "The Chess!");

	char enginePath[] = "stockfish.exe";
	ConnectToEngine(enginePath);

	Texture t1, t2, t3;
	Texture white_turn, black_turn,white_win,black_win,reset_board,change_mode;
	Texture mode_classic,mode_random;
	t1.loadFromFile("images/figures.png");
	t2.loadFromFile("images/board.png");
	t3.loadFromFile("images/board_2.png");
	white_turn.loadFromFile("images/white.png");
	black_turn.loadFromFile("images/black.png");
	white_win.loadFromFile("images/white_win.png");
	black_win.loadFromFile("images/black_win.png");
	reset_board.loadFromFile("images/reset_board.png");
	change_mode.loadFromFile("images/change_mode.png");
	mode_classic.loadFromFile("images/mode_classic.png");
	mode_random.loadFromFile("images/mode_random.png");

	for (int i = 0; i < 32; i++) f[i].setTexture(t1);
	Sprite sBoard(t2);
	Sprite sBoard2(t3);
	Sprite sBlackTurn(black_turn);
	Sprite sWhiteTurn(white_turn);
	Sprite sWhiteWin(white_win);
	Sprite sBlackWin(black_win);
	Sprite sReset(reset_board);
	Sprite sChangeMode(change_mode);
	Sprite sModeClassic(mode_classic);
	Sprite sModeRandom(mode_random);
	sBoard2.setPosition(504,0);
	sWhiteTurn.setPosition(511, 10);
	sBlackTurn.setPosition(511, 10);
	sWhiteWin.setPosition(84, 196);
	sBlackWin.setPosition(84,196);
	sChangeMode.setPosition(512,100);
	sReset.setPosition(512,400);
	sModeClassic.setPosition(512,150);
	sModeRandom.setPosition(512,150);

	//shuffleNonPawns();
	loadPosition();

	bool isMove = false;
	float dx = 0, dy = 0;
	Vector2f oldPos, newPos;
	std::string str;
	int n = 0;

	while (window.isOpen())
	{
		Vector2i pos = Mouse::getPosition(window) - Vector2i(offset);

		Event e;
		while (window.pollEvent(e))
		{
			if (e.type == Event::Closed)
				window.close();

			////move back//////
			if (e.type == Event::KeyPressed)
				if (e.key.code == Keyboard::BackSpace)
				{
					if (position.length() > 6) position.erase(position.length() - 6, 5); loadPosition();
				}

			/////drag and drop///////
			if (e.type == Event::MouseButtonPressed) {
				if (e.key.code == Mouse::Left) {
					Vector2f mousePosF = window.mapPixelToCoords(Mouse::getPosition(window));

					// ==== 检查是否点击了“重置”按钮 ====
					if (sReset.getGlobalBounds().contains(mousePosF)) {
						std::cout << "Reset button clicked!" << std::endl;
						position = "";
						isWhiteTurn = true;
						resetBoard();
						if (mode == 1) {
							shuffleNonPawns();
						} 
						loadPosition();     // 重置棋盘布局
						continue; // 跳过当前循环（避免继续拖动棋子）
					}

					// ==== 检查是否点击了“切换模式”按钮 ====
					if (sChangeMode.getGlobalBounds().contains(mousePosF)) {
						std::cout << "Change Mode button clicked!" << std::endl;
						mode = (mode + 1)%2;
						position = "";
						isWhiteTurn = true;
						resetBoard();
						if (mode == 1) {
							shuffleNonPawns();
						}
						loadPosition();     // 重置棋盘布局
						continue; // 跳过当前循环（避免继续拖动棋子）
					}

					for (int i = 0; i < 32; i++) {
						if (f[i].getGlobalBounds().contains(pos.x, pos.y))
						{
							isMove = true; n = i;
							dx = pos.x - f[i].getPosition().x;
							dy = pos.y - f[i].getPosition().y;
							oldPos = f[i].getPosition();
						}
					}
				}
			}

			if (e.type == Event::MouseButtonReleased)
				if (e.key.code == Mouse::Left)
				{
					if (!isMove) break;

					isMove = false;
					Vector2f p = f[n].getPosition() + Vector2f(size / 2, size / 2);
					newPos = Vector2f(size * int(p.x / size), size * int(p.y / size));
					str = toChessNote(oldPos) + toChessNote(newPos);
					if (isValidMove(oldPos, newPos, board, isWhiteTurn)) {
						str = toChessNote(oldPos) + toChessNote(newPos);
						move(str);
						if (oldPos != newPos) position += str + " ";
						f[n].setPosition(newPos);

						isWhiteTurn = !isWhiteTurn;

						// ===== 更新 board =====
						int x1 = oldPos.x / size, y1 = oldPos.y / size;
						int x2 = newPos.x / size, y2 = newPos.y / size;

						board[y2][x2] = board[y1][x1]; // 把棋子放到新位置
						board[y1][x1] = 0;             // 清空旧位置

						int result = checkWinner();
						if (result != 0) {
							if (result == 1)
								std::cout << "White wins!" << std::endl;
							else
								std::cout << "Black wins!" << std::endl;
						}
					}
					else {
						f[n].setPosition(oldPos); // 不合法还原原位
					}
				}
		}

		//comp move
		if (Keyboard::isKeyPressed(Keyboard::Space) && !isWhiteTurn && mode==0)
		{
			do {
				str = getNextMove(position);
				if (str == "error") break;

				oldPos = toCoord(str[0], str[1]);
				newPos = toCoord(str[2], str[3]);
			} while (!isValidMove(oldPos, newPos, board,isWhiteTurn));

			for (int i = 0; i < 32; i++) if (f[i].getPosition() == oldPos) n = i;

			for (int k = 0; k < 50; k++)
			{
				Vector2f p = newPos - oldPos;
				f[n].move(p.x / 50, p.y / 50);
				window.draw(sBoard);
				for (int i = 0; i < 32; i++) f[i].move(offset);
				for (int i = 0; i < 32; i++) window.draw(f[i]); window.draw(f[n]);
				for (int i = 0; i < 32; i++) f[i].move(-offset);
				window.display();
			}

			move(str);  position += str + " ";
			f[n].setPosition(newPos);

			isWhiteTurn = !isWhiteTurn;

			int x1 = oldPos.x / size, y1 = oldPos.y / size;
			int x2 = newPos.x / size, y2 = newPos.y / size;

			board[y2][x2] = board[y1][x1];
			board[y1][x1] = 0;
		}

		if (isMove) f[n].setPosition(pos.x - dx, pos.y - dy);

		////// draw  ///////
		window.clear();
		window.draw(sBoard);
		for (int i = 0; i < 32; i++) f[i].move(offset);
		for (int i = 0; i < 32; i++) window.draw(f[i]); window.draw(f[n]);
		for (int i = 0; i < 32; i++) f[i].move(-offset);
		window.draw(sBoard2);
		if (isWhiteTurn) {
			window.draw(sWhiteTurn);
		}
		else {
			window.draw(sBlackTurn);
		}
		window.draw(sReset);
		window.draw(sChangeMode);
		if (mode == 0) {
			window.draw(sModeClassic);
		}
		else if (mode == 1) {
			window.draw(sModeRandom);
		}
		int result = checkWinner();
		if (result != 0) {
			if (result == 1) {
				window.draw(sWhiteWin);
			}
			else {
				window.draw(sBlackWin);
			}
				
		}
		window.display();
	}

	CloseConnection();

	return 0;
}