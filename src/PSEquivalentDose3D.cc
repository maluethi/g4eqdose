//
// Created by luma on 12/18/20.
//

#include "PSEquivalentDose3D.hh"

PSEquivalentDose3D::PSEquivalentDose3D(G4String name, G4int nx, G4int ny, G4int nz)
    : PSEquivalntDose(name),fNx(nx),fNy(ny),fNz(nz) {

}
PSEquivalentDose3D::~PSEquivalentDose3D() {}

G4int PSEquivalentDose3D::GetIndex(G4Step * aStep) {
    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int ix = touchable->GetReplicaNumber(1);
    G4int iy = touchable->GetReplicaNumber(2);
    G4int iz = touchable->GetReplicaNumber(0);

    G4int tmp = fNy;
    if (tmp) return iy*fNx*fNz+ix*fNz+iz;
    else return iy*fNx*fNz+ix*fNz+iz;
}
