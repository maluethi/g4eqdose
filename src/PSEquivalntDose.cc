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

    G4String data_dir = "data/";
    G4String geometry = "AP";

    fGammaCoeff = ConvCoeff("Photons", geometry, data_dir);
    fElectronCoeff = ConvCoeff("Electrons", geometry, data_dir);
    fPositronCoeff = ConvCoeff("Positrons", geometry, data_dir);
    fProtonCoeff = ConvCoeff("Protons", geometry, data_dir);
    fNeutronCoeff = ConvCoeff("Neutrons", geometry, data_dir);

}

PSEquivalntDose::~PSEquivalntDose() {;}

void PSEquivalntDose::Initialize(G4HCofThisEvent* HCE) {
    EvtMap = new G4THitsMap<G4double>(detector->GetName(),GetName());
    if (HCID < 0) HCID = GetCollectionID(0);
    HCE->AddHitsCollection(HCID, EvtMap);
}

void PSEquivalntDose::EndOfEvent(G4HCofThisEvent*) {}

G4bool PSEquivalntDose::ProcessHits(G4Step * aStep, G4TouchableHistory *) {
    //G4cout << "HIT HIT HIT" << G4endl;

    G4double stepLength = aStep->GetStepLength();
    if ( stepLength == 0. ) return FALSE;

    auto pre_step = aStep->GetPreStepPoint();
    auto energy = pre_step->GetKineticEnergy();
    auto pid = aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    auto name = aStep->GetTrack()->GetParticleDefinition()->GetParticleName();

    G4double coeff = 0;

    switch (pid) {
        case 22:
            coeff = fGammaCoeff.GetCoefficient(energy); break;
        case 11:
            coeff = fElectronCoeff.GetCoefficient(energy); break;
        case -11:
            coeff = fPositronCoeff.GetCoefficient(energy); break;
        case 2212:
            coeff = fProtonCoeff.GetCoefficient(energy); break;
        case 2112:
            coeff = fNeutronCoeff.GetCoefficient(energy); break;
        default:
            G4ExceptionDescription ED;
            ED << "No conversion coefficients available for " << pid << "(" << name << ")" << "energy:" << energy << G4endl;
            G4Exception("PSEquivalentDose::ProcessHits","Whoopsa",JustWarning,ED);
    }


    G4int idx = ((G4TouchableHistory*)
            (pre_step->GetTouchable()))->GetReplicaNumber(indexDepth);
    G4double cubicVolume = ComputeVolume(aStep, idx);
    G4double flux = stepLength / cubicVolume;

    G4double CellEqDose = flux * coeff;
    if (weighted) CellEqDose *= pre_step->GetWeight();
    G4int index = GetIndex(aStep);
    EvtMap->add(index, CellEqDose);

    //G4cout << "PDG:" << aStep->GetTrack()->GetParticleDefinition()->GetPDGEncoding() << " EqDose: " << CellEqDose << " ENE: " << pre_step->GetKineticEnergy() << " C: " << coeff  << G4endl;

    return TRUE;

}

void PSEquivalntDose::clear() {}

void PSEquivalntDose::DrawAll() {}

void PSEquivalntDose::PrintAll() {}

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