# parodus2fs
A simple adapter that lets you access files via Xmidt.

# Building and Testing Instructions

```
mkdir build
cd build
cmake ..
make
make test
```

# Cloud API

The interface today is quite simple.  There are 6 endpoints that only support
the Xmidt CRUD method (retrieve).

## Retrieve - /fs/

This endpoint provides all the configuration values and a stripped down document
that covers how to use the other 4 endpoints.

### Possible WRP (CRUD) Status Codes
* `200` - Success
* `500` - Unable to allocate the needed memory

### Successful Response Paylod (JSON)

```JSON
{
    "config": {
        "read-notify": false,
        "max-bytes-to-transfer": 1000,
        "receive-timeout": 10000
    },
    "head": {
        "desc": "Transfer up to the max-bytes-to-transfer maximum starting with the head and truncating the tail, if needed.  A binary copy of the file is returned."
    },
    "tail": {
        "desc": "Transfer up to the max-bytes-to-transfer maximum truncating the head of the file to preserve the tail, if needed.  A binary copy of the file is returned."
    },
    "full": {
        "desc": "Only transfer the file if the entire file can be transfered without truncation.  A binary copy of the file is returned."
    },
    "ls": {
        "desc": "Transfer the directory listing as a JSON list in random order.  If the list is truncated then the \"partial\" boolean parameter (omitted unless needed) shall be set to true.  The list is returned with the name \"list\".  The max-bytes-to-transfer parameter governs the truncation size."
    }
}
```

## Retrieve - /fs/config

TBD - you can get it from the `/fs/` endpoint above.

## Retrieve - /fs/ls/{directory path}

This enpoint lets you get a simple directory listing.  The listing comes back
in beautiful JSON like follows.  It, like all of the responses are limited by
the `max-bytes-to-transfer` configuration value.  This helps prevent mistakes
where you could ask for a file that is 10GB and not realize it.

#### Possible WRP (CRUD) Status Codes
* `200` - Success
* `403` - You don't have permissions on the local filesystem to do this
* `404` - The `{directory path}` you specified either doesn't exist or isn't a directory
* `500` - Unable to allocate the needed memory

#### Successful Response Paylod (JSON)

Here is the result I got from running the request: `/fs/ls/` on my laptop:

```JSON
{
    "list": [
        "..",
        "lost+found",
        "etc",
        "lib",
        "dev",
        "var",
        "opt",
        "proc",
        "boot",
        "sbin",
        "run",
        "home",
        "sys",
        "lib64",
        "mnt",
        "tmp",
        "media",
        "srv",
        "usr",
        "bin",
        ".",
        "root"
    ]
}
```

## Retrieve - /fs/full/{file path}

This enpoint gets exactly the entire file or nothing.  If the file you ask for
is too big to fit in a buffer governed by `max-bytes-to-transfer` you'll get
nothing back.

#### Possible WRP (CRUD) Status Codes
* `200` - Success
* `403` - You don't have permissions on the local filesystem to do this
* `404` - The `{file path}` you specified either doesn't exist or isn't a regular file
* `413` - The file is too big
* `500` - Unable to allocate the needed memory

#### Successful Response Paylod (binary/octet-stream)

The payload is the data exactly as it is on the filesystem.

## Retrieve - /fs/head/{file path}

This endpoint lets you get the **first** `max-bytes-to-transfer` of a file.  It could
be the entire file or just a part.  It's very similar to the program `head` except
today you don't have control of it.

#### Possible WRP (CRUD) Status Codes
* `200` - Success
* `403` - You don't have permissions on the local filesystem to do this
* `404` - The `{file path}` you specified either doesn't exist or isn't a regular file
* `500` - Unable to allocate the needed memory

#### Successful Response Paylod (binary/octet-stream)

The payload is the data exactly as it is on the filesystem, though it may be
truncated.

## Retrieve - /fs/tail/{file path}

This endpoint lets you get the **last** `max-bytes-to-transfer` of a file.  It could
be the entire file or just a part.  It's very similar to the program `tail` except
today you don't have control of it.

#### Possible WRP (CRUD) Status Codes
* `200` - Success
* `403` - You don't have permissions on the local filesystem to do this
* `404` - The `{file path}` you specified either doesn't exist or isn't a regular file
* `500` - Unable to allocate the needed memory

#### Successful Response Paylod (binary/octet-stream)

The payload is the data exactly as it is on the filesystem, though it may be
truncated.


## Delete - /fs/full/{file path}

This enpoint deletes the entire file or nothing.

#### Possible WRP (CRUD) Status Codes
* `200` - Success
* `403` - You don't have permissions on the local filesystem to do this
* `404` - The `{file path}` you specified either doesn't exist or isn't a regular file
* `500` - Unable to allocate the needed memory

#### Successful Response Paylod

The payload is empty.


