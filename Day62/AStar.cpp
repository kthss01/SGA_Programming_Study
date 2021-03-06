#include "stdafx.h"
#include "AStar.h"


AStar::AStar()
{
}


AStar::~AStar()
{
}

HRESULT AStar::Init()
{
	_aStarState = ASTAR_STATE_END;

	_selectedType = TILE_TYPE_EMPTY;
	_selectedTypeColor = RGB(255, 255, 255);

	_startPointSet = false;
	_endPointSet = false;

	for (int i = 0; i < TILE_Y; i++) {
		for (int j = 0; j < TILE_X; j++) {
			_tile[i][j].type = TILE_TYPE_EMPTY;
			_tile[i][j].color = RGB(255, 255, 255);
			_tile[i][j].parent = NULL;
			_tile[i][j].F = BIGNUM;
			_tile[i][j].H = 0;
			_tile[i][j].i = i;
			_tile[i][j].j = j;
			_tile[i][j].rc =
				// 타일 만드는데 오른쪽 기점으로 해서 왼편으로 만들꺼
				// 이렇게 만드는 이유는 왼쪽 공간의 여백을 주려고 하는거
				RectMake(
					WINSIZEX - (TILE_X + 1) *  WIDTH + WIDTH * j,
					WINSIZEY / 2 - (TILE_Y / 2) * HEIGHT + HEIGHT * i,
					WIDTH, HEIGHT);
		}
	}

	_openList.clear();
	_closeList.clear();

	return S_OK;
}

void AStar::Release()
{
}

void AStar::Update()
{
	if (_aStarState == ASTAR_STATE_END) {
		// 타일 선택( start, end, wall empty)
		TileComposition();
	}

	if(INPUT->GetKeyDown(VK_SPACE) && 
		_startPointSet && _endPointSet 
		&& _aStarState == ASTAR_STATE_END) {
		// star, end, wall, empty 각각 해주어야하는 부분 초기화
		TileInitializing();
	}

	if (INPUT->GetKeyDown(VK_RETURN)) {
		if (_aStarState == ASTAR_STATE_FOUND ||
			_aStarState == ASTAR_STATE_NOWAY) {
			this->Init();
		}
	}

	// 종료가 되었을 때
	if (_aStarState == ASTAR_STATE_END ||
		_aStarState == ASTAR_STATE_FOUND ||
		_aStarState == ASTAR_STATE_NOWAY) return;

	AddOpenList();
	CalculateH();
	CalculateF();
	AddCloseList();
	CheckArrive();
}

