#include "system.h"

RQSystem::RQSystem(Processable* input,Processable* calledInput,IOrbit* orbit,INode* node){
    this->node = node;
    this->inputProcess = input;
    this->calledInputProcess = calledInput;
    this->orbit = orbit;
}


Event* RQSystem::Process(){

    ProcessOutputClaim();
    Event* ret = ProcessNonCalledClaim(inputProcess->Process());
    ProcessNonCalledClaim(orbit->Process());
    ProcessCalledClaim();
    return ret;
}


Event* RQSystem::ProcessNonCalledClaim(Event* sended){
    Event* ret = nullptr;
    if(sended!=nullptr){
        ret = sended;
        EventType type = sended->GetType();;
        switch(type){
        case EventType::FromInputProcess:{
            statistic.incomeInputProcess++;
            break;
        }
        case EventType::Orbit:{
            statistic.orbitSize--;
            break;
        }
        default:
            break;
        }
        sended = node->Income(sended);
        if(sended!=nullptr){
            orbit->Accept(sended);
            statistic.orbitSize++;
        }
        else{
            switch(type){
            case EventType::FromInputProcess:{
                statistic.currentEventType = "Input";
                break;
            }
            case EventType::Orbit:{
                statistic.currentEventType = "Orbit";
                break;
            }
            default:
                break;
            }
        }
    }
    return ret;
}


void RQSystem::ProcessCalledClaim(){
    Event* sended = calledInputProcess->Process();
    if(sended!=nullptr){
        sended = node->Income(sended);
        statistic.incomeCalledProcess++;
        if(sended==nullptr)statistic.currentEventType="Called";
    }
}


void RQSystem::ProcessOutputClaim(){
    Event* outputClaim = node->Process();
    if (outputClaim !=nullptr){
        statistic.currentEventType="None";
        switch (outputClaim->GetType()){
        case EventType::FromInputProcess:
        case EventType::Orbit:{
            statistic.servedInputProcess++;
            statistic.currentRunServed.first++;
            break;
        }
        case EventType::Called:{
            statistic.servedCalledProcess++;
            statistic.currentRunServed.second++;
            break;
        }

        }
        outputClaim->SetType(EventType::Served);
    }
}

const Statistic* RQSystem::GetStatistics() const{
    return &statistic;
}

RQSystem::~RQSystem(){
    delete node;
    delete calledInputProcess;
    delete inputProcess;
}

