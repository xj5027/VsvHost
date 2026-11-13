#include "NetWorkMananger.h"
#include <chrono>
#include <thread>

int main(int argc, char** argv)
{
	NetWorkManager manager;
	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
}