void AStar::Render()
{
	// 한번 더 흰색으로 초기화 시켜주는거 배경 쓸거면 주석처리
	PatBlt(GetMemDC(), 0, 0, WINSIZEX, WINSIZEY, WHITENESS);

	SetTextColor(GetMemDC(), RGB(0, 0, 0));

	TextOut(GetMemDC(), 10, 80,  "1 : EMPTY", strlen("1 : EMPTY"));
	TextOut(GetMemDC(), 10, 100, "2 : START", strlen("2 : START"));
	TextOut(GetMemDC(), 10, 120, "3 : END",   strlen("3 : END"));
	TextOut(GetMemDC(), 10, 140, "4 : WALL",  strlen("4 : WALL"));

	char str[128];
	if (_aStarState == ASTAR_STATE_END)
		sprintf_s(str, "Press Space to Start");
	else if (_aStarState == ASTAR_STATE_FOUND)
		sprintf_s(str, "Way Found!");
	else if (_aStarState == ASTAR_STATE_NOWAY)
		sprintf_s(str, "No Way!");
	else if (_aStarState == ASTAR_STATE_SEARCHING)
		sprintf_s(str, "Now Searching...");

	TextOut(GetMemDC(), 15, 160, str, strlen(str));

	HBRUSH newBrush, oldBrush;
	HFONT newFont, oldFont;

	newFont = CreateFont(
		9, 0, 0, 0,		// 너비 0으로 해도 높이랑 너비중 큰 값으로 설정해주는게 있음
		FW_NORMAL,
		false,			// 글자 진하게 할지
		false,			// 글자 기울게 할지
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY,
		DEFAULT_PITCH,	// 너비랑 높이 중에서 큰녀석 기준으로 글자 맞춰주는거
		FF_SWISS,
		"굴림"			// 글자체
	);
	oldFont = (HFONT)SelectObject(GetMemDC(), newFont);

	for (int i = 0; i < TILE_Y; i++) {
		for (int j = 0; j < TILE_X; j++) {
			newBrush = CreateSolidBrush(_tile[i][j].color);
			oldBrush = (HBRUSH)SelectObject(GetMemDC(), newBrush);

			Rectangle(GetMemDC(),
				_tile[i][j].rc.left, _tile[i][j].rc.top,
				_tile[i][j].rc.right, _tile[i][j].rc.bottom);

			SelectObject(GetMemDC(), oldBrush);
			DeleteObject(newBrush);

			// F 값이 계산되어 있을때만
			if (_tile[i][j].F < BIGNUM) {
				// 배경색 투명하게
				SetBkMode(GetMemDC(), TRANSPARENT);
				sprintf_s(_tile[i][j].str, "%d", _tile[i][j].F);
				TextOut(GetMemDC(),
					_tile[i][j].rc.left + 2,
					_tile[i][j].rc.top + 2,
					_tile[i][j].str, strlen(_tile[i][j].str));

				sprintf_s(_tile[i][j].str, "%d", _tile[i][j].G);
				TextOut(GetMemDC(),
					_tile[i][j].rc.left + 2,
					_tile[i][j].rc.top + 22,
					_tile[i][j].str, strlen(_tile[i][j].str));

				sprintf_s(_tile[i][j].str, "%d", _tile[i][j].H);
				TextOut(GetMemDC(),
					_tile[i][j].rc.left + 22,
					_tile[i][j].rc.top + 22,
					_tile[i][j].str, strlen(_tile[i][j].str));
			} // end of if(F < BIGNUM)

			if (_tile[i][j].parent == NULL) continue;

			int length = 15;
			// 시작지점 도형의 중심좌표, 도착지점 부모의 중심좌표
			LineMake(GetMemDC(),
				(_tile[i][j].rc.left + _tile[i][j].rc.right) / 2,
				(_tile[i][j].rc.top + _tile[i][j].rc.bottom) / 2,

				(_tile[i][j].parent->j - _tile[i][j].j) * length
				+ (_tile[i][j].rc.left +_tile[i][j].rc.right) / 2,
				(_tile[i][j].parent->i - _tile[i][j].i) * length
				+ (_tile[i][j].rc.top + _tile[i][j].rc.bottom) / 2
				);

		} // end of for(j)
	} // end of for(i)

	SelectObject(GetMemDC(), oldFont);
	DeleteObject(newFont);
}

void AStar::TileComposition()
{
	if (INPUT->GetKeyDown('1')) {
		_selectedType = TILE_TYPE_EMPTY;
		_selectedTypeColor = RGB(255, 255, 255);
	}
	else if (INPUT->GetKeyDown('2')) {
		_selectedType = TILE_TYPE_START;
		_selectedTypeColor = RGB(100, 255, 100);
	}
	if (INPUT->GetKeyDown('3')) {
		_selectedType = TILE_TYPE_END;
		_selectedTypeColor = RGB(255, 100, 100);
	}
	if (INPUT->GetKeyDown('4')) {
		_selectedType = TILE_TYPE_WALL;
		_selectedTypeColor = RGB(100, 100, 255);
	}

	if (INPUT->GetKey(VK_LBUTTON)) {
		for (int i = 0; i < TILE_Y; i++) {
			for (int j = 0; j < TILE_X; j++) {
				if (PtInRect(&_tile[i][j].rc, g_ptMouse)) {
					// 클릭 당시에 start, end 설정되어있으면 false로 만들고 해당 타입 설정해야되서
					if (_tile[i][j].type == TILE_TYPE_START) _startPointSet = false;
					if (_tile[i][j].type == TILE_TYPE_END) _endPointSet = false;

					_tile[i][j].type = _selectedType;
					_tile[i][j].color = _selectedTypeColor;

					if (_selectedType == TILE_TYPE_START) {
						if (_startPointSet) {
							_tile[_startY][_startX].type = TILE_TYPE_EMPTY;
							_tile[_startY][_startX].color = RGB(255,255,255);
						}
						_startPointSet = true;
						_startX = j;
						_startY = i;
					}

					if (_selectedType == TILE_TYPE_END) {
						if (_endPointSet) {
							_tile[_endY][_endX].type = TILE_TYPE_EMPTY;
							_tile[_endY][_endX].color = RGB(255, 255, 255);
						}
						_endPointSet = true;
						_endX = j;
						_endY = i;
					}

				} // end of if(PtInRect)
			} // end of for(j)
		} // end of for(i)
	} // end of if(INPUT->VK_LBUTTON)
}

