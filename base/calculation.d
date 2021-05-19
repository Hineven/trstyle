calculation.o :  calculation.cpp calculation.hpp heads.hpp data.hpp
	g++ $< -c -o $@ -Wall -Werror -Ofast

calculation.d :  calculation.cpp calculation.hpp heads.hpp data.hpp
