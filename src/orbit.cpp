#include "orbit.h"
IOrbit::IOrbit(DelayProvider *d): Processable(d){};




Orbit::Orbit(DelayProvider* d,double delayIntensity,double delayIntensity1) : IOrbit(d){
    this->delayIntensity = delayIntensity;
    this->delayIntensity1 = delayIntensity1;
}

void Orbit::Accept(Event* claim){
    claim->SetMoment(d->GetDelay(delayIntensity,delayIntensity1));
    claim->SetType(EventType::Orbit);
    claimStorage.push_back(claim);
}

Event* Orbit::Process(){
    Event* ret = nullptr;
    for(std::vector<Event*>::size_type i = 0; i != claimStorage.size(); i++) {
        if (claimStorage[i]->Moment()==Environment::Time()){
            ret=claimStorage[i];
            claimStorage.erase(claimStorage.begin() + i);
            --i;
            return ret;
        }
    }
    return nullptr;
}
