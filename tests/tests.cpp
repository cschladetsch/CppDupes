#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>
#include "../include/FileHashMapper.hpp"
#include "../include/DirectoryComparer.hpp"

class CustomTestListener : public testing::TestEventListener {
    testing::TestEventListener* default_listener;
public:
    CustomTestListener(testing::TestEventListener* listener) : default_listener(listener) {}

    virtual void OnTestProgramStart(const testing::UnitTest& unit_test) override {
        default_listener->OnTestProgramStart(unit_test);
    }

    virtual void OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) override {
        std::cout << "\n=== Test Iteration " << iteration << " Starting ===\n";
        default_listener->OnTestIterationStart(unit_test, iteration);
    }

    virtual void OnEnvironmentsSetUpStart(const testing::UnitTest& unit_test) override {
        std::cout << "\n=== Setting Up Test Environment ===\n";
        default_listener->OnEnvironmentsSetUpStart(unit_test);
    }

    virtual void OnEnvironmentsSetUpEnd(const testing::UnitTest& unit_test) override {
        default_listener->OnEnvironmentsSetUpEnd(unit_test);
    }

    virtual void OnTestSuiteStart(const testing::TestSuite& test_suite) override {
        std::cout << "\n=== Test Suite '" << test_suite.name() << "' Starting ===\n";
        default_listener->OnTestSuiteStart(test_suite);
    }

    virtual void OnTestStart(const testing::TestInfo& test_info) override {
        std::cout << "\n=== TEST '" << test_info.name() << "' Starting ===\n";
        default_listener->OnTestStart(test_info);
    }

    virtual void OnTestPartResult(const testing::TestPartResult& test_part_result) override {
        if (test_part_result.failed()) {
            std::cout << "\nFAILURE in " << test_part_result.file_name() << ":" 
                     << test_part_result.line_number() << "\n"
                     << test_part_result.summary() << "\n";
        }
        default_listener->OnTestPartResult(test_part_result);
    }

    virtual void OnTestEnd(const testing::TestInfo& test_info) override {
        std::cout << "\n=== TEST '" << test_info.name() << "' ";
        if (test_info.result()->Passed()) {
            std::cout << "PASSED";
        } else {
            std::cout << "FAILED";
        }
        std::cout << " (took " << test_info.result()->elapsed_time() << "ms) ===\n";
        default_listener->OnTestEnd(test_info);
    }

    virtual void OnTestSuiteEnd(const testing::TestSuite& test_suite) override {
        std::cout << "\n=== Test Suite '" << test_suite.name() 
                 << "' Finished. Passed: " << test_suite.successful_test_count()
                 << "/" << test_suite.total_test_count() << " tests ===\n";
        default_listener->OnTestSuiteEnd(test_suite);
    }

    virtual void OnEnvironmentsTearDownStart(const testing::UnitTest& unit_test) override {
        std::cout << "\n=== Tearing Down Test Environment ===\n";
        default_listener->OnEnvironmentsTearDownStart(unit_test);
    }

    virtual void OnEnvironmentsTearDownEnd(const testing::UnitTest& unit_test) override {
        default_listener->OnEnvironmentsTearDownEnd(unit_test);
    }

    virtual void OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration) override {
        std::cout << "\n=== Test Iteration " << iteration << " Complete ===\n";
        default_listener->OnTestIterationEnd(unit_test, iteration);
    }

    virtual void OnTestProgramEnd(const testing::UnitTest& unit_test) override {
        std::cout << "\n=== TEST PROGRAM COMPLETE ===\n"
                 << "Total Tests: " << unit_test.total_test_count() << "\n"
                 << "Passed: " << unit_test.successful_test_count() << "\n"
                 << "Failed: " << unit_test.failed_test_count() << "\n"
                 << "Disabled: " << unit_test.disabled_test_count() << "\n"
                 << "Time Elapsed: " << unit_test.elapsed_time() << "ms\n";
        default_listener->OnTestProgramEnd(unit_test);
    }

    ~CustomTestListener() {
        delete default_listener;
    }
};

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    
    // Remove default listener
    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    auto default_listener = listeners.Release(listeners.default_result_printer());
    
    // Add custom verbose listener
    listeners.Append(new CustomTestListener(default_listener));

    return RUN_ALL_TESTS();
}
