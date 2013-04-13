COMPILER= g++
FLAGS= -Wall -g -std=c++0x 
LIBS= -lglut -lGL -lGLU
DEPS= ObjReader.hpp ObjElement.hpp Util.hpp
OBJ_BASIC= ObjElement.o ObjReader.o 
PROGRAMS= read_file_test
BOOST_REGEX_SO=/usr/lib/libboost_regex.a

# for each .cpp file (or any change to ObjReader.hpp), create a .o
# file -c: generate the object file.  -o: put the object file in the
# file named $@, which is the file on the left hand side of :
%.o: %.cpp $(DEPS)
	$(COMPILER) -c -o $@ $< $(FLAGS) $(LIBS) 


load_file_test: $(OBJ_BASIC) LoadFile.o DrawingGlobal.o DrawingGlobal.hpp $(BOOST_REGEX_SO)
	$(COMPILER) -o $@ $^ $(FLAGS) $(LIBS) 


read_file_test: $(OBJ_BASIC) TestReader.o $(BOOST_REGEX_SO)
	$(COMPILER) -o $@ $^ $(FLAGS) $(LIBS) 

.PHONY : clean
clean :
	rm -f *.o $(PROGRAMS)
