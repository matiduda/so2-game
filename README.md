> :warning: **Important information**<br><br>This game was built as a project for an **Operating systems II** university course. If one uses any part of this code in their project, they can expect to have it not be accepted. One of the possible consequences of that action would be a **fail in the course**.<br><br>Please be careful!

## TODO:

- [ ] Enemy client
- [ ] Bot client
- [ ] Bot client - run away from enemy
- [ ] Create dropped treasure after player death
- [ ] Server - add key behaviour:
    - [ ] B/b – dodanie jednej bestii w losowym miejscu labiryntu,
    - [x] c/t/T – dodanie nowej monety, skarbu, dużego skarbu w losowym miejscu labiryntu,
    - [x] Q/q – zakończenie gry.

- [x] Server - draw players on server view
- [x] Server - fix player info on disconnect

## How to build

To build the game you have to have `ncurses` library installed. You can do this by using the following command
```sh
sudo apt-get install libncurses5-dev
```

To build the game, simply run
```sh
make
```

And then run `server` and `client` programs from build directory

```sh
cd build/
./server
./client
```

<div style="text-align: center">
<b>Made with :purple_heart:</b><br>
</div>