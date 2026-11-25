CXX = g++
CXXFLAGS = -std=c++11 -Wall -g3 -pthread -c
LDFLAGS = -pthread

OBJS = main.o log.o conveyor.o producer.o consumer.o
PROGRAM = carassembly

$(PROGRAM) : $(OBJS)
	$(CXX) -o $(PROGRAM) $(OBJS) $(LDFLAGS)

main.o : main.cpp shared.h conveyor.h producer.h consumer.h log.h
	$(CXX) $(CXXFLAGS) main.cpp

log.o : log.cpp log.h car_assembly.h
	$(CXX) $(CXXFLAGS) log.cpp

conveyor.o : conveyor.cpp conveyor.h shared.h log.h
	$(CXX) $(CXXFLAGS) conveyor.cpp

producer.o : producer.cpp producer.h conveyor.h shared.h
	$(CXX) $(CXXFLAGS) producer.cpp

consumer.o : consumer.cpp consumer.h conveyor.h shared.h
	$(CXX) $(CXXFLAGS) consumer.cpp

clean: 
	rm -f *.o $(PROGRAM)
