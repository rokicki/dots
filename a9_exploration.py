def a(element: list[int], word: str):
    word += "A , "
    clip = element[:5]
    clip = clip[-4:] + [clip[0]]
    return clip + element[-4:], word

def b(element: list[int], word: str):
    word += "B , "
    clip = element[-5:]
    clip = clip[-4:] + [clip[0]]
    return element[:4] + clip, word

def ap(element: list[int], word: str):
    word += "A', "
    clip = element[:5]
    clip = [clip[4]] + clip[:4]
    return clip + element[-4:], word

def bp(element: list[int], word: str):
    word += "B', "
    clip = element[-5:]
    clip = [clip[4]] + clip[:4]
    return element[:4] + clip, word

def check(element: list[int], word: str):
    global order
    global states, state_list
    string = stringify(element)
    if string not in states.keys():
        states[string] = word
        state_list.append((element, word))
        order += 1

def stringify(element: list[int]):
    return "".join(str(element))

def get_word_length(word: str):
    length = 0
    for i in range(len(word)):
        sub_str = word[i:i + 1]
        if sub_str == "A" or sub_str == "B": length += 1
    return length
print("\n\n\n")
print("--------------------------------------")
print("|          Group definition          |")
print("--------------------------------------")
element, word = [0, 1, 2, 3, 4, 5, 6, 7, 8], ""
turned_state, turned_word = a(element, word)
print("", element, "Identity", "\n", turned_state, turned_word[:-2], "\n")

turned_state, turned_word = ap(element, word)
print("", element, "Identity", "\n", turned_state, turned_word[:-2], "\n\n\n")

print("Calculating group properties...\n\n")
states = {}
state_list = [(element, word)]
order = 0
while len(state_list) > 0:
    pulled = state_list.pop(0)
    element, word = pulled[0], pulled[1]

    turned_state, turned_word = a(element, word)
    check(turned_state, turned_word)

    turned_state, turned_word = b(element, word)
    check(turned_state, turned_word)

    turned_state, turned_word = ap(element, word)
    check(turned_state, turned_word)

    turned_state, turned_word = bp(element, word)
    check(turned_state, turned_word)
max_word_length = 0
max_word = ""
max_states = []
for element in states.keys():
    if get_word_length(states[element]) > max_word_length:
        max_word_length = get_word_length(states[element])
        max_word = states[element]
        max_states.clear()

    if get_word_length(states[element]) == max_word_length:
        max_states.append(element)

print("--------------------------------------")
print("|              Results               |")
print("--------------------------------------")
print("Order: %s" % order)

print("Caley graph diameter: %s" % max_word_length)
print("Elements with longest word minimum word length, and the words:")

for element in max_states:
    print(element, states[element][:-2])
