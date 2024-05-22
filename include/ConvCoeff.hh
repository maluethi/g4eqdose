//
// Created by maluethi on 12/17/20.
//

#ifndef EQDOSE_CONVCOEFF_HH
#define EQDOSE_CONVCOEFF_HH


#include <G4String.hh>
#include <map>

class ConvCoeff {
public:
    ConvCoeff();
    ConvCoeff(G4String ParticleName, G4String Geometry, G4String Filepath);
    ~ConvCoeff();

    G4double GetCoefficient(G4double energy);
    // Return the conversion factor from fluence to equivalent dose

private:
    void ReadFile();

    G4String fParticleName;
    G4String fPDG;
    G4String fGeometry;
    G4String fDataDir;

    std::map <G4double, G4double> fCoefficients;

};


#endif //EQDOSE_CONVCOEFF_HH
