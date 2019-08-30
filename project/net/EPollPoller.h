#ifndef MUDUO_NET_POLLER_EPOLLPOLLER_H
#define MUDUO_NET_POLLER_EPOLLPOLLER_H

#include <vector>
#include <unordered_map>
#include "Timestamp.h"

struct epoll_event;


#include "EventLoop.h"
#include "Channel.h"

class EPollPoller
{
    typedef std::vector<Channel*> ChannelList;
 public:

  EPollPoller(EventLoop* loop);
  ~EPollPoller();

  Timestamp poll(int timeoutMs, ChannelList* activeChannels);
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);

  bool hasChannel(Channel* ptr)
  {
      return channels_.find(ptr->fd()) != channels_.end();
  }

 private:
  static const int kInitEventListSize = 16;

  static const char* operationToString(int op);

  void fillActiveChannels(int numEvents,
                          ChannelList* activeChannels) const;
  void update(int operation, Channel* channel);

  typedef std::vector<struct epoll_event> EventList;

  int epollfd_;
  EventList events_;
  using ChannelMap = std::unordered_map<int, Channel*>;
  ChannelMap channels_;
};
#endif  // MUDUO_NET_POLLER_EPOLLPOLLER_H
