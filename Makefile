COMPILER= g++
FLAGS= -Wall -g 
LIBS = -lglut -lGL -lGLU
DEPS = ObjReader.hpp
OBJ = ObjElement.o ObjReader.o TestReader.o
PROGRAMS = read_file_test

# for each .cpp file (or any change to ObjReader.hpp), create a .o
# file -c: generate the object file.  -o: put the object file in the
# file named $@, which is the file on the left hand side of :
%.o: %.cpp %.hpp $(DEPS)
	$(COMPILER) $(LIBS) -c -o $@ $< $(FLAGS)


read_file_test: $(OBJ)
	$(COMPILER) $(LIBS) -o $@ $^ $(FLAGS)


.PHONY : clean
clean :
	rm -f *.o $(PROGRAMS)
