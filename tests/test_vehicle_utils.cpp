#include "test_ffb_common.h"
#include "VehicleUtils.h"

namespace FFBEngineTests {

TEST_CASE(test_vehicle_class_parsing_keywords, "Internal") {
    // No engine instance needed
    
    ASSERT_EQ((int)ParseVehicleClass("LMP2 ELMS", ""), (int)ParsedVehicleClass::LMP2_UNRESTRICTED);
    ASSERT_EQ((int)ParseVehicleClass("GTE Pro", ""), (int)ParsedVehicleClass::GTE);
    ASSERT_EQ((int)ParseVehicleClass("GT3 Gen 2", ""), (int)ParsedVehicleClass::GT3);
    
    // Explicit requested branches for coverage:
    ASSERT_EQ((int)ParseVehicleClass("LMP2 WEC", ""), (int)ParsedVehicleClass::LMP2_RESTRICTED);
    ASSERT_EQ((int)ParseVehicleClass("LMP2", ""), (int)ParsedVehicleClass::LMP2_UNSPECIFIED);
    ASSERT_EQ((int)ParseVehicleClass("HYPERCAR", ""), (int)ParsedVehicleClass::HYPERCAR);
    ASSERT_EQ((int)ParseVehicleClass("", "488 GTE"), (int)ParsedVehicleClass::GTE);
    ASSERT_EQ((int)ParseVehicleClass("", "M4 GT3"), (int)ParsedVehicleClass::GT3);

    ASSERT_EQ((int)ParseVehicleClass("Random Car", ""), (int)ParsedVehicleClass::UNKNOWN);
}

TEST_CASE(test_vehicle_class_case_insensitivity, "Internal") {
    ASSERT_EQ((int)ParseVehicleClass("gt3", ""), (int)ParsedVehicleClass::GT3);
    ASSERT_EQ((int)ParseVehicleClass("GT3", ""), (int)ParsedVehicleClass::GT3);
    ASSERT_EQ((int)ParseVehicleClass("Gt3", ""), (int)ParsedVehicleClass::GT3);
}

TEST_CASE(test_vehicle_default_loads, "Internal") {
    // Check that all defined classes have a reasonable default load (> 3000N)
    for (int i = 0; i <= (int)ParsedVehicleClass::GT3; i++) {
        double load = GetDefaultLoadForClass((ParsedVehicleClass)i);
        ASSERT_GE(load, 4000.0);
    }
}

} // namespace FFBEngineTests
