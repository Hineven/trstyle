data.o :  data.cpp heads.hpp
	g++ $< -c -o $@ -Wall -Werror -Ofast

data.d :  data.cpp heads.hpp
