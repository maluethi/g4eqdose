//
// Created by luma on 12/17/20.
//

#include "ConvCoeff.hh"
#include "G4Exception.hh"

#include <gtest/gtest.h>

TEST(ConvCoeff, ReadFile) {
    G4String data_path = ".";
    G4String geometry = "AP";
    G4String particle = "Klapper";

    G4VExceptionHandler *ex;
    //ex = new G4VExceptionHandler("bls");

    // Abort if file does not exist
    ASSERT_DEATH(ConvCoeff("Wurst", geometry, data_path), "BeepBuup");

    auto conv = ConvCoeff(particle, geometry, data_path);

    EXPECT_ANY_THROW(conv.GetCoefficient(5));
}
