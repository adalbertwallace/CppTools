#pragma once

enum class IncludeeType{
    InQuotes,
    InBrackets
};

class Includee{
    public:
        Includee(const std::string& name, IncludeeType incType): name(name), incType(incType){
            ;
        }
        std::string GetName() const {
            return name;
        }
        IncludeeType GetType() const {
            return incType;
        }
    private:
        std::string name;
        IncludeeType incType;   
};