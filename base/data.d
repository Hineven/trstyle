data.o :  data.cpp heads.hpp
	g++ $< -c -o $@ -Wall -Werror -O3

data.d :  data.cpp heads.hpp
