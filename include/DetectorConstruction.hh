//
// Created by maluethi on 12/16/20.
//

#ifndef EQDOSE_DETECTORCONSTRUCTION_HH
#define EQDOSE_DETECTORCONSTRUCTION_HH


#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

/// Detector construction class to define materials and geometry.
///
/// Crystals are positioned in Ring, with an appropriate rotation matrix.
/// Several copies of Ring are placed in the full detector.

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

public:
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }
    G4double  GetTestDx() const { return test_dxxy;}

private:
    void DefineMaterials();

    G4bool  fCheckOverlaps;
    G4double test_dxxy;

    G4LogicalVolume*  fScoringVolume;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#endif //EQDOSE_DETECTORCONSTRUCTION_HH
