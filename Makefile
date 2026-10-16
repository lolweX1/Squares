# Compiler and flags
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC

# Qt5 build configuration
QT_INCLUDES = -I/usr/include/x86_64-linux-gnu/qt5 \
              -I/usr/include/x86_64-linux-gnu/qt5/QtWidgets \
              -I/usr/include/x86_64-linux-gnu/qt5/QtGui \
              -I/usr/include/x86_64-linux-gnu/qt5/QtCore

QT_LIBS     = -lQt5Widgets -lQt5Gui -lQt5Core

# Target executable name
TARGET   = sandbox

# Source files
SRCS     = main.cpp Window.cpp Collision.cpp
HEADERS  = Window.hpp Rectangle.hpp Vec2.hpp Collision.hpp

# Generated Qt Meta-Object Compiler (MOC) files
MOC_HDR  = Window.hpp
MOC_CPP  = moc_Window.cpp
MOC_OBJ  = moc_Window.o

# Object files
OBJS     = main.o Window.o Collision.o $(MOC_OBJ)

# Default rule
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(QT_LIBS)

# Compile standard source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(QT_INCLUDES) -c $< -o $@

# Run Qt MOC on Window.hpp
$(MOC_CPP): $(MOC_HDR)
	moc $(MOC_INCLUDES) $< -o $@

# Compile MOC generated C++ file
$(MOC_OBJ): $(MOC_CPP)
	$(CXX) $(CXXFLAGS) $(QT_INCLUDES) -c $< -o $@

# Clean built binaries and temporary object files
clean:
	rm -f $(OBJS) $(MOC_CPP) $(TARGET)

.PHONY: all clean