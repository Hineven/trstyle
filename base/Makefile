CFLAGS = -Wall -Werror -Ofast

libcmds = -lpthread -L/usr/local/lib -lglfw -lX11 -ldl -lGL -lpng

objs = \
calculation.o \
stylization.o \
Pretreatment.o \
data.o 
style: $(objs)
	g++ main.cpp $(CFLAGS) -c -o main.o
	g++ $(objs) main.o $(libcmds) -o style
.PHONY : style


%.d: %.cpp
	@set -e; \
	rm -f $@; \
	g++ -MM $< > $@.$$$$; \
	sed 's,.*:,$*.o : ,g' < $@.$$$$ >> $@; \
	echo '\tg++ $$< -c -o $$@ $(CFLAGS)\n' >> $@;\
	g++ -MM $< > $@.$$$$; \
	sed 's,.*:,$@ : ,g' < $@.$$$$ >> $@; \
	rm -f $@.$$$$

include $(objs:%.o=%.d)

clean: 
	rm -f `find ./ -name '*.d'`
	rm -f `find ./ -name '*.o'`
	rm -f chen

.PHONY : clean
