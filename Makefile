TARGET=rock-paper-scissors.exe
SOURCE=main.cpp
SOURCES=$(SOURCE) *.h
FLAGS=-std=c++17 -s -mwindows -Wall

$(TARGET): $(SOURCES)
	g++ $(FLAGS) $(SOURCE) -o $(TARGET)
test: tests.cpp *.h
	g++ $(FLAGS) tests.cpp -o tests.exe
	./tests.exe
clean:
	rm $(TARGET)
