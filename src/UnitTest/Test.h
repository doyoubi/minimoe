#ifndef MINIMOE_TEST_H
#define MINIMOE_TEST_H

#include <string>
#include <iostream>

namespace minimoe
{

    inline void test_assert(bool expression, const std::string & file, size_t line)
    {
        if (expression) return;
        std::cout << "test fail : " << file << " line " << line << std::endl;
        exit(1);
    }

#define TEST_ASSERT(expression) \
    test_assert(expression, __FILE__, __LINE__);

}

#endif
