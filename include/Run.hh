//
// Created by maluethi on 12/26/20.
//

#ifndef EQDOSE_RUN_HH
#define EQDOSE_RUN_HH


#include "G4Run.hh"
#include "G4Event.hh"

#include "G4THitsMap.hh"
#include <vector>


class Run : public G4Run {

public:

    Run(const std::vector<G4String>& mfdName);
    virtual ~Run();

public:

    virtual void RecordEvent(const G4Event*);
    virtual void Merge(const G4Run*);


    G4int GetNumberOfHitsMap() const {return fRunMap.size();}

    G4THitsMap<G4double>* GetHitsMap(G4int i){return fRunMap[i];}
    G4THitsMap<G4double>* GetHitsMap(const G4String& detName,
                                     const G4String& colName);
    G4THitsMap<G4double>* GetHitsMap(const G4String& fullName);

    void DumpAllScorer();

private:
    std::vector<G4String> fCollName;
    std::vector<G4int> fCollID;
    std::vector<G4THitsMap<G4double>*> fRunMap;
};

//

#endif
