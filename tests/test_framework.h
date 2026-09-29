#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <vector>

// Framework de tests minimalista, sin dependencias externas.

struct TestCase {
    std::string name;
    std::function<void()> fn;
};

inline std::vector<TestCase>& testRegistry() {
    static std::vector<TestCase> registry;
    return registry;
}

struct TestRegistrar {
    TestRegistrar(const std::string& name, std::function<void()> fn) {
        testRegistry().push_back({name, std::move(fn)});
    }
};

inline int g_failures = 0;
inline std::string g_currentTest;

#define TEST(name)                                                      \
    void name();                                                        \
    static TestRegistrar registrar_##name(#name, name);                 \
    void name()

#define ASSERT_TRUE(cond)                                                          \
    do {                                                                          \
        if (!(cond)) {                                                           \
            std::cerr << "[FAIL] " << g_currentTest << " - ASSERT_TRUE(" #cond    \
                      << ") en " << __FILE__ << ":" << __LINE__ << "\n";          \
            g_failures++;                                                        \
        }                                                                         \
    } while (0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(a, b)                                                           \
    do {                                                                         \
        auto va_ = (a);                                                          \
        auto vb_ = (b);                                                         \
        if (!(va_ == vb_)) {                                                    \
            std::cerr << "[FAIL] " << g_currentTest << " - ASSERT_EQ(" #a ", " #b \
                      << ") : " << va_ << " != " << vb_ << " en " << __FILE__     \
                      << ":" << __LINE__ << "\n";                                \
            g_failures++;                                                        \
        }                                                                         \
    } while (0)
