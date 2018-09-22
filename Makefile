CXX = clang++
LD = clang++
CXXFLAGS = -std=c++11 -stdlib=libstdc++ -c -g -O0 -Wall -Wextra -Werror -I./includes/
LDFLAGS = -std=c++11 -stdlib=libstdc++

MyBot.exe : MyBot.cpp Game_Api.o util.o
	$(LD) MyBot.cpp Game_Api.o util.o $(LDFLAGS) -o MyBot.exe

Game_Api.o : Game_Api.h Game_Api.cpp
	$(CXX) $(CXXFLAGS) Game_Api.cpp

util.o : util.h util.cpp
	$(CXX) $(CXXFLAGS) util.cpp
