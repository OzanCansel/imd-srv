# imd-srv
A tcp server application which allows clients to `put`, `remove` and `fetch` desired `key`-`value` pairs in-memory.

It is multithreaded and fully asynchronous server.

### Synopsis
```console
Allowed options:
  --help                produce help message
  --threads arg (=0)    Specify number of executor threads.
  --port arg (=9424)    Specify port number to listen.
```

### Commands
`imd-srv` reads stream line-by-line from its clients and performs required operations if it is a properly constructed command.
All commands are text based and a command starts with `opcode` which is a single character such as `+` and ends with `\n` character.

#### Put
Puts specified key value pairs in map.
```console
+ <msg_id> <key> <value>
```

#### Remove
Removes specified key from map.

``` console
- <msg_id> <key>
````

#### Fetch
Asks for value of the <key>

```console
? <msg_id> <key>
```

`imd-srv` responds as :
``` console
<msg_id> <value>
```

#### Example
Start `imd-srv` first and after connect it with `nc`.

``` console
nc 127.0.0.1 9424
+ 14552 os ubuntu-20.04
+ 14347 login_date 2022/08/18
? 15685 os
15685 ubuntu-20.04
? 87194 login_date
87194 2022/08/18
- 75834 login_date
? 71049 login_date
71049
```
