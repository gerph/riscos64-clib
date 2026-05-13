extern "C" void __cxa_bad_cast() {
    __builtin_trap();
}

extern "C" void __cxa_bad_typeid() {
    __builtin_trap();
}
