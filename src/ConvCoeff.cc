//
// Created by maluethi on 12/17/20.
//

#include <fstream>
#include <G4ios.hh>
#include <limits>
#include <vector>
#include "G4Exception.hh"
#include "ConvCoeff.hh"

ConvCoeff::ConvCoeff() {;}

ConvCoeff::ConvCoeff(G4String ParticleName, G4String Geometry, G4String Filepath)
    : fParticleName(ParticleName), fGeometry(Geometry), fDataDir(Filepath) {

    ReadFile();

}

ConvCoeff::~ConvCoeff() {;}

G4double ConvCoeff::GetCoefficient(G4double energy) {
    auto up = fCoefficients.lower_bound(energy);    // point 2 first: Energy [MeV], second: Coeff [Sv/cm²]

    if (up == fCoefficients.end()) {
        G4ExceptionDescription ED;
        ED << "Energy out of range for Coeefficients: " << fParticleName << " and Energy: " << energy << G4endl;
        G4Exception("ConvCoeff::GetCoefficient","Just one Ping",JustWarning,ED);
        return 0.;
    }

    auto low = std::prev(up);   // point before


    // interpolate linearly
    auto dC = up->second - low->second;
    auto dE = up->first - low->first;

    auto m = dC / dE;
    auto n = (low->second * up->first - up->second * low->first) / dE;
    auto interp = m * energy + n;
    return interp;
}


void ConvCoeff::ReadFile() {
    G4String line;
    G4String filename = fDataDir + "/" + fParticleName + "_Effective dose.txt";

    std::ifstream input (filename);

    if (!input.good()) {
        G4ExceptionDescription ED;
        ED << "File does not exist: " << filename  << G4endl;
        G4Exception("ConvCoeff::ReadFile","BeepBuup",RunMustBeAborted,ED);
    }

    G4cout << "Reading from: " << filename << G4endl;

    G4int n_skip = 2;
    G4int idx = 0;

    std::vector<G4String> col;

    fCoefficients.insert(std::pair<G4double, G4double>(0, 0));

    while (std::getline(input, line))
    {
        if (idx < n_skip) {
            idx++;
            continue;
        }
        if (idx == 2) {
            //TODO: Handle Header
            idx++;
            continue;
        }
        idx++;
        std::istringstream iss(line);
        G4double ene;
        G4double AP;

        iss >> ene >> AP;

        if (input.fail()) {
            input.clear();
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else {
            fCoefficients.insert(std::pair<G4double, G4double>(ene, AP));
        }
        // process pair (a,b)
    }


}