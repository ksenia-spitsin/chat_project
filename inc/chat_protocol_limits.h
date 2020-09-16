#ifndef CHAT_PROTOCOL_LIMITS_H
#define CHAT_PROTOCOL_LIMITS_H

#define MAX_TOTAL_LEN_MSG 256

#define MAX_LEN_CHAT 1024
#define MAX_LEN_IP 15
#define MAX_LEN_PORT 5
#define MAX_LEN_USERNAME 16
#define MAX_LEN_PASSWORD 16
#define MAX_LEN_GROUP_NAME 32
#define MAX_LEN_GROUP_LIST (MAX_TOTAL_LEN_MSG - 4)

#define MAX_TOTAL_LEN_CHAT MAX_LEN_CHAT + MAX_LEN_USERNAME

#define MAX_GROUPS 300

#endif /* CHAT_PROTOCOL_LIMITS_H */
