// preprocessor_else_else.c:17:8: error: #else after #else
// preprocessor_else_else.c:18:6: error: invalid preprocessing directive #error
// preprocessor_else_else.c:20:7: error: #endif without #if
// preprocessor_else_else.c:26:6: error: invalid preprocessing directive #error
// preprocessor_else_else.c:28:6: error: #else without #if
// preprocessor_else_else.c:34:7: error: #endif without #if

#define A 1
#define B 1

#if A == 1
  #if B != 1
    #error Bad 1
  #else
    int a;
  #endif
#else
  #if B != 1
    #error Bad 2
  #else
    #error Bad 3
  #endif
#endif

#if A != 1
  #if B == 1
    #error Bad 4
  #else
    #error Bad 5
  #endif
#else
  #if B == 1
    int b;
  #else
    #error Bad 6
  #endif
#endif

int main(int, char**) {
   a = 1;
   b = 2;
   return 0;
}
