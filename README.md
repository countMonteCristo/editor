# editor
Just another text editor in C++

There are a lot of different text editor implementations, some of them are good enough, some of them not. Here is yet another text editor written in C++
only for studying purposes (and also for fun!).

## Roadmap

- [x] Simple text editor
  - [x] Font
    - [x] Use character tiles from stored PNG file as a font
    - [x] Support monospaced fonts
  - [x] Base key bindings
  - [x] Scrolling
  - [x] Mouse support
- [ ] UI
  - [x] Panels for line numbers and cursor position
  - [x] Selection using keyborad and mouse
  - [x] History for undo/redo
  - [ ] Context Menu
  - [ ] Command mode
  - [ ] Multiple cursors
  - [ ] Wrap long lines
  - [ ] Tabs for editing several files simultaneously
- [ ] Tests

## Getting started
### Dependencies
* gcc or clang or other c++ compiler
* cmake
* pkg-config
* libconfig
* fontconfig
* SDL2
* SDL2_ttf

### Installation
1. Install dependencies:
```sh
$ sudo apt-get install gcc cmake libsdl2-dev libsdl2-ttf-dev libconfig++-dev libfontconfig1
```

2. Clone the repo:
```sh
git clone https://github.com/countMonteCristo/editor.git
```

3. Build project
```sh
cmake .
make
```

4. Run
```sh
./editor [TEXT_FILE]
```

### Key Bindings

| Key binding |              Action                |
|-------------|------------------------------------|
| CTRL+S      | Save file                          |
| CTRL+Z      | Undo                               |
| CTRL+Y      | Redo                               |
| CTRL+C      | Copy selected text to clipboard    |
| CTRL+V      | Paste selected text from clipboard |
| CTRL+X      | Cut selected text to clipboard     |
| CTRL+A      | Select whole text                  |
| ↑←↓→        | Navigation                         |
| PgUp        | Go up on one screen                |
| PgDown      | Go down on one screen              |
| CTRL+←      | Go to begin of current word        |
| CTRL+→      | Go to end of current word          |
| Home        | Go to begin of current line        |
| End         | Go to end of current line          |


### Features

There is a config file [editor.conf](https://github.com/countMonteCristo/editor/blob/main/editor.conf), where all configurable settings are stored.

## Contacts

Artem Shapovalov: artem_shapovalov@frtk.ru

Project Link: https://github.com/countMonteCristo/editor

## Acknowledgments
* SDL2 [wiki](https://wiki.libsdl.org/)
* SDL2_ttf [man](https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf.pdf)
* libconfig [wiki](https://hyperrealm.github.io/libconfig/libconfig_manual.html)
* fontconfig [wiki](https://www.freedesktop.org/wiki/Software/fontconfig/)
* How to get font by name using fontconfig from [stackoverflow](https://stackoverflow.com/questions/10542832/how-to-use-fontconfig-to-get-font-list-c-c)
