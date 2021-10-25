#include "ClientThread.h"
#include "Messages.h"

#include <iostream>

ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::
ThreadBody(std::string ip, int port, int id, int orders, int type) {
	customer_id = id;
	num_orders = orders;
	laptop_type = type;
	if (!stub.Init(ip, port)) {
		std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
	}
    if(laptop_type == 1) {
        for (int i = 0; i < num_orders; i++) {
            LaptopOrder order;
            LaptopInfo laptop;
            order.SetOrder(customer_id, i, laptop_type);

            timer.Start();
            laptop = stub.OrderLaptop(order);
            timer.EndAndMerge();

            if (!laptop.IsValid()) {
                std::cout << "Invalid laptop " << customer_id << std::endl;
                break;
            }
        }
    }
    else if(laptop_type == 2)
    {
        LaptopOrder order;
        CustomerRecord customerRecord;
        order.SetOrder(customer_id, -1, laptop_type);
        timer.Start();
        customerRecord = stub.ReadRecord(order);
        std::cout<<customerRecord.GetCustomerId()<<"\t"<<customerRecord.GetLastOrder()<<std::endl;
        timer.EndAndMerge();
    }
    else if(laptop_type == 3)
    {
        for (int i = 0; i < num_orders; i++) {
            LaptopOrder order;
            CustomerRecord record;
            order.SetOrder(i, -1, laptop_type);

            timer.Start();
            record = stub.ReadRecord(order);
            if(record.GetCustomerId()!=-1){
                std::cout<<record.GetCustomerId()<<"\t"<<record.GetLastOrder()<<std::endl;
            }
            timer.EndAndMerge();

        }
    }
}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;	
}

