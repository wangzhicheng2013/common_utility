#include <iostream>
#include "string_utility.hpp"
int main() {
    const char *str = "XXX\t\n122\n222,YY\\UUTY\t111\n\n\\1";
    const char *sep_chars = " \t,\n";
    int num_toks = 0;
    char **toks = G_STRING_UTILITY.extended_split_string(str, sep_chars, 0, &num_toks, '\\');
    std::cout << num_toks << std::endl;
    for (int i = 0;i < num_toks;i++) {
        std::cout << toks[i] << std::endl;
    }
    G_STRING_UTILITY.extended_split_free(&toks, num_toks);

    return 0;
}