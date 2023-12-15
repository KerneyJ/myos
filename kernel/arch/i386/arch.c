// a bunch of architecture specfic stuff
extern int check_a20();

int setup_arch(){
	return check_a20();
}
