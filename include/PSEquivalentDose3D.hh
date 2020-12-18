//
// Created by luma on 12/18/20.
//

#ifndef EQDOSE_PSEQUIVALENTDOSE3D_HH
#define EQDOSE_PSEQUIVALENTDOSE3D_HH

#include "PSEquivalntDose.hh"

class PSEquivalentDose3D : public PSEquivalntDose {
public: // with description
    PSEquivalentDose3D(G4String name,G4int nx,G4int ny, G4int nz);
    virtual ~PSEquivalentDose3D();

protected: // with description
    virtual G4int GetIndex(G4Step*);

private:
    G4int fNx, fNy, fNz;
};


#endif //EQDOSE_PSEQUIVALENTDOSE3D_HH
