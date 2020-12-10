#pragma once
#include <iostream>
#include "GeoIndex.pb.h"
#include "math.h"
#include <cstdint>
#define sqr(a) ((a) * (a))

class Filter_Drivers
{
public:
    Filter_Drivers(){};
    Filter_Drivers(const Filter_Drivers &) = delete;
    Filter_Drivers &operator=(const Filter_Drivers &) = delete;
    ~Filter_Drivers(){};

    std::string create_order(std::vector<MailTaxiAPI::DriverInformation> drivers, MailTaxiAPI::Coordinate from)
    {
        MailTaxiAPI::DriverInformation optimal_driver;
        double min_duration = -1;
        routing tmp_routing;

        for (size_t i = 0; i < drivers.size(); i++)
        {
            tmp_routing = get_routing(std::vector<MailTaxiAPI::Coordinate>{from, drivers[i].coordinate()});
            if (min_duration == -1 || tmp_routing.duration < min_duration)
            {
                min_duration = tmp_routing.duration;
                optimal_driver = drivers[i];
            }
        }
        if (min_duration == -1)
            return "";
        return optimal_driver.token();
    };

private:
    struct routing
    {
        double distance;
        double duration;
    };
    routing get_routing(std::vector<MailTaxiAPI::Coordinate> coordinates)
    {
        double distance = 0;
        double duration = 0;
        if (coordinates.size() == 2)
            duration =  sqrt(sqr(coordinates[1].latitude() - coordinates[0].latitude()) +
                            sqr(coordinates[1].longitude() - coordinates[0].longitude()));
        return {distance, duration};
    };
};