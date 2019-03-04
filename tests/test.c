#include <stdio.h>
#include <stdbool.h>

void log_enter() {
  printf("Entering\n");
}

void log_exit() {
  printf("Exiting\n");
}

__attribute__((annotate("ltl_verify"))) void annotate_this() {
  if (getchar()) {
    printf("Hello world!\n");
  } else {
    printf("No\n");
  }
}

int main() {
  annotate_this();
  return 0;
}