void AStar::TileInitializing()
{
	for (int i = 0; i < TILE_Y; i++) {
		for (int j = 0; j < TILE_X; j++) {
			if (_tile[i][j].type == TILE_TYPE_EMPTY) {
				_tile[i][j].listOn = false;
				_tile[i][j].walkable = true;
			}
			else if (_tile[i][j].type == TILE_TYPE_WALL) {
				_tile[i][j].listOn = false;	// 햇다고 해도 되긴함
				_tile[i][j].walkable = false;
			}
			else if (_tile[i][j].type == TILE_TYPE_START) {
				// 시작지점은 리스트에 들어가 있어야함
				_tile[i][j].listOn = true;
				_tile[i][j].walkable = true;
				_closeList.push_back(&_tile[i][j]);
			}
			else if (_tile[i][j].type == TILE_TYPE_END) {
				_endX = j;
				_endY = i;

				_tile[i][j].listOn = false;
				_tile[i][j].walkable = true;
			}
		}
	}

	_aStarState = ASTAR_STATE_SEARCHING;
	_lastIndex = 0;
}

void AStar::AddOpenList()
{
	Ci = _closeList[_lastIndex]->i;
	Cj = _closeList[_lastIndex]->j;
	Cg = _closeList[_lastIndex]->G;

	// 윗공간을 계산할 수 있을 때 맨 위 타일이 아닐 때
	if (Ci != 0) { /// 상단
		// 걸어 갈 수 있을 때
		if (_tile[Ci - 1][Cj].walkable) {
			// OpenList에 있지 않을 때
			if (!_tile[Ci - 1][Cj].listOn) {
				_tile[Ci - 1][Cj].listOn = true;
				_tile[Ci - 1][Cj].color = RGB(220, 255, 220);	// 연녹색
				_tile[Ci - 1][Cj].G = Cg + 10;
				_tile[Ci - 1][Cj].parent = _closeList[_lastIndex];
				_openList.push_back(&_tile[Ci - 1][Cj]);
			}
			else {
				// 가중치가 더 작으면 변경
				if (Cg + 10 < _tile[Ci - 1][Cj].G) {
					_tile[Ci - 1][Cj].G = Cg + 10;
					_tile[Ci - 1][Cj].parent = _closeList[_lastIndex];
				}
			}
		} // end of if(_tile[Ci-1][Cj].walkable)

		// 맨 왼 타일이 아닐 때
		if (Cj != 0) { /// 좌상단
			// 걸어 갈 수 있을 때
			if (_tile[Ci - 1][Cj - 1].walkable &&
				_tile[Ci - 1][Cj].walkable &&
				_tile[Ci][Cj - 1].walkable) {
				// OpenList에 있지 않을 때
				if (!_tile[Ci - 1][Cj - 1].listOn) {
					_tile[Ci - 1][Cj - 1].listOn = true;
					_tile[Ci - 1][Cj - 1].color = RGB(220, 255, 220);	// 연녹색
					_tile[Ci - 1][Cj - 1].G = Cg + 14;
					_tile[Ci - 1][Cj - 1].parent = _closeList[_lastIndex];
					_openList.push_back(&_tile[Ci - 1][Cj - 1]);
				}
				else {
					// 가중치가 더 작으면 변경
					if (Cg + 14 < _tile[Ci - 1][Cj - 1].G) {
						_tile[Ci - 1][Cj - 1].G = Cg + 14;
						_tile[Ci - 1][Cj - 1].parent = _closeList[_lastIndex];
					}
				}
			}
		} // end of if(Cj != 0)

		if (Cj != TILE_X - 1) { /// 우상단
			// 걸어 갈 수 있을 때
			if (_tile[Ci - 1][Cj + 1].walkable &&
				_tile[Ci - 1][Cj].walkable &&
				_tile[Ci][Cj + 1].walkable) {
				// OpenList에 있지 않을 때
				if (!_tile[Ci - 1][Cj + 1].listOn) {
					_tile[Ci - 1][Cj + 1].listOn = true;
					_tile[Ci - 1][Cj + 1].color = RGB(220, 255, 220);	// 연녹색
					_tile[Ci - 1][Cj + 1].G = Cg + 14;
					_tile[Ci - 1][Cj + 1].parent = _closeList[_lastIndex];
					_openList.push_back(&_tile[Ci - 1][Cj + 1]);
				}
				else {
					// 가중치가 더 작으면 변경
					if (Cg + 14 < _tile[Ci - 1][Cj + 1].G) {
						_tile[Ci - 1][Cj + 1].G = Cg + 14;
						_tile[Ci - 1][Cj + 1].parent = _closeList[_lastIndex];
					}
				}
			}
		} // end of if(Cj != TILE_X -1)
	} // end of if(Ci != 0)

	if (Cj != 0) { /// 좌측
		if (_tile[Ci][Cj - 1].walkable) {
			// OpenList에 있지 않을 때
			if (!_tile[Ci][Cj - 1].listOn) {
				_tile[Ci][Cj - 1].listOn = true;
				_tile[Ci][Cj - 1].color = RGB(220, 255, 220);	// 연녹색
				_tile[Ci][Cj - 1].G = Cg + 10;
				_tile[Ci][Cj - 1].parent = _closeList[_lastIndex];
				_openList.push_back(&_tile[Ci][Cj - 1]);
			}
			else {
				// 가중치가 더 작으면 변경
				if (Cg + 10 < _tile[Ci][Cj - 1].G) {
					_tile[Ci][Cj - 1].G = Cg + 10;
					_tile[Ci][Cj - 1].parent = _closeList[_lastIndex];
				}
			}
		}
	} // end of if(Cj != 0)

	if (Cj != TILE_X - 1) { /// 우측
		if (_tile[Ci][Cj + 1].walkable) {
			// OpenList에 있지 않을 때
			if (!_tile[Ci][Cj + 1].listOn) {
				_tile[Ci][Cj + 1].listOn = true;
				_tile[Ci][Cj + 1].color = RGB(220, 255, 220);	// 연녹색
				_tile[Ci][Cj + 1].G = Cg + 10;
				_tile[Ci][Cj + 1].parent = _closeList[_lastIndex];
				_openList.push_back(&_tile[Ci][Cj + 1]);
			}
			else {
				// 가중치가 더 작으면 변경
				if (Cg + 10 < _tile[Ci][Cj + 1].G) {
					_tile[Ci][Cj + 1].G = Cg + 10;
					_tile[Ci][Cj + 1].parent = _closeList[_lastIndex];
				}
			}
		}
	} // end of if(Cj != TILE_X - 1)

	if (Ci != TILE_Y - 1) { /// 하단
		if (_tile[Ci + 1][Cj].walkable) {
			// OpenList에 있지 않을 때
			if (!_tile[Ci + 1][Cj].listOn) {
				_tile[Ci + 1][Cj].listOn = true;
				_tile[Ci + 1][Cj].color = RGB(220, 255, 220);	// 연녹색
				_tile[Ci + 1][Cj].G = Cg + 10;
				_tile[Ci + 1][Cj].parent = _closeList[_lastIndex];
				_openList.push_back(&_tile[Ci + 1][Cj]);
			}
			else {
				// 가중치가 더 작으면 변경
				if (Cg + 10 < _tile[Ci + 1][Cj].G) {
					_tile[Ci + 1][Cj].G = Cg + 10;
					_tile[Ci + 1][Cj].parent = _closeList[_lastIndex];
				}
			}
		} // end of if(_tile[Ci + 1][Cj].walkable)

		if (Cj != 0) { /// 좌하단
			if (_tile[Ci + 1][Cj - 1].walkable &&
				_tile[Ci + 1][Cj].walkable &&
				_tile[Ci][Cj - 1].walkable) {
				// OpenList에 있지 않을 때
				if (!_tile[Ci + 1][Cj - 1].listOn) {
					_tile[Ci + 1][Cj - 1].listOn = true;
					_tile[Ci + 1][Cj - 1].color = RGB(220, 255, 220);	// 연녹색
					_tile[Ci + 1][Cj - 1].G = Cg + 14;
					_tile[Ci + 1][Cj - 1].parent = _closeList[_lastIndex];
					_openList.push_back(&_tile[Ci + 1][Cj - 1]);
				}
				else {
					// 가중치가 더 작으면 변경
					if (Cg + 14 < _tile[Ci + 1][Cj - 1].G) {
						_tile[Ci + 1][Cj - 1].G = Cg + 14;
						_tile[Ci + 1][Cj - 1].parent = _closeList[_lastIndex];
					}
				}
			}
		} // end of if(Cj != 0) 

		if (Cj != TILE_X - 1) { /// 우하단
			if (_tile[Ci + 1][Cj + 1].walkable &&
				_tile[Ci + 1][Cj].walkable &&
				_tile[Ci][Cj + 1].walkable) {
				// OpenList에 있지 않을 때
				if (!_tile[Ci + 1][Cj + 1].listOn) {
					_tile[Ci + 1][Cj + 1].listOn = true;
					_tile[Ci + 1][Cj + 1].color = RGB(220, 255, 220);	// 연녹색
					_tile[Ci + 1][Cj + 1].G = Cg + 14;
					_tile[Ci + 1][Cj + 1].parent = _closeList[_lastIndex];
					_openList.push_back(&_tile[Ci + 1][Cj + 1]);
				}
				else {
					// 가중치가 더 작으면 변경
					if (Cg + 14 < _tile[Ci + 1][Cj + 1].G) {
						_tile[Ci + 1][Cj + 1].G = Cg + 14;
						_tile[Ci + 1][Cj + 1].parent = _closeList[_lastIndex];
					}
				}
			}
		} // end of if(Cj != TILE_X - 1) 
	} // end of if(Ci != TILE_Y - 1)
}

