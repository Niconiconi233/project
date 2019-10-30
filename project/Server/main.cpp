//
// Created by soft01 on 2019/8/5.
//

#include "HttpServer.h"
#include "../net/EventLoop.h"
#include "../Logging/base/ConfigReader.h"
#include "../Logging/base/SingLeton.h"
//#include "MailServer/MailServer.h"

int main()
{
    auto& reader = Singletion<ConfigReader>::instance();
    EventLoop loop;
    HttpServer mainserver(reader.getAsString("ip"), reader.getAsInt("port"), &loop, 3);
    //MailServer mailserver(reader.getAsString("mail_ip"), reader.getAsInt("mail_port"), &loop);
    mainserver.init();
    loop.loop();
    return 0;
}
