//
// Created by maluethi on 12/16/20.
//

#ifndef EQDOSE_PSEQUIVALNTDOSE_HH
#define EQDOSE_PSEQUIVALNTDOSE_HH

#include <G4VPrimitiveScorer.hh>
#include <G4UImessenger.hh>
#include <G4THitsMap.hh>
#include "ConvCoeff.hh"

class PSEquivalntDose : public G4VPrimitiveScorer,
        public G4UImessenger {

public:
    PSEquivalntDose(G4String name, G4int depth=0);
    virtual ~PSEquivalntDose();

    virtual void Initialize(G4HCofThisEvent*);
    virtual void EndOfEvent(G4HCofThisEvent*);
    virtual void clear();
    virtual void DrawAll();
    virtual void PrintAll();

protected:
    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);
    virtual G4double ComputeVolume(G4Step* aStep, G4int idx);


private:
    G4String fGeometry;
    G4int HCID;
    G4THitsMap<G4double>* EvtMap;
    G4bool  weighted;

    ConvCoeff fGammaCoeff;
    ConvCoeff fElectronCoeff;
    ConvCoeff fPositronCoeff;
    ConvCoeff fProtonCoeff;
    ConvCoeff fNeutronCoeff;

};


#endif //EQDOSE_PSEQUIVALNTDOSE_HH