void AStar::CalculateH()
{
	for (int i = 0; i < _openList.size(); i++) {
		int vertical = (_endX - _openList[i]->j) * 10;
		int horizontal = (_endY - _openList[i]->i) * 10;

		// 방향이 반대로 넘어가는 경우
		if (vertical < 0) vertical *= -1;
		if (horizontal < 0) horizontal *= -1;

		_openList[i]->H = vertical + horizontal;
	}
}

void AStar::CalculateF()
{
	for (int i = 0; i < _openList.size(); i++) {
		_openList[i]->F = _openList[i]->G + _openList[i]->H;
	}
}

void AStar::AddCloseList()
{
	if (_openList.size() == 0) {
		_aStarState = ASTAR_STATE_NOWAY;
		return;
	}

	int index = 0; // 오픈 리스트 중 가장 F가 작은 타일의 인덱스
	int lowest = BIGNUM; // 오픈 리스트 중 가장 작은 F 값

	for (int i = 0; i < _openList.size(); i++) {
		if (_openList[i]->F < lowest) {
			lowest = _openList[i]->F;
			index = i;
		}
	}
	_openList[index]->color = RGB(180, 180, 255); // 연한 파랑색
	
	_closeList.push_back(_openList[index]);
	_openList.erase(_openList.begin() + index);

	// 이렇게 계산해도 됨
	//_lastIndex = _closeList.size() - 1;
	_lastIndex++;
}

void AStar::CheckArrive()
{
	if (_closeList[_lastIndex]->i == _endY &&
		_closeList[_lastIndex]->j == _endX) {
		_aStarState = ASTAR_STATE_FOUND;
		_closeList[_lastIndex]->color = RGB(255, 100, 100); // 진한 빨강
		ShowWay(_closeList[_lastIndex]);
	}
}

void AStar::ShowWay(aStarTile * tile)
{
	// 도착지점이 아닐 때까지
	if (!(tile->i == _endY && tile->j == _endX))
		tile->color = RGB(255, 180, 180); 

	tile = tile->parent;

	if (tile->parent == NULL)
		return;
	else
		ShowWay(tile);
}
