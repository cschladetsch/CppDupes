#pragma once

#include <gtest/gtest.h>
#include "../include/rang.hpp"

class CustomTestListener : public testing::TestEventListener {
    testing::TestEventListener* default_listener;

public:
    explicit CustomTestListener(testing::TestEventListener* listener);
    ~CustomTestListener();

    // Overrides from TestEventListener
    void OnTestProgramStart(const testing::UnitTest& unit_test) override;
    void OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) override;
    void OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration) override;
    void OnEnvironmentsSetUpStart(const testing::UnitTest& unit_test) override;
    void OnEnvironmentsSetUpEnd(const testing::UnitTest& unit_test) override;
    void OnEnvironmentsTearDownStart(const testing::UnitTest& unit_test) override;
    void OnEnvironmentsTearDownEnd(const testing::UnitTest& unit_test) override;
    void OnTestSuiteStart(const testing::TestSuite& test_suite) override;
    void OnTestSuiteEnd(const testing::TestSuite& test_suite) override;
    void OnTestStart(const testing::TestInfo& test_info) override;
    void OnTestPartResult(const testing::TestPartResult& test_part_result) override;
    void OnTestEnd(const testing::TestInfo& test_info) override;
    void OnTestProgramEnd(const testing::UnitTest& unit_test) override;

};

