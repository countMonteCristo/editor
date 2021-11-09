#ifndef KEYBOARD_HPP_
#define KEYBOARD_HPP_

class Keyboard {
public:
    explicit Keyboard() = delete;
    explicit Keyboard(const Keyboard&) = delete;
    void operator=(const Keyboard&) = delete;

    static bool shift_pressed();
    static bool ctrl_pressed();
};

#endif // KEYBOARD_HPP_
