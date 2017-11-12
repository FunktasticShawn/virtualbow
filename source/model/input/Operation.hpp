#pragma once
#include "model/document/Document.hpp"
#include <json.hpp>

using nlohmann::json;

struct Operation: public DocumentNode
{
    DocumentItem<double> brace_height{*this, validators::any<double>, 0.2};
    DocumentItem<double>  draw_length{*this, validators::any<double>, 0.7};
    DocumentItem<double>   mass_arrow{*this, validators::pos<double>, 0.025};

    void load(const json& obj)
    {
        brace_height = (double) obj["brace_height"];
        draw_length  = (double) obj["draw_length"];
        mass_arrow   = (double) obj["mass_arrow"];
    }

    void save(json& obj) const
    {
        obj["brace_height"] = (double) brace_height;
        obj["draw_length"]  = (double) draw_length;
        obj["mass_arrow"]   = (double) mass_arrow;
    }
};

