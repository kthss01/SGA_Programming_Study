// Day93-Queue.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "CircularQueue.h"

int main()
{
	CircularQueue queue;

	for (int i = 0; i < 10; i++) {
		queue.Enqueue(i);
	}

	std::cout << queue.Percent() << std::endl;

	queue.Print();

	queue.Dequeue();
	queue.Dequeue();

	queue.Print();

    return 0;
}

