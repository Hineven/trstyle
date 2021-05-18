calculation.o :  calculation.cpp calculation.hpp heads.hpp data.hpp
	g++ $< -c -o $@ -Wall -Werror -O3

calculation.d :  calculation.cpp calculation.hpp heads.hpp data.hpp
