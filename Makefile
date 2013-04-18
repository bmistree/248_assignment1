COMPILER= g++
FLAGS= -Wall -g -std=c++0x 
LIBS= -lglut -lGL -lGLU -I/usr/local/include/
DEPS= ObjReader.hpp ObjElement.hpp Util.hpp 
OBJ_BASIC= ObjElement.o ObjReader.o 
PROGRAMS= read_file_test load_file_test
SOS=/usr/lib/libboost_regex.so

# for each .cpp file (or any change to ObjReader.hpp), create a .o
# file -c: generate the object file.  -o: put the object file in the
# file named $@, which is the file on the left hand side of :
%.o: %.cpp $(DEPS)
	$(COMPILER) -c -o $@ $< $(FLAGS) $(LIBS) 

%.o: %.c $(DEPS)
	$(COMPILER) -c -o $@ $< $(FLAGS) $(LIBS) 


load_file_test: $(OBJ_BASIC) LoadFile.o DrawingGlobal.o DrawingGlobal.hpp Bitmap.o $(SOS)
	$(COMPILER) -o $@ $^ $(FLAGS) $(LIBS) 


tmp: test.cpp $(SOS)
	$(COMPILER) -o $@ $^ $(FLAGS) $(LIBS)

read_file_test: $(OBJ_BASIC) TestReader.o $(SOS)
	$(COMPILER) -o $@ $^ $(FLAGS) $(LIBS) 

.PHONY : clean
clean :
	rm -f *.o $(PROGRAMS)
