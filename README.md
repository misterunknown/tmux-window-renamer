# tmux-window-renamer

This tool can rename tmux windows while a parent command is running. The
intented use case is to rename the current tmux window to the host name of a SSH
session.

## Compile

You have to have a C compiler, e.g. gcc. In Debian/Ubuntu you can compile this
program as follows:

```
$ sudo apt install git gcc
$ git clone https://github.com/misterunknown/tmux-window-renamer.git
$ cd tmux-window-renamer
$ gcc -o tmux-window-renamer tmux-window-renamer.c
$ sudo ln -s $(realpath tmux-window-renamer) /usr/local/bin/
```

## Usage with ssh

Move or link the tool to `/usr/local/bin/tmux-window-renamer` and use this ssh
config (~/.ssh/config):

```
Match Host * exec "test $_ = /usr/bin/ssh"
    LocalCommand /usr/local/bin/tmux-window-renamer %h
```

Now, if you connect to a ssh server, the tmux window gets renamed to the given
hostname, until you disconnect from the host.

## Test use case

To test if it works, you could also run these commands:

```
$ echo $$
11305
$ bash --init-file <(echo "/usr/local/bin/tmux-window-renamer MYFANCYNAME")
$ echo $$
11718
$ exit
```

If you are inside a tmux session, the window gets renamed to `MYFANCYNAME` while
the interactive bash runs.
