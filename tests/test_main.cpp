#include "test_framework.h"

int main() {
    int total = 0;
    for (auto& t : testRegistry()) {
        g_currentTest = t.name;
        total++;
        t.fn();
    }
    std::cout << total << " tests corridos, " << g_failures << " fallas.\n";
    return g_failures == 0 ? 0 : 1;
}
