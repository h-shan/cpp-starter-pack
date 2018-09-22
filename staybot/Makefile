CXX = clang++
LD = clang++
CXXFLAGS = -std=c++11 -stdlib=libstdc++ -c -g -O0 -Wall -Wextra -Werror -I./includes/
LDFLAGS = -std=c++11 -stdlib=libstdc++

StayBot.exe : stayBot.cpp Game_Api.o
	$(LD) stayBot.cpp Game_Api.o $(LDFLAGS) -o StayBot.exe

Game_Api.o : Game_Api.h Game_Api.cpp
	$(CXX) $(CXXFLAGS) Game_Api.cpp
