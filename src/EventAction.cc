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
/// \file B1EventAction.cc
/// \brief Implementation of the B1EventAction class

#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4THitsMap.hh"


EventAction::EventAction(RunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction),
  fCollID_eqdose(-1)
{} 


EventAction::~EventAction()
{}


void EventAction::BeginOfEventAction(const G4Event* evt)
{
  G4int total_events = G4RunManager::GetRunManager()->GetNumberOfEventsToBeProcessed();
  G4int event_number = evt->GetEventID();


}


void EventAction::EndOfEventAction(const G4Event* evt)
{
    G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
    if(!HCE) return;

    if (fCollID_eqdose < 0) {
        G4SDManager *SDMan = G4SDManager::GetSDMpointer();
        fCollID_eqdose = SDMan->GetCollectionID("TestBox/EqDose");
    }

    auto hc = GetHitsCollection(fCollID_eqdose, evt);


    G4THitsMap<G4double>* evtMap =
            (G4THitsMap<G4double>*)(HCE->GetHC(fCollID_eqdose));

    G4double eq_dose = 0.;
    std::map<G4int,G4double*>::iterator itr;
    for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
        eq_dose = *(itr->second);
        G4cout << "dose" << eq_dose << G4endl;
    }
    if (eq_dose > 0.) fRunAction->AddEqDose(eq_dose);

}

G4THitsMap<G4double>* EventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
    auto hitsCollection
            = static_cast<G4THitsMap<G4double>*>(
                    event->GetHCofThisEvent()->GetHC(hcID));

    if ( ! hitsCollection ) {
        G4ExceptionDescription msg;
        msg << "Cannot access hitsCollection ID " << hcID;
        G4Exception("B4dEventAction::GetHitsCollection()",
                    "MyCode0003", FatalException, msg);
    }

    return hitsCollection;
}