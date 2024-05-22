//
// Created by maluethi on 12/16/20.
//

#include "PSEquivalntDose.hh"
#include "G4VSolid.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VPVParameterisation.hh"

class ConvCoeff;

PSEquivalntDose::PSEquivalntDose(G4String name, G4int depth) :
    G4VPrimitiveScorer(name, depth), G4UImessenger(), HCID(-1),EvtMap(0),weighted(true) {

    fGammaCoeff = ConvCoeff("bla", "bla", "data/");

}

PSEquivalntDose::~PSEquivalntDose() {;}

void PSEquivalntDose::Initialize(G4HCofThisEvent* HCE) {
    EvtMap = new G4THitsMap<G4double>(detector->GetName(),GetName());
    if (HCID < 0) HCID = GetCollectionID(0);
    HCE->AddHitsCollection(HCID, EvtMap);



}

void PSEquivalntDose::EndOfEvent(G4HCofThisEvent*) {};

G4bool PSEquivalntDose::ProcessHits(G4Step * aStep, G4TouchableHistory *) {
    G4cout << "HIT HIT HIT" << G4endl;

    G4double stepLength = aStep->GetStepLength();
    if ( stepLength == 0. ) return FALSE;

    auto pre_step = aStep->GetPreStepPoint();
    auto energy = pre_step->GetKineticEnergy();

    auto coeff = fGammaCoeff.GetCoefficient(energy);

    G4int idx = ((G4TouchableHistory*)
            (pre_step->GetTouchable()))->GetReplicaNumber(indexDepth);
    G4double cubicVolume = ComputeVolume(aStep, idx);

    G4double CellFlux = stepLength / cubicVolume;
    if (weighted) CellFlux *= pre_step->GetWeight();
    G4int index = GetIndex(aStep);
    EvtMap->add(index, CellFlux);

    G4cout << "PDG:" << aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() << " FLUX: " << CellFlux << " ENE: " << pre_step->GetKineticEnergy() << " C: " << coeff  << G4endl;

    return TRUE;

}

void PSEquivalntDose::clear() {};

void PSEquivalntDose::DrawAll() {};

void PSEquivalntDose::PrintAll() {};

G4double PSEquivalntDose::ComputeVolume(G4Step* aStep, G4int idx){

    G4VPhysicalVolume* physVol = aStep->GetPreStepPoint()->GetPhysicalVolume();
    G4VPVParameterisation* physParam = physVol->GetParameterisation();
    G4VSolid* solid = 0;
    if(physParam)
    { // for parameterized volume
        if(idx<0)
        {
            G4ExceptionDescription ED;
            ED << "Incorrect replica number --- GetReplicaNumber : " << idx << G4endl;
            G4Exception("PSEquivalentDose::ComputeVolume","DetPS0001",JustWarning,ED);
        }
        solid = physParam->ComputeSolid(idx, physVol);
        solid->ComputeDimensions(physParam,idx,physVol);
    }
    else
    { // for ordinary volume
        solid = physVol->GetLogicalVolume()->GetSolid();
    }

    return solid->GetCubicVolume();
}