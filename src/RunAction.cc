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
/// \file B1RunAction.cc
/// \brief Implementation of the B1RunAction class

#include <Run.hh>
#include <G4ScoringManager.hh>
#include <G4ScoringBox.hh>
#include <PSEquivalentDose3D.hh>
#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"


RunAction::RunAction()
: G4UserRunAction(),
  fEqDose(0.),
  fEqDose2(0.)
  {
  auto sievert = joule / kilogram;

  const G4double milliseivert = 1.e-3 * sievert;
  const G4double microsievert = 1.e-6 * sievert;
  const G4double nanosievert  = 1.e-9 * sievert;
  const G4double picosievert  = 1.e-12 * sievert;

  new G4UnitDefinition("milliseivert", "mSv" , "EqDose", milliseivert);
  new G4UnitDefinition("microsievert", "uSv" , "EqDose", microsievert);
  new G4UnitDefinition("nanosievert" , "nSv"  , "EqDose", nanosievert);
  new G4UnitDefinition("picosievert" , "pSv"  , "EqDose", picosievert);

  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fEqDose);
  accumulableManager->RegisterAccumulable(fEqDose2);

  fSDName.push_back(G4String("TestBox"));

  fManager = G4ScoringManager::GetScoringManagerIfExist();

  G4cout << "MANAGER" << G4endl;
  fManager->Dump();

}


RunAction::~RunAction()
{}
G4Run* RunAction::GenerateRun()
{
    return new Run(fSDName);
}

void RunAction::BeginOfRunAction(const G4Run*)
{ 
  // inform the runManager to save random number seed
      G4RunManager::GetRunManager()->SetRandomNumberStore(false);

      // reset accumulables to their initial values
      G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
      accumulableManager->Reset();

    const DetectorConstruction* detCons
            = static_cast<const DetectorConstruction*>
            (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    G4int nSegment[3];
    nSegment[0] = 2;
    nSegment[1] = 2;
    nSegment[2] = 2;

    G4double mSize[3];
    mSize[0] = 5.;
    mSize[1] = 5.;
    mSize[2] = 5.;

    G4VScoringMesh* mesh = new G4ScoringBox("TestBoxPS");

    mesh->SetNumberOfSegments(nSegment);
    mesh->SetSize(mSize);
    mesh->Construct(detCons->GetPhysicalVolume());

    PSEquivalentDose3D* ps = new PSEquivalentDose3D("EqDose", nSegment[0], nSegment[1], nSegment[2]);

    mesh->SetPrimitiveScorer(ps);

    fManager->RegisterScoringMesh(mesh);

    G4cout << "MANAGER" << G4endl;
    fManager->Dump();
    fManager->List();

}


void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;

  // Merge accumulables 
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  auto* tRun = (Run*) run;  // Get custom run object

    G4THitsMap<G4double>* totEdep  = tRun->GetHitsMap("TestBox/EqDose");
    std::map<G4int,G4double*>::iterator itr;
    for (itr = totEdep->GetMap()->begin(); itr != totEdep->GetMap()->end(); itr++) {
        G4cout << "dose " << itr->first << " " << " " << *itr->second << G4endl;
    }

    G4cout << "MANAGER" << G4endl;
    fManager->Dump();

  // Compute dose = total energy deposit in a run and its variance
  //
  G4double eqdose  = fEqDose.GetValue();
  G4double eqdose2 = fEqDose2.GetValue();
  
  G4double rms = eqdose2 - eqdose * eqdose / nofEvents;
  if (rms > 0.) rms = std::sqrt(rms); else rms = 0.;  

  const DetectorConstruction* detectorConstruction
   = static_cast<const DetectorConstruction*>
     (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  G4double mass = detectorConstruction->GetScoringVolume()->GetMass();



  const PrimaryGeneratorAction* generatorAction
   = static_cast<const PrimaryGeneratorAction*>
     (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String runCondition;
  if (generatorAction)
  {
    const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
    runCondition += particleGun->GetParticleDefinition()->GetParticleName();
    runCondition += " of ";
    G4double particleEnergy = particleGun->GetParticleEnergy();
    runCondition += G4BestUnit(particleEnergy,"Energy");
  }
        
  // Print
  //  
  if (IsMaster()) {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------";
  }
  else {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------";
  }
  
  G4cout
     << G4endl
     << " The run consists of " << nofEvents << " "<< runCondition << G4endl
     << " Cumulated dose per run, in scoring volume : " 
     << G4BestUnit( eqdose, "EqDose") << " rms = " << G4BestUnit( rms, "EqDose") << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl;
}



void RunAction::AddEqDose(G4double eq_dose)
{
    fEqDose  += eq_dose;
    fEqDose2 += eq_dose * eq_dose;
}