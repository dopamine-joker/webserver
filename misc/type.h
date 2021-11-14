#ifndef WEBSERVER_TYPE_H
#define WEBSERVER_TYPE_H

enum FD_STATUS{
    SERVER_READ = 0,
    SERVER_WRITE = 1,
};

enum ACTOR_MODEL {
    PROACTOR = 0,
    REACTOR = 1,
};

enum TRIGMODE {
    LT = 0,
    ET = 1,
};

#endif //WEBSERVER_TYPE_H
