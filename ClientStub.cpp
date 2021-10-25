#include "ClientStub.h"

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);	
}


CustomerRecord ClientStub::ReadRecord(LaptopOrder order) {
    CustomerRecord record;
    char buffer[32];
    int size;
    order.Marshal(buffer);
    size = order.Size();
    if (socket.Send(buffer, size, 0)) {
        size = record.Size();
        if (socket.Recv(buffer, size, 0)) {
            record.Unmarshal(buffer);
        }
    }
    return record;
}
LaptopInfo ClientStub::OrderLaptop(LaptopOrder order) {
	LaptopInfo info;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0)) {
		size = info.Size();
		if (socket.Recv(buffer, size, 0)) {
			info.Unmarshal(buffer);
		} 
	}
	return info;
}

