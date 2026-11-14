from machine import Pin
import time

# Define GPIO pins for 2 Braille cells (6 pins per cell)
braille_pins = [
    [Pin(12, Pin.OUT), Pin(13, Pin.OUT), Pin(14, Pin.OUT), Pin(27, Pin.OUT), Pin(26, Pin.OUT), Pin(25, Pin.OUT)],  # Cell 1
    [Pin(33, Pin.OUT), Pin(32, Pin.OUT), Pin(21, Pin.OUT), Pin(19, Pin.OUT), Pin(18, Pin.OUT), Pin(5, Pin.OUT)]    # Cell 2
]

# Braille alphabet: a–z, 6-bit format (dots 1 to 6)
braille_map = {
    'a': 0b000001, 'b': 0b000011, 'c': 0b001001, 'd': 0b011001, 'e': 0b010001,
    'f': 0b001011, 'g': 0b011011, 'h': 0b010011, 'i': 0b001010, 'j': 0b011010,
    'k': 0b000101, 'l': 0b000111, 'm': 0b001101, 'n': 0b011101, 'o': 0b010101,
    'p': 0b001111, 'q': 0b011111, 'r': 0b010111, 's': 0b001110, 't': 0b011110,
    'u': 0b100101, 'v': 0b100111, 'w': 0b111010, 'x': 0b101101, 'y': 0b111101,
    'z': 0b110101
}

def display_char(cell, char):
    # Clear all dots first
    for pin in braille_pins[cell]:
        pin.value(0)

    if char in braille_map:
        pattern = braille_map[char]
        for i in range(6):
            bit = (pattern >> i) & 1
            braille_pins[cell][i].value(bit)

def display_text(text, delay=2):
    text = text.lower()
    i = 0
    while True:
        c1 = text[i] if i < len(text) else ' '
        c2 = text[i + 1] if i + 1 < len(text) else ' '
        print(f"Displaying: '{c1}' and '{c2}'")

        display_char(0, c1)
        display_char(1, c2)

        time.sleep(delay)
        i += 2
        if i >= len(text):
            i = 0  # loop back

# Example usage
text_to_display = "hi"
display_text(text_to_display)
