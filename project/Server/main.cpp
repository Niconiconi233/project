//
// Created by soft01 on 2019/8/5.
//

#include "HttpServer.h"
#include "../net/EventLoop.h"
#include "../Logging/base/ConfigReader.h"
#include "../Logging/base/SingLeton.h"

int main()
{
    auto& reader = Singletion<ConfigReader>::instance();
    EventLoop loop;
    HttpServer server(reader.getAsString("ip"), reader.getAsInt("port"), &loop, 0);
    server.init();
    loop.loop();
    return 0;
}
