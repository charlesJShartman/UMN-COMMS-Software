BUILDDIR=build
BIN=radio_service spam_radio ground_control
TESTS=test_ppp
ARGS=sat verbose

all : ${BUILDDIR}
	make -s -C ${BUILDDIR} comms

run : radio_service
	bin/radio_service ${ARGS}

debug : radio_service
	gdb --tui --args bin/radio_service ${ARGS}

tests : ${TESTS}

${BUILDDIR} : CMakeLists.txt
	cmake -B ${BUILDDIR}

${BIN} ${TESTS} : ${BUILDDIR}
	make -s -C ${BUILDDIR} $@

.PHONY : clean
clean :
	rm -rf ${BUILDDIR} bin lib
