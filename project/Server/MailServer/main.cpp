//
// Created by soft01 on 2019/8/15.
//

#include "../../net/EventLoop.h"
#include "../../Logging/base/SingLeton.h"
#include "../../Logging/base/ConfigReader.h"
#include "MailServer.h"

int main()
{
    auto& reader = Singletion<ConfigReader>::instance();
    EventLoop loop;
    MailServer server(reader.getAsString("mail_ip"), reader.getAsInt("mail_port"), &loop);
    loop.loop();
}
