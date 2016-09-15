// Brent Search// Created by aditya gautam on 3/25/16.//#include "BrentSearch.hpp"#include "GridSearch.hpp"#include <math.h>#ifdef BAZEL#include "Algorithms/AlgorithmOptions.hpp"#include "Models/LinearMixedModel.hpp"#else#include "AlgorithmOptions.hpp"#include "../Models/LinearMixedModel.hpp"#endif// Default parametersBrentSearch::BrentSearch() {    a = default_a;    b = default_b;    c = default_c;    m = default_m;    e = default_e;    t = default_t;    delta = default_delta; // Default window size = 1}BrentSearch::BrentSearch(const unordered_map<string, string>& opts) {    try {        a = stod(opts.at("a"));    } catch (std::out_of_range& oor) {        a = default_a;    }    try {        b = stod(opts.at("b"));    } catch (std::out_of_range& oor) {        b = default_b;    }    try {        c = stod(opts.at("c"));    } catch (std::out_of_range& oor) {        c = default_c;    }    try {        m = stod(opts.at("m"));    } catch(std::out_of_range& oor) {        m = default_m;    }    try {        e = stod(opts.at("e"));    } catch(std::out_of_range& oor) {        e = default_e;    }    try {        t = stod(opts.at("t"));        } catch(std::out_of_range& oor) {        t = default_t;    }    try {        delta = stod(opts.at("delta"));    } catch (std::out_of_range& oor) {        delta = default_delta;    }}// Setters and Getters Methodsvoid BrentSearch::set_a(double a) {    this->a = a;}void BrentSearch::set_b(double b) {    this->b = b;}void BrentSearch::set_c(double c) {    this->c = c;}void BrentSearch::set_m(double m) {    this->m = m;}void BrentSearch::set_e(double e) {    this->e = e;}void BrentSearch::set_t(double t) {    this->t = e;}// Defines the window of search i.e set a and b.void BrentSearch::set_delta(double delta) {    this->delta = delta;}// Stores the minimum value of the cost function obtainedvoid BrentSearch::set_min_cost_val(double min_cost) {    this->min_val = min_cost;}// Stored the best parameter which gives the least cost func. val.void BrentSearch::set_best_param_val(double best_val) {    this->min_val_param = best_val;}double BrentSearch::get_a() {    return this->a;}double BrentSearch::get_b() {    return this->b;}double BrentSearch::get_c() {    return this->c;}double BrentSearch::get_m() {    return this->m;}double BrentSearch::get_e() {    return this->e;}double BrentSearch::get_t() {    return this->t;}double BrentSearch::get_delta() {    return this->delta;}double BrentSearch::get_min_cost_val() {    return this->min_val;}// Stored the best parameter which gives the least cost func. val.double BrentSearch::get_best_param_val() {    return this->min_val_param;}// Get all the basic brent parameteres in vector formvector<double> BrentSearch::get_brent_params() {    vector<double> params;    params.push_back(get_a());    params.push_back(get_b());    params.push_back(get_c());    params.push_back(get_m());    params.push_back(get_e());    params.push_back(get_t());    return params;}void BrentSearch::run(Model* model) {    if(!model) {        return;    } else if (LinearMixedModel* model = dynamic_cast<LinearMixedModel*>(model)) {        run(model);    } else {        throw runtime_error("Bad type of model for Algorithm: BrentSearch");    }}/* Brent's search for finding a global minimum between the given interval [a,b]. *    Parameters: a, b -> Starting point and the end point of the search c -> Prior knowledge about the global minimum point m -> Bound on second derivative (keep it to 0 as of now) e -> positive tolerance parameter t -> Positive error tolerance. x -> Value at which the objective function is attaining the minimal value. This Method and functions is inspired from Brent search : https://en.wikipedia.org/wiki/Brent%27s_method */void BrentSearch::run(LinearMixedModel *model) {    GridSearch *gridSearch = new GridSearch();    gridSearch->set_lambda_params(0.0,0.1,0.001);    gridSearch->run(model);    double a0, a2, a3, d0, d1, d2, h, m2, macheps, p, q, qs, r, s, sc, y, y0, y1, y2, y3, yb, z0, z1, z2;    double a, b, c, m, e, t, x = 0.0, best_grid_search_val = 0.0;    int k;    vector<double> params = this->get_brent_params();    // Extract the parameters from vector    a = params.at(0);    b = params.at(1);    c = params.at(2);    m = params.at(3);    e = params.at(4);    t = params.at(5);    best_grid_search_val = model->get_lambda();    a = best_grid_search_val - this->get_delta();    b = best_grid_search_val + this->get_delta();    a0 = b;    x = a0;    a2 = a;    y0 = model->f(b);    yb = y0;    y2 = model->f(a);    y = y2;    if (y0 < y) {        y = y0;    }    else {        x = a;    }    if (m < 0.0 || b <= a) {        model->set_lambda(x);        model->calculate_beta(x);        return;    }    macheps = t * t;    m2 = 0.5 * (1.0 + 16.0 * macheps) * m;    if (c <= a || b <= c) {        sc = 0.5 * (a + b);    }    else {        sc = c;    }    y1 = model->f(sc);    k = 3;    d0 = a2 - sc;    h = 9.0 / 11.0;    if (y1 < y) {        x = sc;        y = y1;    }    int count = 0;    while (1) {        d1 = a2 - a0;        d2 = sc - a0;        z2 = b - a2;        z0 = y2 - y1;        z1 = y2 - y0;        r = d1 * d1 * z0 - d0 * d0 * z1;        p = r;        qs = 2.0 * (d0 * z1 - d1 * z0);        q = qs;        if (k < 1000000 || y2 <= y) {            while (1) {                if (q * (r * (yb - y2) + z2 * q * ((y2 - y) + t)) <                    z2 * m2 * r * (z2 * q - r)) {                    a3 = a2 + r / q;                    y3 = model->f(a3);                    if (y3 < y) {                        x = a3;                        y = y3;                    }                }                k = ((1611 * k) % 1048576);                q = 1.0;                r = (b - a) * 0.00001 * (double) (k);                if ((z2 <= r) || ::isnan(z2)) {                    break;                }            }        }        else {            k = ((1611 * k) % 1048576);            q = 1.0;            r = (b - a) * 0.00001 * (double) (k);            while (r < z2) {                if (q * (r * (yb - y2) + z2 * q * ((y2 - y) + t)) <                    z2 * m2 * r * (z2 * q - r)) {                    a3 = a2 + r / q;                    y3 = model->f(a3);                    if (y3 < y) {                        x = a3;                        y = y3;                    }                }                k = ((1611 * k) % 1048576);                q = 1.0;                r = (b - a) * 0.00001 * (double) (k);            }        }        r = m2 * d0 * d1 * d2;        s = sqrt(((y2 - y) + t) / m2);        h = 0.5 * (1.0 + h);        p = h * (p + 2.0 * r * s);        q = q + 0.5 * qs;        r = -0.5 * (d0 + (z0 + 2.01 * e) / (d0 * m2));        if (r < s || d0 < 0.0) {            r = a2 + s;        }        else {            r = a2 + r;        }        if (p * q > 0) {            a3 = a2 + p / q;        }        else {            a3 = r;        }        while (1) {            a3 = max(a3, r);            if (std::isnan(a3))                break;            if (b <= a3) {                a3 = b;                y3 = yb;            } else {                y3 = model->f(a3);            }            if (y3 < y) {                x = a3;                y = y3;            }            d0 = a3 - a2;            if (a3 <= r) {                break;            }            p = 2.0 * (y2 - y3) / (m * d0);            if ((1.0 + 9.0 * macheps) * d0 <= abs(p)) {                break;            }            if (0.5 * m2 * (d0 * d0 + p * p) <= (y2 - y) + (y3 - y) + 2.0 * t) {                break;            }            a3 = 0.5 * (a2 + a3);            h = 0.9 * h;        }        if ((b <= a3) || ::isnan(a3)) {            break;        }        a0 = sc;        sc = a2;        a2 = a3;        y0 = y1;        y1 = y2;        y2 = y3;    }    // Set the minimum cost value and the best params value in the Brent object.    this->set_min_cost_val(y);    this->set_best_param_val(x);    model->set_lambda(x);    model->calculate_beta(x);}