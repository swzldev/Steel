#pragma once

#include <harness/test.h>
#include <harness/test_manager.h>

// this file provides macros to define and register tests
// as well as some helper macros for creating them

#define TEST(name) \
    int name(); \
    static test_registrar registrar_##name(#name, name); \
    int name()
