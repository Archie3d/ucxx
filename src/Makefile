TARGET = test

CC = gcc
CXX = g++
LD = ld
RM = rm -rf

INCLUDES = .
SOURCES = \
	StringUtils.cpp\
	ByteArray.cpp\
	ByteArraySerializer.cpp\
	Variant.cpp\
	Mutex.cpp\
	Sema.cpp\
	Thread.cpp\
	Socket.cpp\
	TcpSocket.cpp\
	TcpServer.cpp\
	test.cpp

LIBS = pthread
OBJECTS = $(patsubst %.cpp, obj/%.o, $(SOURCES))

CXXFLAGS = $(patsubst %, -I%, $(INCLUDES))
CXXFLAGS += -Wall -pthread
LINKFLAGS += $(patsubst %, -l%, $(LIBS))


all: $(TARGET)

$(OBJECTS): | obj

obj:
	@mkdir -p $@

$(TARGET): $(OBJECTS)
	@echo [ L ] $@
	@$(CXX) $(OBJECTS) $(CXXFLAGS) $(LINKFLAGS) -o $(TARGET)

clean:
	@$(RM) $(TARGET)
	@$(RM) $(OBJECTS)
	@$(RM) obj/

obj/%.o: %.cpp
	@echo [ C ] $<
	@$(CXX) -c $< $(CXXFLAGS) -o $@
