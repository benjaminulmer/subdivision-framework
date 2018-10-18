#include "Program.h"

int main(int argc, char* argv[]) {
	Program p;
	p.setArgs(argc, argv);
	p.start();
	return 0;
}
