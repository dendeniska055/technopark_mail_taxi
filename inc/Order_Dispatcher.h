#include <iostream>
#include <assert.h>
#include <string>
#include <vector>

#include "Map_API.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

enum order_status 
{
    PIN,
    OFFER,
    ORDER,
    FIND,
    ACCEPTED,
    WAY,
    DONE,
    CANCELED
};

class Order_Dispatcher
{
public:
    Order_Dispatcher();
    Order_Dispatcher(long order_id);
    Order_Dispatcher(const Order_Dispatcher &) = delete;
    Order_Dispatcher &operator=(const Order_Dispatcher &) = delete;
    ~Order_Dispatcher(){};

    void change_order_status(point form, point to, enum order_status status);
    enum order_status check_status();
    void confirm_drivers_order(long driver_id, bool confirm);

private:
    long order_id;
    enum order_status status;
    void new_order(long user_id); // criteria
    void pin_order(point form, long user_id); // criteria
    void offer_order(point form, point to);
    void canceled_order(long user_id);
    void done_order();
    void remove_from_dump_order();
};