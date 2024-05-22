//
// Created by maluethi on 12/17/20.
//

#include <fstream>
#include <G4ios.hh>
#include "G4Exception.hh"
#include "ConvCoeff.hh"

ConvCoeff::ConvCoeff() {;}

ConvCoeff::ConvCoeff(G4String ParticleName, G4String Geometry, G4String Filepath)
    : fParticleName(ParticleName), fGeometry(Geometry), fDataDir(Filepath) {

    ReadFile();

}

ConvCoeff::~ConvCoeff() {;}

G4double ConvCoeff::GetCoefficient(G4double energy) {
    auto up = fCoefficients.upper_bound(energy);    // point 2 first: Energy [MeV], second: Coeff [Sv/cm²]

    if (up == fCoefficients.end()) {
        G4ExceptionDescription ED;
        ED << "Energy out of range for Coeefficients: " << fParticleName << " and Energy: " << energy << G4endl;
        G4Exception("ConvCoeff::GetCoefficient","Just one Ping",JustWarning,ED);
    }

    auto low = std::prev(up);   // point before


    // interpolate linearly
    auto dC = up->second - low->second;
    auto dE = up->first - low->first;

    auto m = dC / dE;
    auto n = (low->second * up->first - up->second * low->first) / dE;

    return m * energy + n;
}


void ConvCoeff::ReadFile() {
    G4String line;
    std::ifstream myfile (fDataDir + "/Protons_Effective dose.txt");
    while (std::getline(myfile, line))
    {
        myfile.ignore(3, '\n');

        std::istringstream iss(line);
        G4double ene;
        G4double AP;

        iss >> ene >> AP;

        fCoefficients.insert(std::pair<G4double, G4double >(ene, AP));

        // process pair (a,b)
    }


}