#include <iostream>
#include <uuid/uuid.h>

int main() {
    std::cout << "test uuid" << std::endl;

    int i,n;
    uuid_t uu[4];
    char buf[1024];
    struct timeval tv;
    
    uuid_generate(uu[0]);
    uuid_generate_random(uu[1]);
    uuid_generate_time(uu[2]);
    n = uuid_generate_time_safe(uu[3]);
    std::cout << "n = " << n << std::endl;
    for (i=0; i<4; i++) {
        uuid_unparse(uu[i], buf);
        std::cout << "uuid[" << i << "] = " << buf << std::endl;
    }
    uuid_time(uu[2], &tv);
    std::cout << "tv.tv_sec = " << tv.tv_sec << "tv.tv_usec = " << tv.tv_usec << std::endl;

    return 0;
}