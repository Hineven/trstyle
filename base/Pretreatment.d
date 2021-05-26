Pretreatment.o :  Pretreatment.cpp heads.hpp data.hpp stylization.hpp \
 calculation.hpp
	g++ $< -c -o $@ -Wall -Werror -Ofast

Pretreatment.d :  Pretreatment.cpp heads.hpp data.hpp stylization.hpp \
 calculation.hpp
