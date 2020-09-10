#
# HPB_bot makefile for Linux
#
# created: 12/16/2000 botman (botman@mailandnews.com)
#

# the following specifies the path to your MOD...
#MOD_DIR = /home/paul/hlds_l/rcbot

CPP = g++ -m32

BASEFLAGS = -Dstricmp=strcasecmp -Dstrcmpi=strcasecmp -DRCBOT_META_BUILD -D__linux__# -D_DEBUG

#CPPFLAGS = ${BASEFLAGS} -g -ggdb -march=i486 -I. -I../engine -I../common -I../pm_shared

CPPFLAGS = ${BASEFLAGS} -mtune=generic -march=i686 -mmmx -msse -msse2 -mfpmath=sse -pipe -O2 \
	-Wno-write-strings -Wno-write-strings -Wno-unused-local-typedefs \
	-funsafe-math-optimizations -I. -I../engine -I../common -I../pm_shared
		
OBJ = bits.o bot.o bot_client.o bot_commands.o bot_ga.o bot_menu.o bot_navigate.o bot_nn.o \
	bot_rep.o bot_visibles.o bot_weapons.o client.o dir.o dll.o engine.o ga.o globals.o \
	h_export.o linkfunc.o megahal.o netmessages.o perceptron.o rcbot_meta_api.o util.o waypoint.o

rcbot_mm.so: ${OBJ}
	${CPP} -fPIC -shared -o $@ ${OBJ} -ldl -lm
	cp -f rcbot_mm.so ./Release

clean:
	-rm -f *.o
	-rm -f rcbot_mm.so	

%.o:	%.cpp
	${CPP} ${CPPFLAGS} -c $< -o $@

%.o:	%.c
	${CPP} ${CPPFLAGS} -c $< -o $@
