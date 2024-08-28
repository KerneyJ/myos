void panic(const char* str){
    __asm__ volatile("hlt");
}
