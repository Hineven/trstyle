data.o :  data.cpp data.hpp heads.hpp
	g++ $< -c -o $@ -Wall -Werror -Ofast

data.d :  data.cpp data.hpp heads.hpp
