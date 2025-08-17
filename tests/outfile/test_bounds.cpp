/*
 * Test file to verify bounds checking behavior before and after fixes
 */

#include <boost/test/unit_test.hpp>
#include "swmm_output.h"

#define DATA_PATH "./test_example1.out"

struct FixtureBounds {
    FixtureBounds() {
        error = SMO_init(&p_handle);
        BOOST_REQUIRE(error == 0);
        
        error = SMO_open(p_handle, DATA_PATH);
        BOOST_REQUIRE(error == 0);
    }
    
    ~FixtureBounds() {
        SMO_close(p_handle);
    }
    
    int error;
    SMO_Handle p_handle;
    float* array;
    int array_dim;
};

BOOST_FIXTURE_TEST_CASE(test_subcatch_bounds_current, FixtureBounds) {
    // Test current behavior: index == N should be allowed (bug)
    // With 8 subcatchments (0-7), index 8 should be rejected but currently isn't
    error = SMO_getSubcatchSeries(p_handle, 8, SMO_runoff_rate, 0, 10, &array, &array_dim);
    // Current behavior: this might succeed (the bug we're fixing)
    std::cout << "subcatch index 8 (== Nsubcatch) error: " << error << std::endl;
    
    // Test clearly invalid index  
    error = SMO_getSubcatchSeries(p_handle, 100, SMO_runoff_rate, 0, 10, &array, &array_dim);
    BOOST_CHECK_EQUAL(error, 420); // Should definitely fail
}

BOOST_FIXTURE_TEST_CASE(test_node_bounds_current, FixtureBounds) {
    // Test current behavior: index == N should be allowed (bug)
    // With 14 nodes (0-13), index 14 should be rejected but currently isn't
    error = SMO_getNodeSeries(p_handle, 14, SMO_invert_depth, 0, 10, &array, &array_dim);
    std::cout << "node index 14 (== Nnodes) error: " << error << std::endl;
    
    // Test clearly invalid index
    error = SMO_getNodeSeries(p_handle, 100, SMO_invert_depth, 0, 10, &array, &array_dim);
    BOOST_CHECK_EQUAL(error, 420); // Should definitely fail
}

BOOST_FIXTURE_TEST_CASE(test_link_bounds_current, FixtureBounds) {
    // Test current behavior: index == N should be allowed (bug)
    // With 13 links (0-12), index 13 should be rejected but currently isn't  
    error = SMO_getLinkSeries(p_handle, 13, SMO_flow_rate_link, 0, 10, &array, &array_dim);
    std::cout << "link index 13 (== Nlinks) error: " << error << std::endl;
    
    // Test clearly invalid index
    error = SMO_getLinkSeries(p_handle, 100, SMO_flow_rate_link, 0, 10, &array, &array_dim);
    BOOST_CHECK_EQUAL(error, 420); // Should definitely fail
}

BOOST_FIXTURE_TEST_CASE(test_endperiod_bounds_current, FixtureBounds) {
    // Test current behavior: endPeriod == Nperiods might be allowed (bug)
    // With 36 periods (0-35), endPeriod 36 should be rejected but might not be
    error = SMO_getSubcatchSeries(p_handle, 0, SMO_runoff_rate, 0, 36, &array, &array_dim);
    std::cout << "endPeriod 36 (== Nperiods) error: " << error << std::endl;
    
    // Test clearly invalid endPeriod
    error = SMO_getSubcatchSeries(p_handle, 0, SMO_runoff_rate, 0, 100, &array, &array_dim);
    std::cout << "endPeriod 100 (>> Nperiods) error: " << error << std::endl;
}