#pragma once
#include <utility>

std::pair<double, double> getQuotes(double weighted_midprice, double midprice);

int sendOrders(std::pair<double, double> quotes);