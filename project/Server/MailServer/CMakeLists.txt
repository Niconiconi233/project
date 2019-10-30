cmake_minimum_required(VERSION 3.0)

project(Mailserver)

add_executable(MailServer main.cpp MailServer.cpp ../HttpContext.cpp ../HttpResponse.cpp ../Redis.cpp ../RedisConnectionPool.h)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
target_link_libraries(MailServer pthread)
target_link_libraries(MailServer log)
target_link_libraries(MailServer net)
target_link_libraries(MailServer python2.7)
target_link_libraries(MailServer hiredis)