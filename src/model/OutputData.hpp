#pragma once
#include "DiscreteLimb.hpp"

struct BowStates
{
    std::vector<double> draw_length;
    std::vector<double> draw_force;

    /*
    pub time: Vec<f64>,
    pub draw_force: Vec<f64>,

    pub pos_limb: Vec<Vec<[f64; 2]>>,
    pub vel_limb: Vec<Vec<[f64; 2]>>,
    pub acc_limb: Vec<Vec<[f64; 2]>>,

    pub pos_string: Vec<Vec<[f64; 2]>>,
    pub vel_string: Vec<Vec<[f64; 2]>>,
    pub acc_string: Vec<Vec<[f64; 2]>>,
    */
};

struct BowSetup
{
    DiscreteLimb limb;
    double string_length;
};

struct OutputData
{
    BowSetup setup;
    BowStates statics;
    BowStates dynamics;
};
