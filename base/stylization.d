stylization.o :  stylization.cpp stylization.hpp heads.hpp data.hpp \
 calculation.hpp
	g++ $< -c -o $@ -Wall -Werror -Ofast

stylization.d :  stylization.cpp stylization.hpp heads.hpp data.hpp \
 calculation.hpp
