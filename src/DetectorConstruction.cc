//
// Created by maluethi on 12/16/20.
//
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include <G4PVReplica.hh>
#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"

#include "PSEquivalentDose3D.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
        : G4VUserDetectorConstruction(),
          fCheckOverlaps(true)
{
    DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
    G4NistManager* man = G4NistManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    // Gamma detector Parameters
    //
    test_dxxy = 1*cm;


    G4NistManager* nist = G4NistManager::Instance();
    G4Material* test_mat = nist->FindOrBuildMaterial("G4_CONCRETE");
    G4Material* galactic = nist->FindOrBuildMaterial("G4_Galactic");


    //     
    // World
    //
    G4double world_sizeXY = 1.2*test_dxxy;
    G4double world_sizeZ  = 1.2*test_dxxy;

    G4Box* solidWorld =
            new G4Box("World",                       //its name
                      0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ); //its size

    G4LogicalVolume* logicWorld =
            new G4LogicalVolume(solidWorld,          //its solid
                                test_mat,         //its material
                                "World");            //its name

    G4VPhysicalVolume* physWorld =
            new G4PVPlacement(0,                     //no rotation
                              G4ThreeVector(),       //at (0,0,0)
                              logicWorld,            //its logical volume
                              "World",               //its name
                              0,                     //its mother  volume
                              false,                 //no boolean operation
                              0,                     //copy number
                              fCheckOverlaps);       // checking overlaps 



    // Box Size:
    G4ThreeVector boxSize;
    boxSize.setX(test_dxxy);
    boxSize.setY(test_dxxy);
    boxSize.setZ(test_dxxy);


    auto* TestBox =
            new G4Box("TestBox", boxSize.x() / 2, boxSize.y() / 2, boxSize.z() / 2);

    G4LogicalVolume* lTestBox =
            new G4LogicalVolume(TestBox,        //its solid
                                test_mat,         //its material
                                "lTestBox");        //its name

    //
    // place patient in world
    //                    
    new G4PVPlacement(0,                       //no rotation
                      G4ThreeVector(),         //at (0,0,0)
                      lTestBox,            //its logical volume
                      "TestBox",               //its name
                      logicWorld,              //its mother  volume
                      false,                   //no boolean operation
                      0,                       //copy number
                      fCheckOverlaps);         // checking overlaps 


    // Print materials
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;



    // Replicas & such things
    G4int nxCells = 2;
    G4int nyCells = 2;
    G4int nzCells = 2;

    G4ThreeVector sensSize;
    sensSize.setX(boxSize.x() / (G4double) nxCells);
    sensSize.setY(boxSize.y() / (G4double) nyCells);
    sensSize.setZ(boxSize.z() / (G4double) nzCells);

    // Replication of Water Phantom Volume.
    // Y Slice
    G4String yRepName("RepY");
    G4VSolid* solYRep = new G4Box(yRepName, boxSize.x()/2.,sensSize.y()/2.,boxSize.z()/2.);
    G4LogicalVolume* logYRep = new G4LogicalVolume(solYRep, test_mat, yRepName);

    new G4PVReplica(yRepName,logYRep,lTestBox,kYAxis, nyCells, sensSize.y());

    // X Slice
    G4String xRepName("RepX");
    G4VSolid* solXRep = new G4Box(xRepName,sensSize.x()/2.,sensSize.y()/2.,boxSize.z()/2.);
    G4LogicalVolume* logXRep = new G4LogicalVolume(solXRep,test_mat,xRepName);
    new G4PVReplica(xRepName, logXRep, logYRep,kXAxis, nxCells, sensSize.x());

    // Z Slice
    G4String zRepName("RepZ");
    G4VSolid* solZRep = new G4Box(zRepName,sensSize.x()/2.,sensSize.y()/2.,sensSize.z()/2.);
    G4LogicalVolume* logZRep = new G4LogicalVolume(solZRep,test_mat,zRepName);
    new G4PVReplica(zRepName, logZRep, logXRep,kZAxis, nzCells, sensSize.z());

    fScoringVolume = lTestBox;

    //always return the physical World
    //
    return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
    auto sd_manager = G4SDManager::GetSDMpointer();
    sd_manager->SetVerboseLevel(1);

    // declare patient as a MultiFunctionalDetector scorer
    //  
    G4MultiFunctionalDetector* mfd_box = new G4MultiFunctionalDetector("TestBox");
    G4SDManager::GetSDMpointer()->AddNewDetector(mfd_box);
    G4VPrimitiveScorer* primitiv2 = new PSEquivalentDose3D("EqDose", 2, 2, 2);
    mfd_box->RegisterPrimitive(primitiv2);

    SetSensitiveDetector("RepZ", mfd_box);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
