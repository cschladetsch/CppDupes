#include "CustomTestListener.hpp"
#include <iostream>

CustomTestListener::CustomTestListener(testing::TestEventListener* listener) : default_listener(listener) {}

void CustomTestListener::OnTestProgramStart(const testing::UnitTest& unit_test) {
    default_listener->OnTestProgramStart(unit_test);
}

void CustomTestListener::OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) {
    std::cout << "=== Test Iteration " << iteration << " Starting ===\n";
    default_listener->OnTestIterationStart(unit_test, iteration);
}

void CustomTestListener::OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration) {
    default_listener->OnTestIterationEnd(unit_test, iteration);
}

void CustomTestListener::OnEnvironmentsSetUpStart(const testing::UnitTest& unit_test) {
    default_listener->OnEnvironmentsSetUpStart(unit_test);
}

void CustomTestListener::OnEnvironmentsSetUpEnd(const testing::UnitTest& unit_test) {
    default_listener->OnEnvironmentsSetUpEnd(unit_test);
}

void CustomTestListener::OnEnvironmentsTearDownStart(const testing::UnitTest& unit_test) {
    default_listener->OnEnvironmentsTearDownStart(unit_test);
}

void CustomTestListener::OnEnvironmentsTearDownEnd(const testing::UnitTest& unit_test) {
    default_listener->OnEnvironmentsTearDownEnd(unit_test);
}

void CustomTestListener::OnTestSuiteStart(const testing::TestSuite& test_suite) {
    std::cout << "=== Test Suite '" << test_suite.name() << "' Starting ===\n";
    default_listener->OnTestSuiteStart(test_suite);
}

void CustomTestListener::OnTestSuiteEnd(const testing::TestSuite& test_suite) {
    std::cout << "=== Test Suite '" << test_suite.name()
              << "' Finished. Passed: " << test_suite.successful_test_count()
              << "/" << test_suite.total_test_count() << " tests ===\n";
    default_listener->OnTestSuiteEnd(test_suite);
}

void CustomTestListener::OnTestStart(const testing::TestInfo& test_info) {
    std::cout << "=== TEST '" << test_info.name() << "' Starting ===\n";
    default_listener->OnTestStart(test_info);
}

void CustomTestListener::OnTestPartResult(const testing::TestPartResult& test_part_result) {
    if (test_part_result.failed()) {
        std::cout << "FAILURE in " << test_part_result.file_name() << ":"
                  << test_part_result.line_number() << "\n"
                  << test_part_result.summary() << "\n";
    } else {
        std::cout << "SUCCESS: " << test_part_result.summary() << "\n";
    }
    default_listener->OnTestPartResult(test_part_result);
}

void CustomTestListener::OnTestEnd(const testing::TestInfo& test_info) {
    std::cout << "=== TEST '" << test_info.name() << "' ";
    if (test_info.result()->Passed()) {
        std::cout << "PASSED";
    } else {
        std::cout << "FAILED";
    }
    std::cout << " (took " << test_info.result()->elapsed_time() << "ms) ===\n";
    default_listener->OnTestEnd(test_info);
}

void CustomTestListener::OnTestProgramEnd(const testing::UnitTest& unit_test) {
    std::cout << "=== TEST PROGRAM COMPLETE ===\n"
              << "Total Tests: " << unit_test.total_test_count() << "\n"
              << "Passed: " << unit_test.successful_test_count() << "\n"
              << "Failed: " << unit_test.failed_test_count() << "\n"
              << "Disabled: " << unit_test.disabled_test_count() << "\n"
              << "Time Elapsed: " << unit_test.elapsed_time() << "ms\n";
    default_listener->OnTestProgramEnd(unit_test);
}

CustomTestListener::~CustomTestListener() {
    delete default_listener;
}
