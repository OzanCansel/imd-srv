# imd-srv
A tcp server application which allows clients to `put`, `remove` and `get` desired `key`-`value` pairs which are stored in-memory.

It is multithreaded and fully asynchronous server.

- Requires a C++17 compliant compiler
- `asio`, `thread` and `program options` components should be installed on the system.

Only tested on Ubuntu 20.04 with GCC 11.2.0 and Boost v1.74

### Synopsis
```console
Allowed options:
  --help                produce help message
  --threads arg (=0)    Specify number of executor threads.
  --port arg (=9424)    Specify port number to listen.
```

### Commands
`imd-srv` reads stream line-by-line from its clients and performs required operations if it is a properly constructed command.
All commands are text based and a command starts with `opcode` which is a word such as `put`, `get`, `remove` and ends with `\n` character.

#### Put
Puts specified `key` `value` pairs in map.
```console
put <msg_id> <key> <value>
# or responds with one of the errors below :
error error MISSING_REQ_ID
<msg_id> error MISSING_KEY
```

#### Remove
Removes specified `key` from map.

``` console
# If key doesn't exist or removed respond with :
remove <msg_id> <key>
# or responds with one of the errors below :
error error MISSING_REQ_ID
<msg_id> error MISSING_KEY
````

#### Get
Asks for the value of the `key`

```console
get <msg_id> <key>
```

`imd-srv` responds as :
``` console
# If key exists it responds with :
<msg_id> success <value>
# or responds with one of the errors below :
error error MISSING_REQ_ID
<msg_id> error MISSING_KEY
<msg_id> error NO_SUCH_A_KEY
```
#### Unknown command
```console
# If an unknown command is sent responds with :
error error INAPPROPRIATE_COMMAND
```

#### Example
Start `imd-srv` first and then connect to it with `nc`.

``` console
calynr@calynr-dev:~$ nc 127.0.0.1 9424
put 14552 os ubuntu-20.04
14552 success
put 14347 login_date 2022/08/20
14347 success
get 15685 os
15685 success ubuntu-20.04
get 87194 login_date
87194 success 2022/08/20
remove 75834 login_date
75834 success
get 71049 login_date
71049 error NO_SUCH_A_KEY
```
