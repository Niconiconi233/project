#include "HttpServer.h"
#include "../net/EventLoop.h"
#include "../Logging/base/ConfigReader.h"
#include "../Logging/base/SingLeton.h"

int main()
{
    //ConfigReader reader("ServerConfig.conf");
    auto& reader = Singletion<ConfigReader>::instance();
    EventLoop loop;
    HttpServer server(reader.getAsString("ip"), reader.getAsInt("port"), &loop, 4);
    server.enableGzip(reader.getAsBool("gzip"));
    server.init();
    loop.loop();
    return 0;
}
