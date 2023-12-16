// a bunch of architecture specfic stuff
extern int check_a20();
extern void enable_a20();

int setup_arch(){
	if(!check_a20())
		enable_a20();
	return 0;
}
