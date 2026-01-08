SRC = main.cpp
TARGET = ${SRC:.cpp=}
LIB_SRCS = sched.cpp task.cpp tts.cpp
HEADERS = awaiter.hpp sched.hpp task_types.hpp task.hpp tts.hpp debug.hpp
FLAGS = -std=c++20

${TARGET}: ${SRC} ${LIB_SRCS} ${HEADERS}
	g++ -o $@ ${SRC} ${LIB_SRCS} ${FLAGS} -g

# all: ${SRC} ${LIB_SRCS}