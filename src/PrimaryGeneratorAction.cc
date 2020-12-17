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


#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4PhysicalConstants.hh"

#include "G4GenericMessenger.hh"
#include "DetectorConstruction.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction()
        : G4VUserPrimaryGeneratorAction(),
          fParticleGun(nullptr),
          fEnvelopeBox(nullptr) {
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);


    // default particle kinematic
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName;
    G4ParticleDefinition *particle
            = particleTable->FindParticle(particleName = "gamma");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
    fParticleGun->SetParticleEnergy(6. * MeV);

    DefineCommands();
}


PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
}


void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {

    const DetectorConstruction* detCon
            = static_cast<const DetectorConstruction*>
            (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, -detCon->GetTestDx()));


    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition *particle
            = particleTable->FindParticle(BeamParticle_);

    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(BeamEnergyMean_);
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

void PrimaryGeneratorAction::DefineCommands() {
    fMessenger = new G4GenericMessenger(this,
                                        "/user/beam/",
                                        "Beam Control");


    auto& BeamParticleCMD = fMessenger->DeclareProperty("particle", BeamParticle_, "Beam particle");
    BeamParticleCMD.SetParameterName("part", true);
    BeamParticleCMD.SetDefaultValue("e-");

    auto& BeamEnergyMeanCMD = fMessenger->DeclarePropertyWithUnit("energy", "MeV", BeamEnergyMean_, "Mean energy of electrons.");
    BeamEnergyMeanCMD.SetParameterName("energy", true);
    BeamEnergyMeanCMD.SetRange("energy>=0.");
    BeamEnergyMeanCMD.SetDefaultValue("0.");

}