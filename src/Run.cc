

#include "Run.hh"
#include "G4SDManager.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"


Run::Run(const std::vector<G4String>& mfdName) : G4Run()
{
    G4SDManager* pSDman = G4SDManager::GetSDMpointer();
    //=================================================
    //  Initalize RunMaps for accumulation.
    //  Get CollectionIDs for HitCollections.
    //=================================================
    G4int nMfd = mfdName.size();
    for ( G4int idet = 0; idet < nMfd ; idet++){  // Loop for all MFD.
        G4String detName = mfdName[idet];
        //--- Seek and Obtain MFD objects from SDmanager.
        G4MultiFunctionalDetector* mfd =
                (G4MultiFunctionalDetector*)(pSDman->FindSensitiveDetector(detName));
        //
        if ( mfd ){
            //--- Loop over the registered primitive scorers.
            for (G4int icol = 0; icol < mfd->GetNumberOfPrimitives(); icol++){
                // Get Primitive Scorer object.
                G4VPrimitiveScorer* scorer=mfd->GetPrimitive(icol);
                // collection name and collectionID for HitsCollection,
                // where type of HitsCollection is G4THitsMap in case of primitive
                // scorer.
                // The collection name is given by <MFD name>/<Primitive Scorer name>.
                G4String collectionName = scorer->GetName();
                G4String fullCollectionName = detName+"/"+collectionName;
                G4int    collectionID = pSDman->GetCollectionID(fullCollectionName);
                //
                if ( collectionID >= 0 ){
                    G4cout << "++ "<<fullCollectionName<< " id " << collectionID
                           << G4endl;
                    // Store obtained HitsCollection information into data members.
                    // And, creates new G4THitsMap for accumulating quantities during RUN.
                    fCollName.push_back(fullCollectionName);
                    fCollID.push_back(collectionID);
                    fRunMap.push_back(new G4THitsMap<G4double>(detName,collectionName));
                }else{
                    G4cout << "** collection " << fullCollectionName << " not found. "
                           << G4endl;
                }
            }
        }
    }
}

Run::~Run()
{
    //--- Clear HitsMap for RUN
    G4int nMap = fRunMap.size();
    for ( G4int i = 0; i < nMap; i++){
        if(fRunMap[i] ) fRunMap[i]->clear();
    }
    fCollName.clear();
    fCollID.clear();
    fRunMap.clear();
}


void Run::RecordEvent(const G4Event* aEvent)
{
    numberOfEvent++;

    //=============================
    // HitsCollection of This Event
    //============================
    G4HCofThisEvent* pHCE = aEvent->GetHCofThisEvent();
    if (!pHCE) return;

    pHCE->GetHC(0);

    //=======================================================
    // Sum up HitsMap of this Event  into HitsMap of this RUN
    //=======================================================
    G4int nCol = fCollID.size();
    for ( G4int i = 0; i < nCol ; i++ ){  // Loop over HitsCollection
        G4THitsMap<G4double>* evtMap=0;
        if ( fCollID[i] >= 0 ){           // Collection is attached to pHCE
            evtMap = (G4THitsMap<G4double>*)(pHCE->GetHC(fCollID[i]));
        }else{
            G4cout <<" Error evtMap Not Found "<< i << G4endl;
        }
        if ( evtMap )  {
            //=== Sum up HitsMap of this event to HitsMap of RUN.===
            *fRunMap[i] += *evtMap;

            //======================================================
        }
    }
}


void Run::Merge(const G4Run * aRun) {
    const Run * localRun = static_cast<const Run *>(aRun);

    //=======================================================
    // Merge HitsMap of working threads
    //=======================================================
    G4int nCol = localRun->fCollID.size();
    for ( G4int i = 0; i < nCol ; i++ ){  // Loop over HitsCollection
        if ( localRun->fCollID[i] >= 0 ){
            *fRunMap[i] += *localRun->fRunMap[i];
        }
    }

    G4Run::Merge(aRun);
}

G4THitsMap<G4double>* Run::GetHitsMap(const G4String& detName,
                                          const G4String& colName){
    G4String fullName = detName+"/"+colName;
    return GetHitsMap(fullName);
}

G4THitsMap<G4double>* Run::GetHitsMap(const G4String& fullName){
    G4int nCol = fCollName.size();
    for ( G4int i = 0; i < nCol; i++){
        if ( fCollName[i] == fullName ){
            return fRunMap[i];
        }
    }
    return NULL;
}

void Run::DumpAllScorer(){

    // - Number of HitsMap in this RUN.
    G4int n = GetNumberOfHitsMap();
    // - GetHitsMap and dump values.
    for ( G4int i = 0; i < n ; i++ ){
        G4THitsMap<G4double>* runMap =GetHitsMap(i);
        if ( runMap ) {
            G4cout << " PrimitiveScorer RUN "
                   << runMap->GetSDname() <<","<< runMap->GetName() << G4endl;
            G4cout << " Number of entries " << runMap->entries() << G4endl;
        }
    }
}
